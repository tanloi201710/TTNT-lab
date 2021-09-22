// Ho Tan Loi - B1809148 - STT 18

// Khai bao, khoi tao

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <set>
#include <stack>
#include <queue>
#include <fstream>


using namespace std;

const int BOARD_ROWS = 3;
const int BOARD_COLS = 3;
const int EMPTY_TILE = 0;
const int MAX_OP = 4;			// so luong cac hanh dong trong bai toan nay

// Hang chuoi de in ra ten cac hanh dong
const char* action[] = {"Trang thai bat dau", 
						"TRAI",
						"PHAI",
						"LEN",
						"XUONG"};

//1. Dinh nghia trang thai cua bai toan
struct State {
	int num[BOARD_ROWS][BOARD_COLS];
	int empty_row;		// Luu chi muc dong cua o trong
	int empty_col;		// Luu chi muc cot cua o trong
};

void print_state(State s) {
	for(int row = 0; row < BOARD_ROWS; row++) {
		for(int col = 0; col < BOARD_COLS; col++) {
			if(s.num[row][col] == 0)
				printf("_   ");
			else
				printf("%d   ", s.num[row][col]);	
		}
		printf("\n");
	}	
}

/**
 * Doc du lieu tu file
 */
State* read_file(string file) {
	//ifstream fin(file);
	ifstream fin(file.c_str());
	
	State *s = new State();
	for(int row = 0; row < BOARD_ROWS; row++) {
		for(int col = 0; col < BOARD_COLS; col++) {
			fin >> s->num[row][col];	
		}
	}
	fin >> s->empty_row >> s->empty_col;
	return s;
}


//So sanh hai trang thai co bang nhau khong
bool compareStates(State s1, State s2){
	// Kiem tra o trong cua 2 trang thai
	if(s1.empty_row != s2.empty_row || s1.empty_col != s2.empty_col)
		return false;
	
	// Kiem tra tung vi tri cua 2 trang thai s1 va s2
	for(int row = 0; row < BOARD_ROWS; row++) {
		for(int col = 0; col < BOARD_COLS; col++) {
			if(s1.num[row][col] != s2.num[row][col]) {
				return false;	
			}	
		}
	}
	return true;
}

//2. Dinh nghia cac hanh dong

/**
 * Dinh nghia hanh dong UP: di chuyen o trong len tren
 */

bool up(State current_state, State &new_state) {
	
	int empty_row = current_state.empty_row;
	int empty_col = current_state.empty_col;
	if(empty_row > 0) {
		new_state = current_state;
		
		new_state.empty_col = empty_col;
		new_state.empty_row = empty_row - 1;
		
		new_state.num[empty_row][empty_col] = current_state.num[empty_row - 1][empty_col];
		new_state.num[empty_row - 1][empty_col] = EMPTY_TILE;
		
		return true;
	}
	
	return false;
}

/**
 * Dinh nghia hanh dong DOWN: di chuyen o trong di xuong
 */
bool down(State current_state, State &new_state) {
    int empty_row = current_state.empty_row;
    int empty_col = current_state.empty_col;
    if(empty_row < BOARD_ROWS - 1) {
        new_state = current_state;
        new_state.empty_col = empty_col;
        new_state.empty_row = empty_row + 1;

        new_state.num[empty_row][empty_col] = current_state.num[empty_row + 1][empty_col];
        new_state.num[empty_row + 1][empty_col] = EMPTY_TILE;

        return true;
    }
    return false;
}


/**
 * Dinh nghia hanh dong LEFT: di chuyen o trong sang trai
 */
bool left(State current_state, State &new_state) {
    int empty_row = current_state.empty_row;
    int empty_col = current_state.empty_col;
    if(empty_col > 0) {
        new_state = current_state;
        new_state.empty_col = empty_col - 1;
        new_state.empty_row = empty_row;

        new_state.num[empty_row][empty_col] = current_state.num[empty_row][empty_col - 1];
        new_state.num[empty_row][empty_col - 1] = EMPTY_TILE;

        return true;
    }
    return false;
}


/**
 * Dinh nghia hanh dong UP: di chuyen o trong sang phai
 */
bool right(State current_state, State &new_state) {
    int empty_row = current_state.empty_row;
    int empty_col = current_state.empty_col;
    if(empty_col < BOARD_COLS - 1) {
        new_state = current_state;
        new_state.empty_col = empty_col + 1;
        new_state.empty_row = empty_row;

        new_state.num[empty_row][empty_col] = current_state.num[empty_row][empty_col + 1];
        new_state.num[empty_row][empty_col + 1] = EMPTY_TILE;

        return true;
    }
    return false;
    
}

