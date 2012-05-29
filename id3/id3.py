#!/usr/bin/env python
import sys
import math
import lib.pydot as pydot
from lib.tree import *

# Read an example file
def read_examples(file):
    f = open(file, 'r')
    examples = []

    for line in f:
        example = []
        for letter in line:
            if letter != '\n':
                example.append(letter)
        examples.append(example)

    f.close()

    return examples

# Gets the entropy of a list of examples
def get_entropy(pos_examples, neg_examples):
    n_pos = len(pos_examples)
    n_neg = len(neg_examples)

    total = n_pos + n_neg

    if total == 0:
        prop_pos = 0
        prop_neg = 0
    else:
        prop_pos = n_pos/total
        prop_neg = n_neg/total

    if prop_pos == 0:
        pos_factor = 0
    else:
        pos_factor = -(prop_pos) * math.log(prop_pos, 2)

    if prop_neg == 0:
        neg_factor = 0
    else:
        neg_factor = -(prop_neg) * math.log(prop_neg, 2)

    return pos_factor + neg_factor

# Gets the sublist of an example list that satisfy a given attribute
def get_sublists(example_list, attr):
    true_sub_list = []
    false_sub_list = []
    for ex in example_list:
        if ex[attr["i"]] == attr["c"]:
            true_sub_list.append(ex)
        else:
            false_sub_list.append(ex)
    return true_sub_list, false_sub_list

# Gets the gain of an attribute
def get_gain(pos_examples, neg_examples, attr):
    entropy_s = get_entropy(pos_examples, neg_examples)

    n_pos = len(pos_examples)
    n_neg = len(neg_examples)

    total = n_pos + n_neg

    s_pos_true, s_pos_false = get_sublists(pos_examples, attr)

    s_neg_true, s_neg_false = get_sublists(neg_examples, attr)

    n_s_true = len(s_pos_true) + len(s_neg_true)

    n_s_false = len(s_pos_false) + len(s_neg_false)

    true_factor = -(n_s_true/total) * get_entropy(s_pos_true, s_neg_true)
    false_factor = -(n_s_false/total) * get_entropy(s_pos_false, s_neg_false)

    return entropy_s + true_factor + false_factor

# Gets the best attribute and its gain given a list of examples
def get_best_attribute(pos_examples, neg_examples, attributes):
    gains = []
    for attr in attributes:
        gains.append(get_gain(pos_examples, neg_examples, attr))

    index = gains.index(max(gains))

    return attributes[index], gains[index]

# Returns a trained tree with ID3 algorithm
def ID3(pos_examples, neg_examples, attributes):
    n_pos_ex = len(pos_examples)
    n_neg_ex = len(neg_examples)


    if n_pos_ex <= 30 or n_neg_ex <= 30:
        return Node(n_pos_ex, n_neg_ex, None, None, leaf=True)

    if len(attributes) == 0:
        return Node(n_pos_ex, n_neg_ex, None, None, leaf=True)

    A, gain = get_best_attribute(pos_examples, neg_examples, attributes)

    # Root node
    root = Node(n_pos_ex, n_neg_ex, A, gain)

    pos_true, pos_false = get_sublists(pos_examples, A)
    neg_true, neg_false = get_sublists(neg_examples, A)

    new_attributes = list(attributes)
    new_attributes.remove(A)

    if len(pos_true) == 0 or len(neg_true) == 0:
        root.left = Node(n_pos_ex, n_neg_ex, None, None, leaf=True)
    else:
        root.left = ID3(pos_true, neg_true, new_attributes)

    if len(pos_false) == 0 or len(neg_false) == 0:
        root.right = Node(n_pos_ex, n_neg_ex, None, None, leaf=True)
    else:
        root.right = ID3(pos_false, neg_false, new_attributes)

    return root

# Returns the proportion P of an example being positive
def classify(tree, example, verbose):
    while not tree.leaf:
        pos = tree.attr["i"]
        value = tree.attr["c"]
        if verbose:
            print("Node", "i =", pos+1, "\tc =", value)
        if example[pos] == value:
            tree = tree.left
        else:
            tree = tree.right
    return tree.pos_ex/(tree.pos_ex+tree.neg_ex)

# Function to get a graph for graphviz to draw the tree
def visit_tree(node, graph):
    if node == None:
        return
    if not node.leaf:
        graph.add_edge(pydot.Edge(node.__str__(), node.left.__str__(), label="Yes"))
        graph.add_edge(pydot.Edge(node.__str__(), node.right.__str__(), label="No"))
    visit_tree(node.left, graph)
    visit_tree(node.right, graph)

def usage():
    print("Usage:\n\t Training mode, just outputs the ID3 tree:\n\t\t./id3.py training positive_ex negative_ex\n\t Test mode, trains and classifies test_ex:\n\t\t./id3.py test positive_ex negative_ex test_ex")

def main():
    try:
    
        arglen = len(sys.argv)
        if arglen < 4:
            raise ValueError

        f_true_train = sys.argv[2]
        f_false_train = sys.argv[3]

        test_mode = False

        if sys.argv[1] == "test":
            if arglen != 5:
                raise ValueError
            test_mode = True
            test_file = sys.argv[4]
        elif sys.argv[1] == "training":
            if arglen != 4:
                raise ValueError
            f_true_train = sys.argv[2]
            f_false_train = sys.argv[3]

        true_train = read_examples(f_true_train)
        false_train = read_examples(f_false_train)

        # Generate list of possible attributes
        attributes = []
        for i in range(10):
            for j in ['A', 'C', 'G', 'T']:
                if i!= 5 and i != 6:
                    attributes.append({"i" : i, "c" : j})
        # Tree training
        tree = ID3(true_train, false_train, attributes)

        # Test mode
        if test_mode:
            test_examples = read_examples(test_file)
            pos_class, neg_class = 0, 0
            for example in test_examples:
                print("Instance:", ''.join(example))
                prop = classify(tree, example, True)
                print("Proportion:", format(prop, ".3f"), end=" ")
                if prop > 0.5:
                    pos_class += 1
                    print("\tYes")
                else:
                    neg_class += 1
                    print("\tNo")
                print()

            print("\nPositive classifications =", pos_class)
            print("Negative classifications =", neg_class)
        else:
            graph = pydot.Dot(graph_type='digraph')

            visit_tree(tree, graph)

            graph.write_png("output.png", prog="dot")

    except IOError:
        print("I/O error")
    except ValueError:
        print("Error in parameters")
        usage()
    except:
        print("Unexpected error:", sys.exc_info()[0])
        raise

if __name__ == "__main__":
    main()
