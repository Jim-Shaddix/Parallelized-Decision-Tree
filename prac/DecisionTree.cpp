#include <iostream>  // C++ output 
#include <fstream>   // reading files
#include <sstream>   // reading strings [as streams]

#include <cstdlib>   // used for random number generator
#include <numeric>   // accumulate (concatenate vector of strings) 
#include <iomanip>   // setw (set width on cout)

#include "global_files/timer.h"  
#include "global_files/useful-func.h"
#include "global_files/read_csv.h"
#include "DecisionTree.h"

#define PRINT_CLASS_COUNTS 0
#define PRINT_GINI 0
#define PRINT_MATCH 0
#define PRINT_GAIN 0
#define PRINT_BEST_Q 1
#define PRINT_PARTITION 1

using namespace std;

// GLOBAL VARIABLES
vector<string> headers;
int T_index = 0;

// <-- Question Methods -->
// {{{
Question::Question(int col, string v){
    column = col;
    value = v;
}

void Question::print(){
    string condition = " == ";
    //if (is_float(value)) {condition = " >= ";}
    cout << "[QUESTION] Is " << headers[column] << condition << value << "?";
}

bool Question::match(vector<string> row){

    string compare = row[column];

    if (PRINT_MATCH){

        cout << "\n[MATCH] [" << headers[column] << "] ";

        // assume string type
        bool ans = (compare == value);
        string condition = " == ";

        // check if float
        //if (is_float(compare)){
        //    condition = " >= ";
        //    ans = (stof(compare) >= stof(value));
        //}

        // print results
        cout << "(" << compare << condition << value << ") = " << ans << endl;
    }

    //if (is_float(compare))
    //    return (stof(compare) >= stof(value));
    
    return compare == value;
} // }}}

// <-- Decision Node Methods -->
// {{{
Decision_Node::Decision_Node(Question q, float gain, float impurity, 
        Decision_Node* t_branch, Decision_Node* f_branch){

    this->q            = q;
    this->gain         = gain;
    this->impurity     = impurity;

    cout << "MADE IT TO HERE! 1" << endl;

    // allocate on heap
    true_branch  = new Decision_Node();
    false_branch = new Decision_Node();

    cout << "MADE IT TO HERE! 2" << endl;

    // perform shallow copy
    *true_branch  = *t_branch;
    *false_branch = *f_branch;

    cout << "MADE IT TO HERE! 3" << endl;

}
Decision_Node::Decision_Node(){
    impurity = 0.0;
    gain     = 0.0;
    true_branch  = nullptr;//new Decision_Node();
    false_branch = nullptr;//new Decision_Node();
}

Decision_Node::~Decision_Node(){
    delete true_branch;
    delete false_branch;
}

// }}}

// <-- Leaf Methods -->
// {{{
Leaf::Leaf(vector<vector<string>> rows){
    get_class_counts(class_counts, rows);
    get_class_prediction(); // store the prediction
}

void Leaf::get_class_prediction(){
    
    map<string,int>::iterator iter;
    prediction = "";
    int max_count = 0;
    for( iter = class_counts.begin(); iter != class_counts.end(); iter++ ) {
        if (iter->second > max_count){
            max_count  = iter->second;
            prediction = iter->first;
        }
    }
}

void Leaf::print(){
    cout << "\n[PREDICTION] " << prediction << endl;
}
// }}}

void get_class_counts(map<string, int>& class_counts, vector<vector<string>>& rows){
    // {{{
    
    /* This function is used to help calculate gini impurity.
     *  - this function stores values in the map that was passed in. Each
     *    class is than mapped to the number of instances it had in a particular
     *    data set.
     * */

    // count the number of occurences of each class
    for (int i = 0; i < rows.size(); ++i)
        class_counts[rows[i][T_index]]++;
    
    // print
    if (PRINT_CLASS_COUNTS){
        map<string,int>::iterator iter;
        cout << "\nCLASS COUNTS: [TOTAL = "<< rows.size()<< "]" <<endl;
        for( iter = class_counts.begin(); iter != class_counts.end(); iter++ ) {
            cout << "Class: " << iter->first << 
                    ", Count: " << iter->second << endl;
        }
    }
} // }}}

