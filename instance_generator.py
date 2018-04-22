import math
import random
import sys


################################################################################

# Argument 1 [integer]: number of boxes
# Argument 2 [integer]: maximum box dimension

# Example:
# > python3 generator.py 1000 100

# Each box is represented by a tuple (l, w, h).

################################################################################


num_boxes = int(sys.argv[1])
max_box_dimension = int(sys.argv[2])

# Sanity check
if (len(sys.argv) != 3):
    exit("ERROR: Wrong number of arguments.")
if (num_boxes < 0 or max_box_dimension < 0):
    exit("ERROR: All arguments must be nonnegative.")

# Generate boxes
boxes = []
for i in range(num_boxes):
    boxes.append([random.randint(1, max_box_dimension), \
                  random.randint(1, max_box_dimension), \
                  random.randint(1, max_box_dimension)])

# Output
with open(str(num_boxes)+"_"+str(max_box_dimension), 'w') as f:
    for b in boxes:
        f.write(str(b[0])+" "+str(b[1])+" "+str(b[2])+"\n")
    f.close()
print(num_boxes, " boxes (max. dimension ", max_box_dimension, ")", sep='')
