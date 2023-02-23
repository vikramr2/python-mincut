/******************************************************************************
 * mincut_recursive.cpp
 *
 * Source of VieCut.
 *
 ******************************************************************************
 * Copyright (C) 2017-2019 Alexander Noe <alexander.noe@univie.ac.at>
 *
 * Published under the MIT license in the LICENSE file.
 *****************************************************************************/

#include <ext/alloc_traits.h>

#include <algorithm>
#include <cstdio>
#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#ifdef PARALLEL
#include "parallel/algorithm/parallel_cactus.h"
#else
#include "algorithms/global_mincut/cactus/cactus_mincut.h"
#endif
#include "algorithms/misc/strongly_connected_components.h"
#include "common/configuration.h"
#include "common/definitions.h"
#include "data_structure/graph_access.h"
#include "data_structure/mutable_graph.h"
#include "io/graph_io.h"
#include "tlx/cmdline_parser.hpp"
#include "tlx/logger.hpp"
#include "tools/graph_extractor.h"
#include "tools/timer.h"

int main(int argn, char** argv) {
    timer t;
    tlx::CmdlineParser cmdl;
    auto cfg = configuration::getConfig();
    bool output = false;
    cmdl.add_param_string("graph", cfg->graph_filename, "path to graph file");
    cmdl.add_bool('o', "output", output, "Write intermediate graphs to disk");
    cmdl.add_flag('v', "verbose", cfg->verbose, "Verbose output.");
    cfg->find_most_balanced_cut = true;
    cfg->save_cut = true;

    if (!cmdl.process(argn, argv))
        return -1;

    std::vector<graphAccessPtr> graphs;

    graphAccessPtr G =
        graph_io::readGraphWeighted(cfg->graph_filename);
    graphs.push_back(G);

#ifdef PARALLEL
    parallel_cactus<graphAccessPtr> mc;
#else
    cactus_mincut<graphAccessPtr> mc;
#endif
    LOG1 << "io time: " << t.elapsed();

    EdgeWeight cut = 0;
    timer t_this;
    size_t ct = 0;

    while (G->number_of_nodes() > 1000) {
        t_this.restart();
        auto [current_cut, mg, unused] = mc.findAllMincuts(G);
        (void)unused;
        NodeWeight largest_block = 0;
        NodeID largest_id = 0;
        for (NodeID n : mg->nodes()) {
            if (mg->containedVertices(n).size() > largest_block) {
                largest_block = mg->containedVertices(n).size();
                largest_id = n;
            }

            for (NodeID c : mg->containedVertices(n)) {
                G->setPartitionIndex(c, n);
            }
        }

        cut = current_cut;
        graph_extractor ge;
        strongly_connected_components scc;
        G = scc.largest_scc(ge.extract_block(G, largest_id).first);

        if (output) {
            std::string name = cfg->graph_filename + "_" + std::to_string(ct++);
            graph_io::writeGraphWeighted(G, name);
        }

        LOG1 << "Minimum cut " << cut;
        LOG1 << "-------------------------------";
        LOG1 << "Next graph: G=(" << G->number_of_nodes()
             << "," << G->number_of_edges() << ")";
    }
}
