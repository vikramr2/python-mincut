#include <ext/alloc_traits.h>
#include <omp.h>

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

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
};

int main(int argn, char** argv) {
    std::cout << "Hello World!" << std::endl;
} 