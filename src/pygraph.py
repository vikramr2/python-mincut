import sys
sys.path.append('../build')
from mincut_wrapper import CGraph, MincutResult, mincut

class PyGraph:
    ''' Wrapper for CGraph to allow for generic node and edge lists

    Parameters
    ----------
    nodes: nodes can be of any type and do not need to be in order
    edges: values in edge list still need to be in the node list
    '''
    def __init__(self, nodes_=[], edges_=[]):
        self.nodes = nodes_
        self.edges = edges_
        self.compact_map = {k: v for v, k in enumerate(nodes_)}
    
    def add_node(self, n):
        self.nodes.append(n)

    def add_edge(self, u, v):
        if u not in self.nodes:
            self.nodes.append(u)
        if v not in self.nodes:
            self.nodes.append(v)
        self.edges.append((u, v))
    
    def as_CGraph(self):
        ''' Conversion to CGraph to be used for mincut computation '''
        cgraph_nodes = list(range(len(self.nodes)))
        cgraph_edges = list(map(lambda t: (self.compact_map[t[0]], self.compact_map[t[1]]), self.edges))

        return CGraph(cgraph_nodes, cgraph_edges)

    def mincut(self, algorithm, queue_implementation, balanced):
        ''' Python wrapper function for C++ mincut '''
        cg = self.as_CGraph()
        mc = mincut(cg, algorithm, queue_implementation, balanced)

        heavy = list(map(lambda t: self.nodes[t], mc.get_heavy_partition()))
        light = list(map(lambda t: self.nodes[t], mc.get_light_partition()))
        cut = mc.get_cut_size()

        return heavy, light, cut