float gini(vector<vector<string>>& rows){
    // {{{
    /* 
     * Calculates the gini impurity of a dataset
     *
     * DEPENDENT FUNCTIONS:
     *      - class counts
     */
    map <string, int> class_counts;
    get_class_counts(class_counts, rows);
    float impurity  = 1;
    float prob_class;
    int i;

    // Accessing the elements using iterators
    map<string,int>::iterator iter;
    for (iter = class_counts.begin(); iter != class_counts.end(); ++iter){
        prob_class = float(iter->second) / float(rows.size());
        impurity -= prob_class * prob_class;
    }

    if (PRINT_GINI)
        cout << "\nGINI IMPURITY = "<<impurity<<endl;

    return impurity;
}   // }}}

void partition(vector<vector<string>>& rows, Question& q, vector<vector<string>>& true_rows, vector<vector<string>>& false_rows){
    // {{{
    /*
     * Partitions a dataset based on a question, and stores the results into
     * true_rows and false_rows
     */
    int i;
    for (i = 0; i < rows.size(); ++i) {
        if (q.match(rows[i]))
            true_rows.push_back(rows[i]);
        else
            false_rows.push_back(rows[i]);
    }
}   // }}}

float info_gain(vector<vector<string>>& true_rows, vector<vector<string>>& false_rows, float curr_gini){
    // {{{
    /*
     * Calculates the inforamtion gain of a particular question.
     *
     */
    float g1 = gini(true_rows);
    float g2 = gini(false_rows);
    float frac_true = float(true_rows.size()) / float((true_rows.size()) + float(false_rows.size()));

    if (PRINT_GAIN){
        cout << "[Information Gain]" << endl;
        cout << "[True-Gini = " << g1 << "] [False Gini = " << g2 << "] [Curr Gini = "<<curr_gini<<"]"<<endl;
        cout << "Information Gain: "<< curr_gini - (frac_true*g1 + (1-frac_true)*g2) 
             << endl;
    }

    return curr_gini - (frac_true*g1 + (1-frac_true)*g2);
}   //}}}

Question find_best_split(vector<vector<string>> rows, float &i_gain){
    /* {{{
     * [RETURNS]
     *      - the best question to split the data, or null, if the data could
     *        not be split, indicating that we are in a leaf node.
     */

    // Init variables
    float gain;
    float best_gain = 0;
    Question best_question;
    vector<vector<string>> true_rows;
    vector<vector<string>> false_rows;
    float impurity = gini(rows);

    // Loop over all elements, and test all possible questions, store the best
    int i,j;
    for (i = 0; i < rows.size(); ++i) {
        for (j = 0; j < rows[i].size(); ++j) {

            if (j == T_index){continue;}

            // generate question
            Question q(j,rows[i][j]);

            // store true & false rows (based on question)
            partition(rows, q, true_rows, false_rows); 

            // eliminates useless questions & questions that have already been
            // asked
            if (true_rows.size() == 0 || false_rows.size() == 0){continue;}

            // information gain of question
            gain = info_gain(true_rows, false_rows, impurity);

            // remove elements
            true_rows.clear();
            false_rows.clear();

            // store best results
            if (gain >= best_gain){
                best_gain     = gain; 
                best_question = q;
            }

        } // inner for
    } // outer for

    if (PRINT_BEST_Q){
        cout <<"\n[BEST_QUESTION] "; 
        best_question.print();
        cout << "[GAIN] " << best_gain << endl;
        cout << "[PARTITION] ";
        partition(rows, best_question, true_rows, false_rows); 
        cout << "(true-size, false-size) =\t\t     (" 
            << true_rows.size()  << "," << false_rows.size() << ")\n";
        //cout << "[false rows size = " << false_rows.size() << "]" << endl;
    }
    // assign the best gain
    i_gain = best_gain;

    return best_question;
    
} // }}}

