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

        Question(): column(0), value(""){}
        Question(int, string);
        bool match (vector<string>);
        void print();
}; // }}}

class Leaf {
// {{{        
    public:
         map<string, int> class_counts; // dictionary of class counts
         string prediction;

        Leaf(){};
        Leaf(vector<vector<string>>);
        void get_class_prediction();
        void print();
}; // }}}

class Decision_Node { 
// {{{        
    /* This class just stores some general information about each node. The 
     * only real important information for the algorithem is the true/false
     * branches.*/
    public:
        float impurity;
        float gain;
        Decision_Node* true_branch;
        Decision_Node* false_branch;
        //Leaf* true_leaf;
        //Leaf* false_leaf;
        Question q;

        //Decision_Node(): impurity(0.0), gain(0.0), true_branch(nullptr), false_branch(nullptr){}
        Decision_Node();
        Decision_Node(Question, float, float, Decision_Node*, Decision_Node*);
        ~Decision_Node();
}; // }}}

void get_class_counts(map<string, int>& class_counts, vector<vector<string>>& rows);

float gini(vector<vector<string>>& rows);

void partition(vector<vector<string>>& rows, Question& q, vector<vector<string>>& true_rows, vector<vector<string>>& false_rows);

float info_gain(vector<vector<string>>& true_rows, vector<vector<string>>& false_rows, float curr_gini);

Question find_best_split(vector<vector<string>> rows, float &i_gain);

#endif
