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

class CGraph {
    std::vector<int> nodes;
    std::vector<std::tuple<int, int> > edges;

public:
    CGraph(std::vector<int> nodes_, 
           std::vector<std::tuple<int, int> > edges_) : nodes(nodes_), edges(edges_) {}
    std::vector<int> get_nodes() { return nodes; }
    std::vector<std::tuple<int, int> > get_edges() { return edges; }
};

MincutResult mincut(std::string graph_filename, std::string algorithm, std::string queue_type, bool balanced) {
    auto cfg = configuration::getConfig();
    cfg->graph_filename = graph_filename;
    cfg->algorithm = algorithm;
    cfg->queue_type = queue_type;
    cfg->find_most_balanced_cut = balanced;
    cfg->save_cut = true;

    std::vector<int> light;
    std::vector<int> heavy;

    timer t;
    GraphPtr G = graph_io::readGraphWeighted<graph_type>(graph_filename);

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

    return MincutResult(light, heavy, cut);
}

int main(int argn, char** argv) {
    std::cout << "Hello World!" << std::endl;
} 

PYBIND11_MODULE(mincut_wrapper, handle) {
    handle.doc() = "This is the module docs.";
    handle.def("mincut", &mincut);

    py::class_<MincutResult>(
        handle, "MincutResult"
    )
    .def(py::init<std::vector<int>, std::vector<int>, int>())
    .def("get_light_partition", &MincutResult::get_light_partition)
    .def("get_heavy_partition", &MincutResult::get_heavy_partition)
    .def("get_cut_size", &MincutResult::get_cut_size);

    py::class_<CGraph>(
        handle, "CGraph"
    )
    .def(py::init<std::vector<int>, std::vector<std::tuple<int, int> > >())
    .def("get_nodes", &CGraph::get_nodes)
    .def("get_edges", &CGraph::get_edges);
}