// attempt 2
Decision_Node* build_tree(vector<vector<string>> &rows){
    // {{{

    float gain = 0;
    Question q = find_best_split(rows, gain);

    // partition data based on question
    vector<vector<string>> true_rows, false_rows;
    partition(rows,q,true_rows,false_rows);

    // used for base case.
    float curr_gini  = gini(rows);
    float true_gini  = gini(true_rows);
    float false_gini = gini(false_rows);

    // create left and right branches & leaves
    Decision_Node* true_b  = nullptr;
    Decision_Node* false_b = nullptr;

    cout << "true-gini = "<<true_gini << " | false-gini = "<< false_gini << endl;
    
    // BASE CASE CONDITIONS:
    if (true_gini  >= 0.2){ 
        cout << "\n[TRUE NODE] -------------------------------------------------"<<endl;
        true_b = build_tree(true_rows);  
    }

    if (false_gini >= 0.2){ 
        cout << "\n[FALSE NODE] ------------------------------------------------"<<endl;
        false_b = build_tree(false_rows); 
    }
    cout << "\n[FINAL]" << endl;

    // assign the final decision node
    Decision_Node* return_node = new Decision_Node(q, gain, curr_gini, true_b, false_b);
    cout << "\n[ABOUT TO RETURN]" << endl;
    //*return_node_p = return_node;
    return return_node;

} // }}}


int main(int argc, char *argv[])
{
    // DEFINING VARIABLES
    const char sep   = ',';
    string delim     = "?";
    //string Target    = "irradiat";
    //string file_name = "data/breast-cancer.data";
    //float percent_training = 0.7;

    string Target    = "Fruit";
    string file_name = "data/test.csv";
    float percent_training = 1;

    double time;
    
    // start timer
    initialize_timer();
    start_timer();

    // variables to initialize
    vector<vector<string>> all_rows; 
    vector<vector<string>> training_data;
    vector<vector<string>> testing_data;

    // initialize all variables
    get_file_data(file_name, sep, Target, delim, percent_training, T_index,
                  headers, all_rows, training_data, testing_data);

    //build-tree
    cout << "got here ... " << endl;
    //Decision_Node* ddd = new Decision_Node;
    cout << "\ngini initial = " << gini(all_rows) << endl;
    Decision_Node* ddd = build_tree(all_rows);

    delete(ddd);

    /*
    //print_vec_matrix(training_data);
    
    Question q1(1,"50-59");
    q1.print();
    bool ans1 = q1.match(all_rows[0]);

    // class counts
    map <string, int> class_counts;
    get_class_counts(class_counts, all_rows);


    // gini impurity
    float g = gini(all_rows);

    // question 1
    Question q1(0,"Green");
    q1.print();
    cout << "MADE IT HERE" << endl;
    bool ans1 = q1.match(all_rows[1]);

    // question 2
    Question q2(1,"3");
    q2.print();
    bool ans2 = q2.match(all_rows[1]);

    // partition
    vector<vector<string>> true_rows;
    vector<vector<string>> false_rows;
    partition(all_rows,q2,true_rows,false_rows);
    
    // Print True Partitions
    int i;
    cout << "\nTrue Rows"<<endl;
    for (i = 0; i < true_rows.size(); ++i) {
        cout << true_rows[i][0] << " " << true_rows[i][1] << " " << true_rows[i][2] << endl;
    }
    
    // Print False Paritions
    cout << "\nFalse Rows"<<endl;
    for (i = 0; i < false_rows.size(); ++i) {
        cout << false_rows[i][0] << " " << false_rows[i][1] << " " << false_rows[i][2] << endl;
    }

    // information gain
    float gain = info_gain(true_rows,false_rows,g);

    */

    // find_best_split 
    //Question q_best = find_best_split(all_rows);
    //float i_gain = 0;
    //Question q_best = find_best_split(all_rows, i_gain);
    
    // Decision-Dode
    //Decision_Node d();

    // Leaf-Node
    //Leaf l(all_rows);
    //l.print();
    
    

    // end timer
    stop_timer();
    time = elapsed_time();

    END:
    printf("\nFinished Program in %f sec",time);
    printf("\n[Finished the Program!]\n");
    return 0;

}



