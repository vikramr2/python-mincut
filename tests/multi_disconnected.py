# In this test case, let's make three disconnected triangles
from pymincut.pygraph import PyGraph

edges = [
    [0, 1],
    [1, 2],
    [0, 2],
    [3, 4],
    [4, 5],
    [3, 5],
    [6, 7],
    [7, 8],
    [6, 8],
]
nodes = list(range(9))

# VieCut settings
algo = 'cactus'
balanced = True
qimp = 'bqueue'

G = PyGraph(nodes, edges)

print(G.mincut(algo, qimp, balanced))