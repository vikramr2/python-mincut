import sys
sys.path.append('../build')
from mincut_wrapper import *

class Graph:
    def __init__(self, nodes_=[], edges_=[]):
        self.nodes = nodes_
        self.edges = edges_
    
    def add_node(self, n):
        self.nodes.append(n)

    def add_edge(self, u, v):
        if u not in self.nodes:
            self.nodes.append(u)
        if v not in self.nodes:
            self.nodes.append(v)
        self.edges.append((u, v))
    
    def as_CGraph(self):
        cgraph_nodes = list(range(len(self.nodes)))
        cgraph_edges = list(map(lambda t: (self.nodes.index(t[0]), self.nodes.index(t[1])), self.edges))

        return CGraph(cgraph_nodes, cgraph_edges)