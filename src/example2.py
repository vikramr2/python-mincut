from graph import Graph

nodes = ['a', 'b', 'c', 'd']
edges = []

for n in nodes:
    if n != 'a':
        edges.append(('a', n))
        edges.append((n, 'a'))

G = Graph(nodes, edges)

print(G.mincut("noi", "bqueue", False))