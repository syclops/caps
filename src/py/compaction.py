
import graph_tool

class TransitiveClosureVisitor(graph_tool.BFSVisitor):

    def __init__(self, label):
        self.label = label