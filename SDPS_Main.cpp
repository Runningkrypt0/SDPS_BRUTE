//	Andrew Blackledge
//	A brute force solver for some iphone puzzle game, using only one digit and a set of operations, try and reach a goal integer.
//	not complete as some operations can be applied recursively, but enough to solve the first 100 or so problems
//	the complexity for this thing is INSANE by the way, so any more than around 5 occurences is going to take forever.

#include "stdafx.h"
#include <string>
#include <iostream>
#include <cmath>

float Apply_Operation(float a, float b, int state) {
	if (state == 0) { //addition
		return a + b;
	}
	else if (state == 1) { //subtraction
		return a - b;
	}
	else if (state == 2) { //multiplication
		return a * b;
	}
	else if (state == 3) { //division
		return a / b;
	}
	else if (state == 4) { //exponentiation
		return std::powf(a, b);
	}
}

void Display_Constant(int n, int state) {
	if (state == 0) { //normal
		printf("%d", n);
	}
	else if (state == 1) { //sqrt
		printf("sqrt(%d)", n);
	}
	else if (state == 2) { //factorial
		printf("%d!", n);
	}
	else if (state == 3) { //factorial of sqrt
		printf("sqrt(%d)!", n);
	}
	else if (state == 4) { //sqrt of factorial
		printf("sqrt(%d!)", n);
	}
}

void Display_Operation(int state) {
	if (state == 0) { //addition
		printf(" + ");
	}
	else if (state == 1) { //subtraction
		printf(" - ");
	}
	else if (state == 2) { //multiplication
		printf(" * ");
	}
	else if (state == 3) { //division
		printf(" / ");
	}
	else if (state == 4) { //exponentiation
		printf(" ^ ");
	}
}

struct State_Table_Tracker {
	int table_length;
	int state_count;
	int read_head;
	int* table;
	State_Table_Tracker(int t, int s) {
		table_length = t;
		state_count = s;
		table = new int[table_length];
		read_head = 0;
	}
	bool Iterate() {
		int ii = 0;
		while (ii < table_length) {
			table[ii]++;
			if (table[ii] < state_count) {
				break;
			}
			table[ii++] = 0;
		}
		return !(ii == table_length);
	}
	int Read_Next() {
		return table[read_head++];
	}
	void Reset_Table() {
		for (int ii = 0; ii < table_length; ii++) {
			table[ii] = 0;
		}
	}
};

struct Order_Tree_Member {
	//int seed_number;
	Order_Tree_Member* parent;
	Order_Tree_Member* left;
	Order_Tree_Member* right;
	State_Table_Tracker* operations;
	State_Table_Tracker* constants;
	float* constant_table;
	Order_Tree_Member() {
		parent = nullptr;
		left = nullptr;
		right = nullptr;
		constant_table = nullptr;
	}
	float Solve() {
		float left_value = 0;
		if (left != nullptr) {
			left_value = left->Solve();
		}
		else {
			left_value = constant_table[constants->Read_Next()];
		}

		int operation = operations->Read_Next();

		float right_value = 0;
		if (right != nullptr) {
			right_value = right->Solve();
		}
		else {
			right_value = constant_table[constants->Read_Next()];
		}

		return Apply_Operation(left_value, right_value, operation);
	}
	void Display(int seed_number) {
		//print left parenthesis
		printf(" (");

		//print left interior
		if (left != nullptr) {
			left->Display(seed_number);
		}
		else {
			Display_Constant(seed_number, constants->Read_Next());
		}

		//print operation
		Display_Operation(operations->Read_Next());

		//print right interior
		if (right != nullptr) {
			right->Display(seed_number);
		}
		else {
			Display_Constant(seed_number, constants->Read_Next());
		}

		//print right parenthesis
		printf(") ");
	}
	Order_Tree_Member* Traverse_Up_Right() {
		if (parent == nullptr || parent->left == this) {
			return parent;
		}
		return parent->Traverse_Up_Right();
	}
	Order_Tree_Member* Traverse_Down_Left() {
		if (left == nullptr) {
			return this;
		}
		return left->Traverse_Down_Left();
	}
	bool Disconnect_Parent() {
		if (parent != nullptr) {
			if (parent->left == this) {
				parent->left = nullptr;
			}
			if (parent->right == this) {
				parent->right = nullptr;
			}
			return true;
		}
		return false;
	}
	bool Iterate() {
		//find leftmost leaf
		if (left != nullptr) {
			return left->Iterate();
		}
		else if(right != nullptr) {
			return right->Iterate();
		}
		else {
			//this is the leftmost leaf
			//find nearest right ancestor
			Order_Tree_Member* ancestor = Traverse_Up_Right();
			if (ancestor == nullptr) {
				return false;
			}

			if (ancestor->right != nullptr) {
				Order_Tree_Member* next_parent = ancestor->right->Traverse_Down_Left();
				Disconnect_Parent();
				next_parent->left = this;
				if (next_parent == this) {
					__debugbreak();
				}
				parent = next_parent;
			}
			else {
				Disconnect_Parent();
				ancestor->right = this;
				if (ancestor == this) {
					__debugbreak();
				}
				parent = ancestor;
			}

			return true;
		}
	}
};

