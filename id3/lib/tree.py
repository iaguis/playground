class Node:
    def __init__(self, pos_examples, neg_examples, attribute, info_gain,
            left_value=True, right_value=False, leaf=False):
        self.pos_ex = pos_examples
        self.neg_ex = neg_examples
        self.attr = attribute
        self.gain = info_gain
        self.left_val = left_value
        self.right_val = right_value
        self.leaf=leaf

        self.left = None
        self.right = None

    def __str__(self):
        ret = "Pos: " + str(self.pos_ex) + " Neg: " + str(self.neg_ex)
        if not self.leaf:
            ret = ret + "\nAttribute: " + "i=" + str(self.attr["i"]+1) + " c=" + str(self.attr["c"])
            ret = ret + "\nGain: " + format(self.gain, "1.4f")
        return ret

