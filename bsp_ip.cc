/*
 * Argument 1 [string]: Path of instance
 *
 * Example
 * ./bsp_ip 50_100
 *
 */


#include <chrono>
#include <ilcplex/ilocplex.h>
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

    //// IP model
    IloModel model(env);
    IloNumVarArray x = IloNumVarArray(env, num_boxes, 0, 1, ILOINT);

    // Constraints: Box stacking rules
    for (int i=0; i<num_boxes-1; i++) {
    	for (int j=i+1; j<num_boxes; j++) {
    	    if (conflicts[i][j] == 1) {
    		model.add(x[i]+x[j] <= 1);
    	    }
    	}
    }
    
    // Constraints: Each box cannot be used more than once
    for (int i=0; i<num_boxes; i+=3) {
	model.add(x[i]+x[i+1]+x[i+2] <= 1);
    }    

    // Objective: Maximize stack height
    IloObjective objective = IloAdd(model,
				    IloMaximize(env, IloScalProd(x, heights)));

    /// Solver
    IloCplex solver(model);
    solver.setOut(env.getNullStream()); // Supress Cplex output
    solver.solve();
    std::cout << "Optimal solution: "
	      << solver.getObjValue()
	      << std::endl;

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
    // Rotate boxes in every direction such that w <= l
    ifstream f(filename, ios::in);
    int i, j, k;
    IloArray<IloIntArray> boxes = IloArray<IloIntArray>(env);
    while (f >> i >> j >> k) {
	boxes.add(IloIntArray(env, 2, min(i, j), max(i, j)));
	heights.add(k);
	boxes.add(IloIntArray(env, 2, min(i, k), max(i, k)));
	heights.add(j);
	boxes.add(IloIntArray(env, 2, min(j, k), max(j, k)));
	heights.add(i);
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