int factorial(float m)
{
	int n = (int)std::floorf(m);
	return (n == 1 || n == 0) ? 1 : (float)factorial(n - 1) * n;
}

int main()
{
	//inputs?
	int constants_count = 5;
	int operations_count = 5;

	float seed_number = 6;
	printf("\nSeed Number : ");
	std::cin >> seed_number;

	int numbers = 4;
	printf("Number of Occurences : ");
	std::cin >> numbers;

	float target = 21;
	printf("Target : ");
	std::cin >> target;

	printf("\nUsing %d %d's, Reach %d\n\n", numbers, (int)seed_number, (int)target);

	//set up constant table
	float* constant_table = new float[constants_count];
	constant_table[0] = seed_number;
	constant_table[1] = std::sqrtf(seed_number);
	constant_table[2] = (float)factorial(seed_number);
	constant_table[3] = (float)factorial(std::sqrtf(seed_number));
	constant_table[4] = std::sqrtf((float)factorial(seed_number));

	//set up trackers
	State_Table_Tracker* constant_tracker = new State_Table_Tracker(numbers,constants_count);
	State_Table_Tracker* operation_tracker = new State_Table_Tracker(numbers-1,operations_count);

	//set up tree
	Order_Tree_Member* root = new Order_Tree_Member();
	//root.seed_number = seed_number;
	root->operations = operation_tracker;
	root->constants = constant_tracker;
	root->constant_table = constant_table;

	Order_Tree_Member* last_member = root;

	for (int ii = 0; ii < numbers - 2; ii++) {

		Order_Tree_Member* next_member = new Order_Tree_Member();
		//next_member.seed_number = seed_number;
		next_member->operations = operation_tracker;
		next_member->constants = constant_tracker;
		next_member->constant_table = constant_table;

		next_member->parent = last_member;
		last_member->left = next_member;

		last_member = next_member;
	}

	float result = target - 1;

	//iterate through parenthesis combinations
	do {
		
		//iterate through constant combinations
		constant_tracker->Reset_Table();
		do {

			//iterate through operation combinations
			operation_tracker->Reset_Table();
			do {

				//Solve
				constant_tracker->read_head = 0;
				operation_tracker->read_head = 0;

				result = root->Solve();

				if (result == target) { break; }
			} while (operation_tracker->Iterate());

			if (result == target) { break; }
		} while (constant_tracker->Iterate());

		if (result == target) { break; }
	} while (root->Iterate());

	//now to display the solution...
	if (result == target) {
		constant_tracker->read_head = 0;
		operation_tracker->read_head = 0;

		printf("Solution:\n\n", (int)seed_number, (int)target);
		root->Display((int)seed_number);
		printf("\n\n");
	}
	else {
		printf("No Solution Found...\n\n", (int)seed_number, (int)target);
	}

    return 0;
}

