#include <ilcp/cp.h>
#include <iostream>


#define RC_EPS 1.0e-6


ILOSTLBEGIN


void LoadData(IloEnv env,
	      const char* filename,
	      IloInt& num_boxes,
	      IloIntArray heights,
	      IloArray<IloIntArray> conflicts);


int main(int argc, char **argv) {
    IloEnv env;

    // Data
    IloInt max_height = 100;
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
	    if (height+heights[i] > max_height) {
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
    std::cout << "Greedy solution (first-fit): " << greedy_stacks.getSize() << std::endl;

    // CP model: try an increasing number of stacks
    std::cout << "Lower bound: " << ceil((float)IloSum(heights)/(float)max_height) << std::endl;
    IloInt num_stacks = ceil((float)IloSum(heights)/(float)max_height);
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
						      max_height);

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
    
    env.end();
    return 0;
}


void LoadData(IloEnv env,
	      const char* filename,
	      IloInt& num_boxes,
	      IloIntArray heights,
	      IloArray<IloIntArray> conflicts) {
    ifstream f(filename, ios::in);
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
	    if (!(((boxes[i][0] < boxes[j][0]) && (boxes[i][1] < boxes[j][1])) ||
		  ((boxes[j][0] < boxes[i][0]) && (boxes[j][1] < boxes[i][1])))) {
		conflicts[i][j] = 1;
	    }
	    else {
		conflicts[i][j] = 0;
	    }
	}
    }
}
