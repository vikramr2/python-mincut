from pygraph import PyGraph

nodes = list(range(24))
edges = []

for n in range(6):
    for v in range(4):
        for w in range(v+1, 4):
            edges.append((n*4+v, n*4+w))
            edges.append((n*4+w, n*4+v))
    
    if n != 5:
        edges.append((n*4+3, (n+1)*4))
        edges.append(((n+1)*4, n*4+3))
    else:
        edges.append((n*4+3, 0))
        edges.append((0, n*4+3))

# Create the igraph
G = PyGraph(nodes, edges)

print(G.mincut("cactus", "bqueue", True))