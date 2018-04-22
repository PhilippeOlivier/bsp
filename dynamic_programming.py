import sys
import time


################################################################################

# Argument 1 [string]: Path to instance

# Example:
# > python3 dynamic_programming.py 1000_100

# Each box is represented by a tuple (l, w, h).

################################################################################


# Helper function to retrace the solution
def index_of_score(score):
    for i in range(len(msh)):
        if (msh[i] == score):
            return i


start_time = time.time()
        
# Import data
boxes_prerotation = open(sys.argv[1]).read().splitlines()
boxes_prerotation = [box.split() for box in boxes_prerotation]
boxes_prerotation = [[int(side) for side in box] for box in boxes_prerotation]

# Make all possible rotations such that l <= w
boxes = []
for box in boxes_prerotation:
    i = box[0]
    j = box[1]
    k = box[2]
    boxes.append([min(i, j), max(i, j), k])
    boxes.append([min(i, k), max(i, k), j])
    boxes.append([min(j, k), max(j, k), i])

# Order the boxes
boxes = sorted(boxes, key=lambda box: box[0]*box[1], reverse=True)

# Initialize the solution array (maximum stack height, with i on top)
msh = [i[2] for i in boxes]

# Dynamic programming algorithm
for i in range(1, len(msh)):
    for j in range(0, i):
        if (boxes[i][0] < boxes[j][0]) and \
           (boxes[i][1] < boxes[j][1] and \
            msh[i] < msh[j]+boxes[i][2]):
            msh[i] = msh[j]+boxes[i][2]
            
# Retrace the solution
solution = []
stack_height = max(msh)
while (stack_height > 0):
    current_index = index_of_score(stack_height)
    current_box = boxes[current_index]
    solution.append(current_box)
    stack_height -= current_box[2]

# Print the solution
# for i in solution:
#     print(i)
print("Stack height:", max(msh))
print("Elapsed time: ", round(time.time()-start_time, 3), "s", sep='')