bool call_operator(State s, State& out, int option) {
    switch (option) {
        case 1:
            return left(s, out);
        case 2:
            return right(s, out);
        case 3:
            return up(s, out);
        case 4:
            return down(s, out);
        default:
            return false;
    }
}

//3. Cai dat giai thuat tim kiem

// 3.1 Khai bao cau truc nut (dinh) de xay dung cay tim kiem
typedef struct Node{
	State state;
	struct Node* parent;
	int no_function;
	int g;		// Luu gia tri duong di cua nut goc den nut hien tai
	int h;		// Luu ket qua uoc luong cua ham heuristic cua nut hien tai den trang thai dich
	int f;		// f = g + h
} Node;

struct node_cmp {
    bool operator() (Node* n1, Node* n2) {
        return n1->f > n2->f;
    }
};

// 3.2. Dinh nghia cac ham heuristic

int h1(State s1, State s2) {
	int count = 0;
	for(int row = 0; row < BOARD_ROWS; row++) {
		for(int col = 0; col < BOARD_COLS; col++) {
			if(s1.num[row][col] != 0) {
				if(s1.num[row][col] != s2.num[row][col]) {
					count++;
				}
			}
		}
	}
	return (count);
};

int h2(State s, State s2) {
    int count = 0;
    for (int row = 0; row < BOARD_ROWS; row++) {
        for (int col = 0; col < BOARD_COLS; col++) {
            if (s.num[row][col] != EMPTY_TILE) {
                for (int r = 0; r < BOARD_ROWS; r++) {
                    for (int c = 0; c < BOARD_COLS; c++) {
                        if (s.num[row][col] == s2.num[r][c]) {
                            count += abs(r - row) + abs(c - col);
                            r = BOARD_ROWS; //hack to break 2 loops
                            break;
                        }
                    }
                }
            }
        }
    }
    
    return count;
};

// Tham so func = 1 neu chung ta su dung ham heuristic h1,
//           va = 2 neu chung ta su dung ham heuristic h2
int heuristic(State s1, State s2, int func) {
	if(func == 1)
		return h1(s1, s2);
	else
		return h2(s1, s2);
}

//3.3 Khai bao cac ham can thiet phuc vu cho ham chinh

int find_state_in_queue(State state, queue<Node*> q) {
	while(!q.empty()) {
		if(compareStates(q.front()->state, state))
			return 1;
		q.pop();
	}
	return 0;
}


int find_state_in_priority_queue(State state, priority_queue<Node*, vector<Node*>, node_cmp> q) {
	while(!q.empty()) {
		if(compareStates(q.top()->state, state))
			return 1;
		q.pop();
	}
	return 0;
}

int find_state_in_stack(State state, stack<Node*> s) {
	while(!s.empty()) {
		if(compareStates(s.top()->state, state))
			return 1;
		s.pop();
	}
	return 0;
}

Node* find_node(State state, priority_queue<Node*, vector<Node*>, node_cmp> q) {
	while(!q.empty()) {
		if(compareStates(q.top()->state, state))
			return q.top();
		q.pop();
	}  
    return NULL;
}

//3.4. Cai dat cac giai thuat. 

/*
***********************
    A. TÌM KIẾM MÙ
***********************
*/

//3.4.1. Thuat toan duyet theo chieu rong
Node* BFS(State init_state, State goal_state){
		
	// Khai bao hai hang doi Open va Close
	queue<Node*> Open_BFS;
	queue<Node*> Close_BFS;
	int step = 0;
	
	// Tao nut trang thai cha
	Node* root = (Node*)malloc(sizeof(Node));
	root->state = init_state;
	root->parent = NULL;
	root->no_function = 0;
	Open_BFS.push(root);
	while(!Open_BFS.empty()){
		
		printf("\n");
		printf("\nOpen BFS: %d", Open_BFS.size());
		printf("\nClose BFS: %d", Close_BFS.size());
		
		Node* node = Open_BFS.front();	// Lay ra gia tri BEN TRAI cua Open de kiem tra
		Open_BFS.pop();
		Close_BFS.push(node);
		
		// Kiem  tra xem dinh lay ra co phai la trang thai muc tieu
		if(compareStates(node->state, goal_state))
		{
			printf("\nTong so nut trong Open la: %d", Open_BFS.size());
			printf("\nTong so nut trong Close la: %d", Close_BFS.size());
			return node;
		}
		
		
		// Goi cac phep toan tren trang thai
		int option;
		for(option=1; option<=MAX_OP; option++) {
			State newstate;
			
			if(call_operator(node->state, newstate, option)) {
				
				// Neu trang thai moi sinh ra da ton tai thi bo qua
				if(find_state_in_queue(newstate, Open_BFS) || find_state_in_queue(newstate, Close_BFS))
					continue;
					
				// Neu trang thai moi chua ton tai thi them vao hang doi
				//printf("\nTrang thai moi duoc dua vao hang doi OPEN\n");
				//print_state(newstate);
				Node *newNode = (Node*)malloc(sizeof(Node));
				newNode->state = newstate;
				newNode->parent = node;
				newNode->no_function = option;
				Open_BFS.push(newNode);
			}
		}
	}
	return NULL;
}

