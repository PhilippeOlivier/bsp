## Box Stacking Problem

The Box Stacking Problem (BSP) consists of stacking boxes of various heights, lenghts, and widths so as to construct the tallest stack of boxes. Boxes can be rotated in any direction, and a box can be used multiple times. A box can only be stacked on top of another if its length and width are strictly lower than those of the box underneath.

The dynamic programming algorithm rotates the boxes in every direction such that (w.l.o.g.) their length is not greater than their width.

The constraint programming model solves a slightly different problem: that of packing *all* the (unrotated) boxes into as few stacks as possible, with stacks having a maximum height (essentially bin packing with conflicts).