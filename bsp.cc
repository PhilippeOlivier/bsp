#include <chrono>
#include <ilcp/cp.h>
#include <iostream>


#define RC_EPS 1.0e-6


ILOSTLBEGIN


void LoadData(IloEnv env,
	      const char* filename,
	      IloInt& max_stack_height,
	      IloInt& num_boxes,
	      IloIntArray heights,
	      IloArray<IloIntArray> conflicts);


int main(int argc, char **argv) {
    IloEnv env;

    std::chrono::high_resolution_clock::time_point time_start =
	std::chrono::high_resolution_clock::now();

    // Data
    IloInt max_stack_height;
    IloInt num_boxes;
    IloIntArray heights = IloIntArray(env);
    IloArray<IloIntArray> conflicts = IloArray<IloIntArray>(env);
    LoadData(env, argv[1], max_stack_height, num_boxes, heights, conflicts);

    // Greedy first-fit solution
    IloArray<IloIntArray> greedy_stacks = IloArray<IloIntArray>(env);
    for (int i=0; i<num_boxes; i++) {
	bool packed = false;
	for (int j=0; j<greedy_stacks.getSize(); j++) {
	    bool conflict = false;
	    IloInt height = 0;
	    for (int k=0; k<greedy_stacks[j].getSize(); k++) {
		height += heights[greedy_stacks[j][k]];
		if (conflicts[i][k] == 1) {
		    conflict = true;
		}
	    }
	    if (height+heights[i] > max_stack_height) {
		conflict = true;
	    }
	    if (!conflict) {
		greedy_stacks[j].add(i);
		packed = true;
	    }
	    if (packed) {
		break;
	    }
	}
	if (!packed) {
	    greedy_stacks.add(IloIntArray(env, 1, i));
	}
    }

    std::cout << "Lower bound: "
	      << ceil((float)IloSum(heights)/(float)max_stack_height)
	      << std::endl;
    
    std::cout << "Greedy solution (first-fit): "
	      << greedy_stacks.getSize()
	      << std::endl;
    
    // CP model: Try an increasing number of stacks
    IloInt num_stacks = ceil((float)IloSum(heights)/(float)max_stack_height);
    while (true) {
	IloModel cp_model(env);
	IloIntVarArray boxes(env, num_boxes, 0, num_stacks-1);

	// Constraints: Conflicting boxes are in separate stacks
	for (int i=0; i<num_boxes-1; i++) {
	    for (int j=i+1; j<num_boxes; j++) {
		if (conflicts[i][j] == 1) {
		    cp_model.add(boxes[i] != boxes[j]);
		}
	    }
	}

	// Constraints: Stacks have a maximum height
	IloIntVarArray stack_heights = IloIntVarArray(env,
						      num_stacks,
						      0,
						      max_stack_height);

	// Constraint: Packing
	cp_model.add(IloPack(env,
			     stack_heights,
			     boxes,
			     heights,
			     IloIntExpr(env, num_stacks)));

	IloCP cp_solver(cp_model);
	cp_solver.setOut(env.getNullStream()); // Supress Cplex output

	if (!cp_solver.solve()) {
	    num_stacks++;
	}
	else {
	    break;
	}
    }
    std::cout << "Optimal solution: " << num_stacks << std::endl;

    std::chrono::high_resolution_clock::time_point time_end =
	std::chrono::high_resolution_clock::now();

    std::cout << "Time elapsed: "
	      << std::chrono::duration_cast<std::chrono::milliseconds>(time_end-time_start).count()/1000.0
	      << "s"
	      << std::endl;
    
    env.end();
    return 0;
}


void LoadData(IloEnv env,
	      const char* filename,
	      IloInt& max_stack_height,
	      IloInt& num_boxes,
	      IloIntArray heights,
	      IloArray<IloIntArray> conflicts) {
    ifstream f(filename, ios::in);

    f >> max_stack_height;
    
    int h, l, w;
    IloArray<IloIntArray> boxes = IloArray<IloIntArray>(env);
    while (f >> h >> l >> w) {
	boxes.add(IloIntArray(env, 2, l, w));
	heights.add(h);
    }

    num_boxes = heights.getSize();

    for (int i=0; i<boxes.getSize(); i++) {
	conflicts.add(IloIntArray(env, boxes.getSize()));
	for (int j=0; j<boxes.getSize(); j++) {
	    if (!(((boxes[i][0] < boxes[j][0]) &&
		   (boxes[i][1] < boxes[j][1])) ||
		  ((boxes[j][0] < boxes[i][0]) &&
		   (boxes[j][1] < boxes[i][1])))) {
		conflicts[i][j] = 1;
	    }
	    else {
		conflicts[i][j] = 0;
	    }
	}
    }
}
