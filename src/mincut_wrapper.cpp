#include <ext/alloc_traits.h>
#include <omp.h>

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <tuple>

#include "algorithms/global_mincut/algorithms.h"
#include "algorithms/global_mincut/minimum_cut.h"
#include "common/configuration.h"
#include "common/definitions.h"
#include "data_structure/graph_access.h"
#include "data_structure/mutable_graph.h"
#include "io/graph_io.h"
#include "tlx/cmdline_parser.hpp"
#include "tlx/logger.hpp"
#include "tools/random_functions.h"
#include "tools/string.h"
#include "tools/timer.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

// typedef graph_access graph_type;
typedef mutable_graph graph_type;
typedef std::shared_ptr<graph_type> GraphPtr;

/** 
 * Container for mincut output
 * 
 * @param light_partition the nodes on one side of the mincut
 * @param heavy_partition the nodes on the other side of the mincut
 * @param cut_size the number of edges in the cut
 */
class MincutResult {
    std::vector<int> light_partition;
    std::vector<int> heavy_partition;
    int cut_size;

public:
    MincutResult(std::vector<int> light_, 
                 std::vector<int> heavy_,
                 int cut_) : light_partition(light_), heavy_partition(heavy_), cut_size(cut_) {}

    std::vector<int> get_light_partition() { return light_partition; }
    std::vector<int> get_heavy_partition() { return heavy_partition; }
    int get_cut_size() { return cut_size; }
};

/**
 * Container for the mincut input
 * 
 * @param nodes list of nodes, must be range from 0 to n (e.g. [0, 1, 2, 3 ... n])
 * @param edges edges must contain values from node list
 */
class CGraph {
    std::vector<int> nodes;
    std::vector<std::tuple<int, int> > edges;

public:
    CGraph(std::vector<int> nodes_, 
           std::vector<std::tuple<int, int> > edges_) : nodes(nodes_), edges(edges_) {}
    std::vector<int> get_nodes() { return nodes; }
    std::vector<std::tuple<int, int> > get_edges() { return edges; }

    /**
     * Get the connected components of the graph
     *
     * @returns a list of node-sets of each connected component
     */
    std::vector<std::unordered_set<int> > connected_components() {
        std::vector<std::vector<int> > adjList(nodes.size());
        for (const auto& edge : edges) {
            int u, v;
            std::tie(u, v) = edge;
            adjList[u].push_back(v);
            adjList[v].push_back(u);
        }

        std::vector<bool> visited(nodes.size(), false);
        std::vector<std::unordered_set<int> > components;

        for (int i = 0; i < nodes.size(); ++i) {
            if (!visited[i]) {
                std::unordered_set<int> component;
                DFS(i, adjList, visited, component);
                components.push_back(component);
            }
        }

        return components;
    }

private:
    /**
     * Depth first traversal of a graph component
     *
     * @param node the starting node of the DFS
     * @param adjList the graph adjacency list
     * @param visited list of already visited nodes
     * @param component set of node-ids belonging to the component 
     *
     * DFS insertets nodes into the component set as it runs
     */
    void DFS(int node, const std::vector<std::vector<int>>& adjList, std::vector<bool>& visited, std::unordered_set<int>& component) {
        visited[node] = true;
        component.insert(node);

        for (int neighbor : adjList[node]) {
            if (!visited[neighbor]) {
                DFS(neighbor, adjList, visited, component);
            }
        }
    }
};

/**
 * Loads VieCut mutable graph from CGraph object
 */
template <class Graph = graph_access>
static std::shared_ptr<Graph> readGraphWeighted(CGraph g) {
    std::shared_ptr<Graph> G = std::make_shared<Graph>();

    std::vector<int> nodes = g.get_nodes();
    std::vector<std::tuple<int, int> > edges = g.get_edges();

    uint64_t nmbNodes = nodes.size();
    uint64_t nmbEdges = edges.size();

    NodeID node_counter = 0;
    EdgeID edge_counter = 0;

    G->start_construction(nmbNodes, nmbEdges);

    for (int n : nodes) {
        NodeID node = G->new_node();
        node_counter++;
        G->setPartitionIndex(node, 0);
    }

    for (std::tuple<int, int> edge : edges) {
        NodeID u = std::get<0>(edge);
        NodeID v = std::get<1>(edge);

        G->new_edge(u, v, 1);
    }

    G->finish_construction();
    G->computeDegrees();

    return G;
} 

/**
 * Slightly modified snippet from VieCut/app/mincut.cpp that computes mincut
 * 
 * Rather than taking a METIS filepath as input, we take a CGraph
 * Rather than storing output in a file, we store it in a MinCut object
 */
MincutResult mincut(CGraph CG, std::string algorithm, std::string queue_type, bool balanced) {
    auto cfg = configuration::getConfig();
    // cfg->graph_filename = graph_filename;
    cfg->algorithm = algorithm;
    cfg->queue_type = queue_type;
    cfg->find_most_balanced_cut = balanced;
    cfg->save_cut = true;

    std::vector<int> light;
    std::vector<int> heavy;

    timer t;
    GraphPtr G = readGraphWeighted<graph_type>(CG);

    timer tdegs;

    random_functions::setSeed(0);

    NodeID n = G->number_of_nodes();
    EdgeID m = G->number_of_edges();

    auto mc = selectMincutAlgorithm<GraphPtr>(algorithm);

    t.restart();
    EdgeWeight cut;
    cut = mc->perform_minimum_cut(G);

    for (NodeID node : G->nodes()) {
        if (G->getNodeInCut(node)) {
            light.push_back(node);
        } else {
            heavy.push_back(node);
        }
    }

    free(mc);

    return MincutResult(light, heavy, cut);
}

PYBIND11_MODULE(mincut_wrapper, handle) {
    handle.doc() = "Mincut methods and it's objects used for input and output";

    // Export mincut method to Python module as a method
    handle.def("mincut", &mincut);

    // Export output class to Python module as an object
    py::class_<MincutResult>(
        handle, "MincutResult"
    )
    .def(py::init<std::vector<int>, std::vector<int>, int>())
    .def("get_light_partition", &MincutResult::get_light_partition)
    .def("get_heavy_partition", &MincutResult::get_heavy_partition)
    .def("get_cut_size", &MincutResult::get_cut_size);

    // Export input class to Python module as an object
    py::class_<CGraph>(
        handle, "CGraph"
    )
    .def(py::init<std::vector<int>, std::vector<std::tuple<int, int> > >())
    .def("get_nodes", &CGraph::get_nodes)
    .def("get_edges", &CGraph::get_edges)
    .def("connected_components", &CGraph::connected_components);
}