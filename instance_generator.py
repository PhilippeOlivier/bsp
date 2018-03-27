import math
import random
import sys


################################################################################

# Argument 1 [integer]: number of boxes
# Argument 2 [integer]: maximum box height
# Argument 3 [integer]: maximum stack height

# Example:
# > python3 generator.py 200 30 100

# Each box is represented by a tuple (h, l, w), with l <= w.

################################################################################


num_boxes = int(sys.argv[1])
max_box_height = int(sys.argv[2])
max_stack_height = int(sys.argv[3])

# Sanity check
if (len(sys.argv) != 4):
    exit("ERROR: Wrong number of arguments.")
if (num_boxes < 0 or \
    max_box_height < 0 or \
    max_stack_height < 0):
    exit("ERROR: All arguments must be nonnegative.")
if (max_box_height > max_stack_height):
    exit("ERROR: Argument max_box_height must be lower or equal to max_stack_height.")

# Generate boxes
boxes = []
for i in range(num_boxes):
    boxes.append([random.randint(1, max_box_height), \
                  random.randint(1, max_box_height), \
                  random.randint(1, max_box_height)])
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
with open(str(num_boxes)+"_"+ \
          str(max_box_height)+"_"+ \
          str(max_stack_height)+".txt", "w") as f:
    f.write(str(max_stack_height)+"\n")
    for b in boxes:
        f.write(str(b[0])+" "+str(b[1])+" "+str(b[2])+"\n")
print(num_boxes, " boxes (max. height ", max_box_height, \
      ") with ", round(float(num_conflicts)/float(num_combinations)*100, 2),
      "% pairwise conflicts. Maximum stack height of ", max_stack_height, \
      ".", sep='')
