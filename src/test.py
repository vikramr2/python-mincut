import os
import sys
from Graph import Graph
sys.path.append('../build')
from mincut_wrapper import *

print(os.getcwd())

# res = mincut("../graphs/small.metis", "noi", "bqueue", False)

nodes = [0, 1, 2, 3, 4, 5, 6, 8]
edges = [(0, 1), (0, 2), (0, 3), (0, 4), \
         (1, 0), (1, 2), (1, 3), \
         (2, 0), (2, 1), (2, 3), \
         (3, 0), (3, 1), (3, 2), (3, 8), \
         (4, 0), (4, 5), (4, 6), (4, 8), \
         (5, 4), (5, 6), (5, 8), \
         (6, 4), (6, 5), (6, 8), \
         (8, 3), (8, 4), (8, 5), (8, 6)]

G = Graph(nodes, edges)
CG = G.as_CGraph()

res = mincut(CG, "vc", "bqueue", False)

print(res.get_light_partition())
print(res.get_heavy_partition())
print(res.get_cut_size())