// assignment operator: int a; int b;   a=b
// copy constructor:    int a; int b=a; 

    /* 1.) (doesnt work decause the parenthesis are doing something funny ...)
    Leaf  l2();
    Leaf* l2_ptr;
    l2_ptr = &l2;
    */

    /* works
    int b = 10;
    int* b_ptr; 
    b_ptr = &b;
    cout << "b = "<< *b_ptr << endl;
    */



// attempt 1
/*
void build_tree(vector<vector<string>> rows, Decision_Node* return_node_p){
    // {{{

    float gain = 0;
    Question q = find_best_split(rows, gain);

    // BASE CASE:
    if (gain < 0.1){}
    
    // partition data based on question
    vector<vector<string>> true_rows, false_rows;
    partition(rows,q,true_rows,false_rows);

    // create left and right branches
    Decision_Node* true_b;
    Decision_Node* false_b;

    build_tree(true_rows,  true_b);
    build_tree(false_rows, false_b);

    // return the decision node
    Decision_Node return_node(q, gain, gini(rows), true_b, false_b);
    *return_node_p = return_node;

    return

} // }}}
*/

/*
// attempt 2
void build_tree(vector<vector<string>> &rows, Decision_Node* return_node_p){
    // {{{

    float gain = 0;
    Question q = find_best_split(rows, gain);

    // partition data based on question
    vector<vector<string>> true_rows, false_rows;
    partition(rows,q,true_rows,false_rows);
    //cout << "\n[build tree] true rows  size: " << true_rows.size()  << endl;
    //cout << "\n[build tree] false rows size: " << false_rows.size() << endl;

    // used for base case.
    float curr_gini  = gini(rows);
    float true_gini  = gini(true_rows);
    float false_gini = gini(false_rows);

    // create left and right branches & leaves
    //Decision_Node* true_b  = nullptr;
    //Decision_Node* false_b = nullptr;
    //Leaf* true_leaf        = nullptr;
    //Leaf* false_leaf       = nullptr;

    // BASE CASE CONDITIONS:
    // use if statements to assign the leaves and nodes 
    cout << "\n[START] ...."<<endl;

    // assign both leaves and nodes
    if (true_gini < 0.5 && false_gini < 0.1) {
        cout << "\nBOTH LEAVES -----------------------------------------"<<endl;
        cout << "true-gini = "<<true_gini << " | false-gini = "<< false_gini << endl;
        Leaf t(true_rows);
        Leaf f(false_rows);
        true_leaf  = &t;
        false_leaf = &f;
    }
    // assign true leaf
    else if (true_gini < 0.1) {
        cout << "\nLEFT LEAVE ------------------------------------------"<<endl;
        cout << "true-gini = "<<true_gini << " | false-gini = "<< false_gini << endl;
        Leaf t(true_rows);
        *true_leaf = t;
        build_tree(false_rows, false_b);
    }
    // assign false leaf
    else if (false_gini < 0.1) {
        cout << "\nRIGHT LEAVE -----------------------------------------"<<endl;
        cout << "true-gini = "<<true_gini << " | false-gini = "<< false_gini << endl;
        Leaf f(false_rows);
        *true_leaf = f;
        build_tree(true_rows, true_b);
    }
    // dont assign any leaves
    else {
        cout << "\nNO LEAVES -------------------------------------------"<<endl;
        cout << "true-gini = "<<true_gini << " | false-gini = "<< false_gini << endl;
        // instantiate the left and right branches
        build_tree(true_rows,  true_b);
        build_tree(false_rows, false_b);
    }

    cout << "\nMAGE IT [FINISH] -----------------------------------------"<<endl;

    // assign the final decision node
    Decision_Node return_node(q, gain, curr_gini, true_b, false_b, true_leaf, false_leaf);
    *return_node_p = return_node;

} // }}}
*/