//3.4.2. Giai thuat duyet theo chieu sau
Node* DFS(State init_state, State goal_state){
	
	// Khai bao hai ngan xep Open va Close
	stack<Node*> Open_DFS;
	stack<Node*> Close_DFS;
	
	// Tao nut trang thai cha
	Node* root = (Node*)malloc(sizeof(Node));
	root->state = init_state;
	root->parent = NULL;
	root->no_function = 0;
	Open_DFS.push(root);
	while(!Open_DFS.empty()){
		
		printf("\n");
		printf("\nOpen BFS: %d", Open_DFS.size());
		printf("\nClose BFS: %d", Close_DFS.size());
		
		// Lay mot dinh trong ngan xep
		Node* node = Open_DFS.top();
		Open_DFS.pop();
		Close_DFS.push(node);
		
		// Kiem  tra xem dinh lay ra co phai la trang thai muc tieu
		if(compareStates(node->state, goal_state))
		{
			printf("\nTong so nut trong Open la: %d", Open_DFS.size());
			printf("\nTong so nut trong Close la: %d", Close_DFS.size());
			return node;
		}
			
		int option;
		// Goi cac phep toan tren trang thai
		for(option=1; option<=MAX_OP; option++) {
			State newstate;
			if(call_operator(node->state, newstate, option)) {
				
				// Neu trang thai moi sinh ra da ton tai thi bo qua
				if(find_state_in_stack(newstate, Open_DFS) || find_state_in_stack(newstate, Close_DFS))
					continue;
					
				// Neu trang thai moi chua ton tai thi them vao ngan xep
				Node *newNode = (Node*)malloc(sizeof(Node));
				newNode->state = newstate;
				newNode->parent = node;
				newNode->no_function = option;
				Open_DFS.push(newNode);
			}
		}
	}
	return NULL;
}

/*
*******************************
    A. TÌM KIẾM HEURISTIC
*******************************
*/

//3.4.4. Giai thuat A_sao

Node* Astart(State init_state, State goal_state, int func){
		
	// Khai bao hai hang doi Open va Close
	// Khac voi BFS, trong giai thuat Greedy cung nhu Best First Search,
	// chung ta su dung Hang Doi Uu Tien (xem giao trinh trang 59) 
	// (sap xep thu tu dua tren gia tri f cua cac node)
	priority_queue<Node*, vector<Node*>, node_cmp> priority_queue_Open; 
	priority_queue<Node*, vector<Node*>, node_cmp> priority_queue_Close;
	int step = 0;
	
	// Tao nut trang thai cha
	Node* root = (Node*)malloc(sizeof(Node));
	root->state = init_state;
	root->parent = NULL;
	root->no_function = 0;
	root->h = heuristic(root->state, goal_state, func);
	root->g = 0;
	root->f = root->h + root->g;
	priority_queue_Open.push(root);
	while(!priority_queue_Open.empty()){

        printf("\n");
		printf("\nOpen BFS: %d", priority_queue_Open.size());
		printf("\nClose BFS: %d", priority_queue_Close.size());
		
		Node* node = priority_queue_Open.top();	// Lay ra gia tri BEN TRAI cua Open de kiem tra
		priority_queue_Open.pop();
		priority_queue_Close.push(node);
		
		// Kiem  tra xem dinh lay ra co phai la trang thai muc tieu
		if(compareStates(node->state, goal_state))
		{
			printf("\nTong so nut trong Open la: %d", priority_queue_Open.size());
			printf("\nTong so nut trong Close la: %d", priority_queue_Close.size());
			return node;
		}
		
		
		
		// Goi cac phep toan tren trang thai
		//printf("\n");
		int option;
		for(option=1; option<=MAX_OP; option++) {
			State newstate;
			
			if(call_operator(node->state, newstate, option)) {
				
				// Neu trang thai moi sinh ra da ton tai thi bo qua
				
				if(find_state_in_priority_queue(newstate, priority_queue_Close))
					continue;
					
				Node* n= find_node(newstate, priority_queue_Open);
				
				if(n==NULL)	{
					// Neu trang thai moi chua ton tai thi them vao hang doi
					Node *newNode = (Node*)malloc(sizeof(Node));
					newNode->state = newstate;
					newNode->parent = node;
					newNode->no_function = option;
					newNode->h = heuristic(newstate, goal_state, func);
					newNode->g = node->g+1;
					newNode->f = newNode->h + newNode->g;
					priority_queue_Open.push(newNode);
					
				} else {
					n->g = node->g+1;
                    n->f = n->g+n->h;
				}
			}
		}
	}
	return NULL;
}

