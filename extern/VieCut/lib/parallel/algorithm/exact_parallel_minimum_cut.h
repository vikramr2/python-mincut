/******************************************************************************
 * exact_parallel_minimum_cut.h
 *
 * Source of VieCut.
 *
 ******************************************************************************
 * Copyright (C) 2018 Alexander Noe <alexander.noe@univie.ac.at>
 *
 * Published under the MIT license in the LICENSE file.
 *****************************************************************************/

#pragma once

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <deque>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include "algorithms/global_mincut/minimum_cut_helpers.h"
#include "algorithms/global_mincut/noi_minimum_cut.h"
#include "algorithms/global_mincut/viecut.h"
#include "common/configuration.h"
#include "common/definitions.h"
#include "data_structure/graph_access.h"
#include "data_structure/priority_queues/fifo_node_bucket_pq.h"
#include "data_structure/priority_queues/maxNodeHeap.h"
#include "data_structure/priority_queues/node_bucket_pq.h"
#include "tools/random_functions.h"
#include "tools/timer.h"

#ifdef PARALLEL

#include "parallel/coarsening/contract_graph.h"
#include "parallel/coarsening/contraction_tests.h"
#include "parallel/coarsening/sparsify.h"
#include "parallel/data_structure/union_find.h"

#else
#include "coarsening/contract_graph.h"
#include "coarsening/contraction_tests.h"
#include "coarsening/sparsify.h"
#include "data_structure/union_find.h"
#endif

template <class GraphPtr>
class exact_parallel_minimum_cut : public minimum_cut {
 public:
    typedef GraphPtr GraphPtrType;
    exact_parallel_minimum_cut() { }
    ~exact_parallel_minimum_cut() { }

    static constexpr bool debug = false;
    bool timing = configuration::getConfig()->verbose;

    EdgeWeight perform_minimum_cut(GraphPtr G) {
        return perform_minimum_cut(G, false);
    }

    EdgeWeight perform_minimum_cut(GraphPtr G,
                                   bool indirect) {
        if (!G) {
            return -1;
        }

        std::vector<GraphPtr> graphs;
        timer t;
        EdgeWeight mincut = G->getMinDegree();
#ifdef PARALLEL
        viecut<GraphPtr> heuristic_mc;
        mincut = heuristic_mc.perform_minimum_cut(G, true);
        LOGC(timing) << "VieCut found cut " << mincut
                     << " [Time: " << t.elapsed() << "s]";
#endif

        graphs.push_back(G);

        // if PARALLEL is set, NodeInCut are already set to the result of viecut
        // This is what we want.
#ifndef PARALLEL
        minimum_cut_helpers<GraphPtr>::setInitialCutValues(graphs);
#endif

        while (graphs.back()->number_of_nodes() > 2 && mincut > 0) {
            GraphPtr curr_g = graphs.back();
            timer ts;
#ifdef PARALLEL

            noi_minimum_cut<GraphPtr> noi;

            auto uf = parallel_modified_capforest(curr_g, mincut);
            if (uf.n() == curr_g->number_of_nodes()) {
                uf = noi.modified_capforest(curr_g, mincut);
                LOGC(timing) << "seq capforest needed";
            }

#else
            LOG1 << "Error: Running exact_parallel_minimum_cut without PARALLEL"
                 << " Using normal noi_minimum_cut instead!";

            noi_minimum_cut noi;
            auto uf = noi.modified_capforest(curr_g, mincut);
#endif

            if (uf.n() > 1) {
                graphs.push_back(contraction::fromUnionFind(curr_g, &uf, true));

                mincut = minimum_cut_helpers<GraphPtr>::updateCut(
                    graphs, mincut);
            } else {
                break;
            }
        }

        if (!indirect && configuration::getConfig()->save_cut)
            minimum_cut_helpers<GraphPtr>::retrieveMinimumCut(graphs);

        return mincut;
    }

    std::vector<NodeID> randomStartNodes(GraphPtr G) {
        std::vector<NodeID> start_nodes;
        for (int i = 0; i < omp_get_max_threads(); ++i)
            start_nodes.push_back(
                random_functions::next() % G->number_of_nodes());

        return start_nodes;
    }

    std::vector<NodeID> bfsStartNodes(GraphPtr G) {
        NodeID starting_node = random_functions::next() % G->number_of_nodes();
        std::vector<NodeID> start_nodes;
        start_nodes.push_back(starting_node);

        for (int i = 1; i < omp_get_max_threads(); ++i) {
            std::deque<NodeID> bfs;
            std::vector<bool> nodes(G->number_of_nodes(), false);
            size_t found = i;

            for (auto el : start_nodes) {
                bfs.push_back(el);
                nodes[el] = true;
            }

            while (!bfs.empty() && found < G->number_of_nodes()) {
                NodeID no = bfs.front();
                bfs.pop_front();
                for (EdgeID e : G->edges_of(no)) {
                    NodeID tgt = G->getEdgeTarget(e);
                    if (!nodes[tgt]) {
                        found++;
                        nodes[tgt] = true;
                        bfs.push_back(tgt);
                        if (found == G->number_of_nodes()) {
                            start_nodes.push_back(tgt);
                            break;
                        }
                    }
                }
            }
        }
        return start_nodes;
    }

    union_find parallel_modified_capforest(
        GraphPtr G,
        const EdgeWeight mincut,
        const bool disable_blacklist = false) {
        union_find uf(G->number_of_nodes());
        timer t;

        timer timer2;
        std::vector<NodeID> start_nodes = randomStartNodes(G);

        // std::vector<bool> would be bad for thread-safety
        std::vector<uint8_t> visited(G->number_of_nodes(), false);
        std::vector<size_t> times(G->number_of_nodes(), 0);

#pragma omp parallel for
        for (int i = 0; i < omp_get_num_threads(); ++i) {
            fifo_node_bucket_pq pq(G->number_of_nodes(), mincut + 1);
            std::vector<bool> blacklisted(G->number_of_nodes(), false);
            std::vector<NodeID> r_v(G->number_of_nodes(), 0);
            std::vector<bool> local_visited(G->number_of_nodes(), false);

            NodeID starting_node = start_nodes[i];
            NodeID current_node = starting_node;

            pq.insert(current_node, 0);

            timer t;
            size_t elements = 0;

            while (!pq.empty()) {
                current_node = pq.deleteMax();
                elements++;
                local_visited[current_node] = true;

                if (!disable_blacklist) {
                    blacklisted[current_node] = true;
                    if (visited[current_node]) {
                        continue;
                    } else {
                        visited[current_node] = true;
                    }
                }

                for (EdgeID e : G->edges_of(current_node)) {
                    auto [tgt, wgt] = G->getEdge(current_node, e);
                    if (!local_visited[tgt]) {
                        if (r_v[tgt] < mincut) {
                            if ((r_v[tgt] + wgt) >= mincut) {
                                if (!blacklisted[tgt]) {
                                    uf.Union(current_node, tgt);
                                }
                            }

                            if (!visited[tgt]) {
                                size_t new_rv =
                                    std::min(r_v[tgt] + wgt,
                                             mincut);
                                r_v[tgt] = new_rv;
                                if (!visited[tgt] && !local_visited[tgt]) {
                                    if (pq.contains(tgt)) {
                                        pq.increaseKey(tgt, new_rv);
                                    } else {
                                        pq.insert(tgt, new_rv);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        return uf;
    }
};
