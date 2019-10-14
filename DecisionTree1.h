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
        string value;

        Question(): column(0), value("NO-QUESTION-FOUND"){}
        Question(int, string);
        void print();
}; // }}}

class Leaf {
// {{{        
    public:
         vector<vector<string>> rows;   // holds all of the rows, until 
                                        // class_counts is created
                                        
         map<string, int> class_counts; // dictionary of class counts
         string prediction;

        Leaf(){};
        Leaf(vector<vector<string>>&);
        void store_prediction();
        void get_class_prediction();
        void print();
}; // }}}


class Decision_Node { 
// {{{        
    /* This class just stores some general information about each node.
     * - if the node has a split, all of the Decision_Node fields will be
     *   instantiated, except for the leaf node.
     * - If the node is a terminating node (without any children), than it 
     *   will only contain a reffernce to a leaf node.
     * */
    public:
        float impurity;
        float gain;
        Decision_Node* true_branch;
        Decision_Node* false_branch;

        Question* q;

        Leaf* leaf_node;
        

        Decision_Node();
        Decision_Node(Question*, float, float, Decision_Node*, Decision_Node*);
        ~Decision_Node();
}; // }}}

void get_class_counts(map<string, int>& class_counts, vector<vector<string>>& rows);

float gini(vector<vector<string>>& rows);

void partition(vector<vector<string>>& rows, Question& q, vector<vector<string>>& true_rows, vector<vector<string>>& false_rows);

float info_gain(vector<vector<string>>& true_rows, vector<vector<string>>& false_rows, float curr_gini);

bool match(Question* q, vector<string>& row);

Question find_best_split(vector<vector<string>> &rows, float &i_gain);

Leaf* get_leaf(vector<string> &row, Decision_Node& tree);

#endif
