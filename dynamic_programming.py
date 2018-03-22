import sys
import time


def index_of_score(score):
    for i in range(len(msh)):
        if (msh[i] == score):
            return i


start_time = time.time()
        
# Import data
blocks_prerotation = open(sys.argv[1]).read().splitlines()
blocks_prerotation = [block.strip().split(' ') for block in blocks_prerotation[1:]]
blocks_prerotation = [[int(i) for i in j] for j in blocks_prerotation]

# Make all possible rotations such that l <= w
blocks = []
for block in blocks_prerotation:
    i = block[0]
    j = block[1]
    k = block[2]
    blocks.append([min(i, j), max(i, j), k])
    blocks.append([min(i, k), max(i, k), j])
    blocks.append([min(j, k), max(j, k), i])

# Order the blocks
blocks = sorted(blocks, key=lambda block: block[0]*block[1], reverse=True)

# Initialize the solution array (maximum stack height, with i on top)
msh = [i[2] for i in blocks]

# Dynamic programming algorithm
for i in range(1, len(msh)):
    for j in range(0, i):
        if (blocks[i][0] < blocks[j][0]) and \
           (blocks[i][1] < blocks[j][1] and \
            msh[i] < msh[j]+blocks[i][2]):
            msh[i] = msh[j]+blocks[i][2]
            
# Retrace the solution
solution = []
stack_height = max(msh)
while (stack_height > 0):
    current_index = index_of_score(stack_height)
    current_block = blocks[current_index]
    solution.append(current_block)
    stack_height -= current_block[2]

# Print the solution
end_time = time.time()
for i in solution:
    print(i)
print("Stack height:", max(msh))
print("Elapsed time:", round(end_time-start_time, 3))
