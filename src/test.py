import sys
import os
sys.path.append('../build')
from mincut_wrapper import *

print(os.getcwd())
res = mincut("../graphs/small.metis", "vc", "bqueue", False)
print(res.get_light_partition())
print(res.get_heavy_partition())
print(res.get_cut_size())