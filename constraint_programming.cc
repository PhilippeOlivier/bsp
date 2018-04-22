#include <chrono>
#include <ilcp/cp.h>
#include <iostream>


ILOSTLBEGIN


void LoadData(IloEnv env,
	      const char* filename,
	      IloInt& num_boxes,
	      IloIntArray heights,
	      IloArray<IloIntArray> conflicts);


int main(int argc, char **argv) {
    IloEnv env;

    std::chrono::high_resolution_clock::time_point time_start =
	std::chrono::high_resolution_clock::now();

    // Load data
    IloInt num_boxes;
    IloIntArray heights = IloIntArray(env);
    IloArray<IloIntArray> conflicts = IloArray<IloIntArray>(env);
    LoadData(env, argv[1], num_boxes, heights, conflicts);

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
    
    std::cout << "Greedy solution (first-fit): "
	      << greedy_stacks.getSize()
	      << std::endl;

    // CP model
    IloModel cp_model(env);
    IloIntVarArray boxes(env, num_boxes, 0, greedy_stacks.getSize()-1);

    // Constraints: Conflicting boxes are in separate stacks
    for (int i=0; i<num_boxes-1; i++) {
	for (int j=i+1; j<num_boxes; j++) {
	    if (conflicts[i][j] == 1) {
		cp_model.add(boxes[i] != boxes[j]);
	    }
	}
    }

    // Constraint: Bin packing
    IloIntVarArray loads = IloIntVarArray(env,
    					  greedy_stacks.getSize(),
    					  0,
    					  IloIntMax);
    cp_model.add(IloPack(env,
    			 loads,
    			 boxes,
    			 heights));

    // Symmetry breaking
    for (int i=0; i<greedy_stacks.getSize()-1; i++) {
    	cp_model.add(loads[i] >= loads[i+1]);
    }
    
    // Objective: Minimize the number of stacks
    cp_model.add(IloMinimize(env, IloMax(boxes)));
    
    IloCP cp_solver(cp_model);
    cp_solver.setOut(env.getNullStream()); // Supress Cplex output
    cp_solver.solve();

    std::cout << "Optimal solution: " << cp_solver.getValue(IloMax(boxes)) << std::endl;

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
	      IloInt& num_boxes,
	      IloIntArray heights,
	      IloArray<IloIntArray> conflicts) {
    // Unrotated boxes
    ifstream f(filename, ios::in);
    int l, w, h;
    IloArray<IloIntArray> boxes = IloArray<IloIntArray>(env);
    while (f >> l >> w >> h) {
	boxes.add(IloIntArray(env, 2, l, w));
	heights.add(h);
    }

    // Rotate boxes in every direction such that l <= w
    int num_unrotated_boxes = heights.getSize();
    for (int i=0; i<num_unrotated_boxes; i++) {
	l = boxes[i][0];
	w = boxes[i][1];
	h = heights[i];
	boxes.add(IloIntArray(env, 2, min(l, h), max(l, h)));
	heights.add(w);
	boxes.add(IloIntArray(env, 2, min(w, h), max(w, h)));
	heights.add(l);
    }

    num_boxes = heights.getSize();

    // Compute conflict matrix
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
