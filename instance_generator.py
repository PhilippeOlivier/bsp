import math
import random
import sys


#################################### NOTES #####################################

# Argument 1: n
# Argument 2: k
# Argument 3: instance #
# Example:
# > python3 generator.py 200 5 1

# Each box is represented by a tuple (h, l, w), with l <= w. Boxes may *NOT* be
# rotated in any direction.

################################## PARAMETERS ##################################

# Boxes have an average height of 10 (independent of k).
k_3_box_heights = [3, 10, 17]
k_5_box_heights = [7, 13]
k_10_box_heights = [1, 2, 5, 19, 23]

# The maximum height of the stack is chosen so that stacks average 10 boxes
# each. Boxes have an average height of 10 so the stack height is maxed at 100.
max_height = 100

# The conflict parameter has a range of [0, 1]. Pairwise conflict probabilities:
# < 0.6: Below ~5%
# 0.7: ~12%
# 0.8: ~25%
# > 0.9: ~33%
conflict_parameter = 0.9

################################################################################


n = int(sys.argv[1])
num_k = int(sys.argv[2])
instance = sys.argv[3]

# Arguments
if (len(sys.argv) != 4):
    exit("ERROR: Wrong number of arguments.")

# Number of boxes
if (n not in [20, 50, 100, 200, 500, 1000]):
    exit("ERROR: Value n is not valid.")

# Number of unique heights
if (num_k not in [3, 5, 10]):
    exit("ERROR: Value k is not valid.")

# Unique heights
k = k_3_box_heights
if (num_k > 3):
    k += k_5_box_heights
if (num_k > 5):
    k += k_10_box_heights
k.sort()

# Generate boxes
boxes = []
for i in range(n):
    boxes.append([random.choice(k),
                   random.randint(math.floor(math.log(i+1))*(i+1),
                                  math.floor(math.log(i+1))*(i+1)+int(100000**conflict_parameter)),
                   random.randint(math.floor(math.log(i+1))*(i+1),
                                  math.floor(math.log(i+1))*(i+1)+int(100000**conflict_parameter))])
    if (boxes[i][1] > boxes[i][2]):
        boxes[i][1], boxes[i][2] = boxes[i][2], boxes[i][1]

# Compute conflict ratio
num_combinations = 0
num_conflicts = 0
for i in range(len(boxes)):
    for j in range(i):
        if not (((boxes[i][1] < boxes[j][1]) and (boxes[i][2] < boxes[j][2])) or \
                ((boxes[j][1] < boxes[i][1]) and (boxes[j][2] < boxes[i][2]))):
            num_conflicts += 1
        num_combinations += 1

# Output
with open(str(n)+"_"+str(num_k)+"_"+instance+".txt", "w") as f:
    for b in boxes:
        f.write(str(b[0])+" "+str(b[1])+" "+str(b[2])+"\n")
print("n=", n, ", k=", num_k, ", #", instance, ", ",
      round(float(num_conflicts)/float(num_combinations)*100, 2),
      "% pairwise conflicts", sep='')
