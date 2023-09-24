from pygraph import PyGraph

'''
This case was originally failing in CM++, 
so we're checking that this works
'''

def make_undir(edges):
    ''' Add reverse edges as well into the graph 
    
    This will switch the graph from directed to undirected
    '''
    undirected_edges = set()

    for edge in edges:
        u, v = edge
        undirected_edges.add((u, v))
        undirected_edges.add((v, u))

    ret = list(undirected_edges)
    for i, edge in enumerate(ret):
        ret[i] = list(edge)

    return ret

# Test case 1: regular unbalanced cut using NOI
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

# Test case 2: Balanced Cactus on a directed graph
algo = 'cactus'
balanced = True

print('Case 2: Cactus Mincut Directed')
print('---')
print(G0.mincut(algo, qimp, balanced))
print('---')
print(G1.mincut(algo, qimp, balanced))

print()

# Test case 3: Balanced Cactus on an undirected graph
cluster0_edges = make_undir(cluster0_edges)
cluster1_edges = make_undir(cluster1_edges)

G0 = PyGraph(cluster0_nodes, cluster0_edges)
G1 = PyGraph(cluster1_nodes, cluster1_edges)

print('Case 3: Make Edges truly undirected for Cactus')
print('---')
print(G0.mincut(algo, qimp, balanced))
print('---')
print(G1.mincut(algo, qimp, balanced))