//3.5. Xay dung ham de in cac hanh dong de dat den trang thai muc tieu

void print_WaysToGetGoal(Node* node) {
	stack<Node*> stackPrint;
	
	// Duyet nguoc ve nut parent
	while(node->parent != NULL) {
		stackPrint.push(node);
		node = node->parent;
	}
	stackPrint.push(node);
	
	// In ra thu tu hanh dong
	int no_action = 0;
	while(!stackPrint.empty()){
		printf("\nAction %d: %s\n", no_action, action[stackPrint.top()->no_function]);
		print_state(stackPrint.top()->state);
		stackPrint.pop();
		no_action++;
	}
}

void print_WaysToGetGoal_with_Heuristic(Node* node) {
	int no_action = 0;
	stack<Node*> stackPrint;
	
	// Duyet nguoc ve nut parent
	while(node->parent != NULL) {
		stackPrint.push(node);
		node = node->parent;
	}
	stackPrint.push(node);
	
	// In ra thu tu hanh dong
	
	while(!stackPrint.empty()){
		printf("\nAction %d: %s\n", no_action, action[stackPrint.top()->no_function]);
		print_state(stackPrint.top()->state);
		printf("g = %d", stackPrint.top()->g);
		printf("\nh = %d", stackPrint.top()->h);
		printf("\nf = %d", stackPrint.top()->f);
		stackPrint.pop();
		no_action++;
	}
}

int main() {
	State *start_state;
	
	// Khai bao trang thai bat dau.
	// Trang thai bat dau duoc cau hinh trong file test.txt
	start_state = read_file("start_easy.txt");
	printf("Trang thai bat dau: \n");
	print_state(*start_state);
	
	
	// Khai bao trang thai muc tieu.
	// Trang thai muc tieu duoc cau hinh trong file goal.txt
	State *goal_state;
	goal_state = read_file("goal_easy.txt");
	printf("\n");
	printf("Trang thai muc tieu: \n");
	print_state(*goal_state);

    // // Tìm kiếm mù
    // Tìm kiếm theo chiều rộng
    printf("\n");
	printf("*** Su dung giai thuat BFS ***");
	Node* bfs_result = BFS(*start_state, *goal_state);
	printf("\n");
	print_WaysToGetGoal(bfs_result);
	printf("\n*** Ket thuc giai thuat BFS ***\n");
	
	
	// Tìm kiếm theo chiều sâu
	printf("\n");
	printf("*** Su dung giai thuat DFS ***");
	Node* dfs_result = DFS(*start_state, *goal_state);
	printf("\n");
	print_WaysToGetGoal(dfs_result);
	printf("\n*** Ket thuc giai thuat DFS ***\n");

	
	// // Tim kiem Heuristic - Giai thuat Asao
	

	printf("\n");
	printf("*** Giai bai toan su dung ham heuristic h1 voi tim kiem Asao ***");
	Node* astart_h1_result = Astart(*start_state, *goal_state, 1);
	printf("\n");
	print_WaysToGetGoal_with_Heuristic(astart_h1_result);

    printf("\n");
	printf("*** Giai bai toan su dung ham heuristic h2 voi tim kiem Asao ***");
	Node* astart_h2_result = Astart(*start_state, *goal_state, 2);
	printf("\n");
	print_WaysToGetGoal_with_Heuristic(astart_h2_result);
	
	
	
	// Giai phong bo nho
    delete bfs_result;
    delete dfs_result;
	delete astart_h1_result;
    delete astart_h2_result;
	delete start_state;
	delete goal_state;
	
	return 1;
}
