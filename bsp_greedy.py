################################################################################
#
# Argument 1 [string]: Path of instance
#
# Example:
# python3 bsp_greedy.py 50_100
#
# The BSP can be represented by a graph and solved as a variant of the maximum
# clique problem. This algorithm solves the BSP with greedy clique generation:
#
#   (1) Preprocessing. We build the adjacency matrix of a graph whose nodes
#   represent boxes. Two nodes (boxes) are connected with an edge if one of them
#   can be stacked on top of the other. The optimal solution is the clique for
#   which the sum of the heights of its nodes is maximal.
#
#   (2) Clique generation. A promising node to add to a clique has both a large
#   number of neighbors (it is more likely that this new clique could be
#   increased further) and a high height (a higher clique is better). Therefore,
#   each node has a score equal to the number of its neighbors times its height.
#   We use roulette wheel selection to choose the starting node of a clique and
#   to incrementally add nodes to this clique until it is not possible anymore
#   to do so.
#
#   (3) Halting. The algorithm is manually halted once a satisfactory solution
#   is found.
#
################################################################################


import numpy
import random
import sys


# The clique is a set of indices
def roulette_wheel(clique):
    valid_indices = []
    valid_scores = []
    for i in range(num_boxes):
        valid = True
        for j in clique:
            if (adjacency_matrix[i][j] == 0):
                valid = False
                break
        if (valid):
            valid_indices.append(i)
            valid_scores.append(node_scores[i])
    if (len(valid_indices) > 0):
        selection = random.randint(0, sum(valid_scores))
        clique.add(valid_indices[numpy.argmax(numpy.cumsum(valid_scores) >= \
                                              selection)])
        return True
    return False


# Import data
boxes_prerotation = open(sys.argv[1]).read().splitlines()
boxes_prerotation = [box.split() for box in boxes_prerotation]
boxes_prerotation = [[int(side) for side in box] for box in boxes_prerotation]

# Make all possible rotations such that (w.l.o.g.) w<=l
boxes = []
for box in boxes_prerotation:
    i = box[0]
    j = box[1]
    k = box[2]
    boxes.append((min(i, j), max(i, j), k))
    boxes.append((min(i, k), max(i, k), j))
    boxes.append((min(j, k), max(j, k), i))
num_boxes = len(boxes)

# Build the adjacency matrix
adjacency_matrix = [[1 for _ in boxes] for _ in boxes]
for i in range(0, num_boxes, 3):
    adjacency_matrix[i][i] = 0
    adjacency_matrix[i][i+1] = 0
    adjacency_matrix[i][i+2] = 0
    adjacency_matrix[i+1][i] = 0
    adjacency_matrix[i+1][i+1] = 0
    adjacency_matrix[i+1][i+2] = 0
    adjacency_matrix[i+2][i] = 0
    adjacency_matrix[i+2][i+1] = 0
    adjacency_matrix[i+2][i+2] = 0
for i in range(num_boxes):
    for j in range(num_boxes):
        if not (((boxes[i][0] < boxes[j][0]) and \
                 (boxes[i][1] < boxes[j][1])) or \
                ((boxes[j][0] < boxes[i][0]) and \
                 (boxes[j][1] < boxes[i][1]))):
            adjacency_matrix[i][j] = 0
    
# Compute node scores
node_scores = [sum(adjacency_matrix[i])*boxes[i][2] for i in range(num_boxes)]

# Main loop
best_clique_height = 0
while (True):
    clique = set()
    while (roulette_wheel(clique)):
        pass
    clique_height = sum(boxes[i][2] for i in clique)
    if (clique_height > best_clique_height):
        best_clique_height = clique_height
        print("Current tallest stack:", best_clique_height)
