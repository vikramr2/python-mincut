from pygraph import PyGraph

nodes = ['a', 'b', 'c', 'd']
edges = []

for n in nodes:
    if n != 'a':
        edges.append(('a', n))
        edges.append((n, 'a'))

G = PyGraph(nodes, edges)

print(G.mincut("noi", "bqueue", False))