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
    [6, 8]
]

for i, edge in enumerate(edges):
    edges[i] = [edge[0]*2, edge[1]*2]

nodes = list(range(9))

for i, node in enumerate(nodes):
    nodes[i] = node*2

# VieCut settings
algo = 'cactus'
balanced = True
qimp = 'bqueue'

G = PyGraph(nodes, edges)

print(G.mincut(algo, qimp, balanced))