import sys
import os
sys.path.append('../build')
from mincut_wrapper import *

print(os.getcwd())

res = mincut("../graphs/small.metis", "noi", "bqueue", False)
print(res.get_light_partition())
print(res.get_heavy_partition())
print(res.get_cut_size())

G = CGraph([1, 2, 3], [(1, 2), (1, 3)])
print(G.get_nodes())
print(G.get_edges())