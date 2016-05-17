__author__ = 'pdvalck'

import asciitree
import sys

from clang import cindex as ast
from pprint import pprint

def node_children(node):
    #return (c for c in node.get_children() if c.location.file.name == sys.argv[1])
    return (c for c in node.get_children())

def print_node(node):

    if node.kind.is_reference() and node.referenced is not None:
        node = node.referenced

    if node.kind == ast.CursorKind.MEMBER_REF_EXPR:
        node = node.get_definition()

    if node.kind == ast.CursorKind.DECL_REF_EXPR and node.get_definition() is not None:
        node = node.get_definition()

    text = node.spelling or node.displayname
    kind = str(node.kind)[str(node.kind).index('.')+1:]
    hash = node.hash

    return '{} {} {}'.format(kind, text, hash)

def pretty_node(node):

	print(asciitree.draw_tree(node, node_children, print_node))

def print_diag(tu):

    severity = 0

    for diag in tu.diagnostics:

        if diag.severity > 2:

            print 'Error at {}:'.format(diag.location)
            print '\t{}'.format(diag.spelling)
            print ''

            severity = 3

    if severity > 0:

        print 'Fatal error'
        sys.exit(0)

def find_node(node, name):

    text = node.spelling or node.displayname

    if text == name:
        return node
    else:
        for c in node.get_children():
            res = find_node(c, name)
            if res is not None:
                return res
        return None

def find_node_type(node, name, node_type):

    text = node.spelling or node.displayname

    if text == name and node.kind == node_type:
        return node
    else:
        for c in node.get_children():
            res = find_node_type(c, name, node_type)
            if res is not None:
                return res
        return None

def extract_literal(node):

    if node.kind == ast.CursorKind.INTEGER_LITERAL:
        return node.get_tokens().next().spelling
    else:
        for c in node.get_children():
            res = extract_literal(c)
            if res is not None:
                return res


def extract_variable(node, var):

    node = find_node_type(node, var, ast.CursorKind.VAR_DECL)

    if node is None:
        return 10
    else:
        return extract_literal(node)




