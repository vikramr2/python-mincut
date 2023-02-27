import sys
import os
sys.path.append('../build')
from mincut_wrapper import *

print(os.getcwd())

# res = mincut("../graphs/small.metis", "noi", "bqueue", False)

nodes = [0, 1, 2, 3, 4, 5, 6, 7]
edges = [(0, 1), (0, 2), (0, 3), (0, 4), \
         (1, 0), (1, 2), (1, 3), \
         (2, 0), (2, 1), (2, 3), \
         (3, 0), (3, 1), (3, 2), (3, 7), \
         (4, 0), (4, 5), (4, 6), (4, 7), \
         (5, 4), (5, 6), (5, 7), \
         (6, 4), (6, 5), (6, 7), \
         (7, 3), (7, 4), (7, 5), (7, 6)]

G = CGraph(nodes, edges)
print(G.get_nodes())
print(G.get_edges())

res = mincut(G, "noi", "bqueue", False)

print(res.get_light_partition())
print(res.get_heavy_partition())
print(res.get_cut_size())