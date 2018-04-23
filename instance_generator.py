################################################################################
#
# Argument 1 [integer]: Number of boxes
# Argument 2 [integer]: Maximum box dimension
#
# Example:
# python3 instance_generator.py 50 100
#
# Each box is represented by a tuple of three dimensions.
#
################################################################################


import math
import random
import sys


num_boxes = int(sys.argv[1])
max_box_dimension = int(sys.argv[2])

# Sanity check
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
