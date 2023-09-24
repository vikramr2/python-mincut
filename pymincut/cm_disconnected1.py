from pygraph import PyGraph

def make_undir(edges):
    undirected_edges = set()

    for edge in edges:
        u, v = edge
        undirected_edges.add((u, v))
        undirected_edges.add((v, u))

    ret = list(undirected_edges)
    for i, edge in enumerate(ret):
        ret[i] = list(edge)

    return ret

algo = 'noi'
qimp = 'bqueue'
balanced = False

cluster0_nodes = [0, 1, 2, 3, 4, 5]
cluster1_nodes = [6, 7, 8]

cluster0_edges = [
    [0, 1],
    [0, 2],
    [1, 2],
    [3, 4],
    [3, 5],
    [4, 5],
]

cluster1_edges = [
    [6, 7],
    [7, 8],
    [6, 8]
]

G0 = PyGraph(cluster0_nodes, cluster0_edges)
G1 = PyGraph(cluster1_nodes, cluster1_edges)

print('Case 1: NOI Mincut')
print('---')
print(G0.mincut(algo, qimp, balanced))
print('---')
print(G1.mincut(algo, qimp, balanced))

print()

algo = 'cactus'
balanced = True

print('Case 2: Cactus Mincut Directed')
print('---')
print(G0.mincut(algo, qimp, balanced))
print('---')
print(G1.mincut(algo, qimp, balanced))

print()

cluster0_edges = make_undir(cluster0_edges)
cluster1_edges = make_undir(cluster1_edges)

G0 = PyGraph(cluster0_nodes, cluster0_edges)
G1 = PyGraph(cluster1_nodes, cluster1_edges)

print(cluster0_edges)

print('Case 3: Make Edges truly undirected for Cactus')
print('---')
print(G0.mincut(algo, qimp, balanced))
print('---')
print(G1.mincut(algo, qimp, balanced))