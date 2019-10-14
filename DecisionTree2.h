#ifndef D_TREE
#define D_TREE

#include<vector>
#include<map>

using namespace std;

class Question {
// {{{        
/* This class creates Questoins. It is not totally necessary to have this
 * class. Its only real functionality is a call to match, which determines
 * whether or not a row evaluates to true or false for the Question.
 * */
    public:
        int column;
        int value;

        Question(): column(0), value(0){}
        Question(int, int);
        void print();
}; // }}}

class Leaf {
// {{{
    public:
        vector<vector<int>> rows;
        vector<vector<vector<int>>> count_matrix;
        int prediction;

        Leaf(){};
        Leaf(vector<vector<int>>&, vector<vector<vector<int>>>&);
        void store_prediction();
        void get_class_prediction();
        void print();
}; // }}}

class Decision_Node { 
// {{{        
    // This class just stores some general information about each node. The 
    // only real important information for the algorithem is the true/false
    // branches.
    public:

        //float impurity;
        //float gain;

        Decision_Node* true_branch;
        Decision_Node* false_branch;
        Leaf* leaf_node;
        
        Question* q;

        Decision_Node();
        Decision_Node(Question*, Decision_Node*, Decision_Node*);
        ~Decision_Node();
}; // }}}



Leaf* get_leaf(vector<int> &row, Decision_Node* tree);

int classify(vector<int> &row, Decision_Node* tree);

vector<bool> classify_all(vector<vector<int>> &rows, Decision_Node* tree);

void print_classification_results(vector<vector<int>> &rows, Decision_Node* tree);


// // <-- ALGORITHM -->
// ----------------------------------------------------------------------------
/*
Decision_Node* build_tree(vector<vector<int>> &rows){
*/

// <-- COUNT MATRICES -->
// ----------------------------------------------------------------------------
// used by convert rows
vector<string> get_unique(vector<string> &myvector);

vector<int> get_unique(vector<int> &myvector);

void convert_rows(vector<vector<string>> &all_rows, 
                  vector<vector<int>>    &all_rows_converted);

vector<vector<int>> get_all_unique_elements(vector<vector<int>>  &all_rows_converted);

vector<vector<vector<int>>> set_count_matrices(vector<vector<int>>  &all_rows_converted);

void print_CM(vector<vector<vector<int>>> count_matrices);

void print_CM_sizes(vector<vector<vector<int>>> count_matrices);

void store_blanck_matrices(vector<vector<int>> all_rows_converted);
// ----------------------------------------------------------------------------

// <-- ALGORITHM -->
// ----------------------------------------------------------------------------
Question find_best_question(vector<vector<vector<int>>> &count_matrices, int row_count, float &best_gain);

bool match(Question* q, vector<int>& row);

void partition(vector<vector<int>>& rows, Question &q, 
        vector<vector<int>> &true_rows, 
        vector<vector<int>> &false_rows,
        vector<vector<vector<int>>> &true_count_matrix,
        vector<vector<vector<int>>> &false_count_matrix);


#endif
