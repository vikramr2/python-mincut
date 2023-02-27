# python-mincut
Python wrapper for VieCut (https://github.com/VieCut/VieCut)

## Compilation
Run the following commands from the root of this project
```bash
mkdir build
cd build
cmake .. && make
cd ..
```
## Usage
Import the `Graph` object and compute mincut as a member function
```Python
from graph import Graph

nodes = # Arbitrary node list of any data type (e.g. [1, 2, 3], [1, 'a', 0.5], etc.)
edges = # Edges are lists of 2-tuples (e.g. [(1, 2), (1, 3)], [(1, 'a'), (0.5, 1)], etc.)

G = Graph(nodes, edges)

''' Mincut Arguments

algorithm (str)            : name of algorithm used to compute mincut (e.g. noi, vc, cactus)
queue_implementation (str) : priority queue implementation (e.g. bqueue, bstack, heap)
balanced (bool)            : is the mincut balanced?
'''
light_partition, heavy_partition, cut_size = G.mincut("noi", "bqueue", False)
```
