#include <iostream>  // C++ output 
#include <fstream>   // reading files
#include <sstream>   // reading strings [as streams]

#include <cstdlib>   // used for random number generator
#include <numeric>   // accumulate (concatenate vector of strings) 
#include <iomanip>   // setw (set width on cout)
#include <omp.h>
#include <cmath>     // used for the absolute value function

#include "global_files/timer.h"  
#include "global_files/useful-func.h"
#include "global_files/read_csv.h"
#include "DecisionTree1.h"
//#include "Decision-Node.h"

#define PRINT_CLASS_COUNTS 0
#define PRINT_GINI         0
#define PRINT_MATCH        0
#define PRINT_GAIN         0
#define PRINT_BEST_Q       0
#define PRINT_PARTITION    0

#define DEBUG_GET_LEAF     0
#define DEBUG_BUILD_TREE   0 // use print best q and partition as well

using namespace std;

// GLOBAL VARIABLES
vector<string> headers;
int T_index = 0;
int node_count = 1;


// <-- CLASSIFICATION -->
// ----------------------------------------------------------------------------

Leaf* get_leaf(vector<string> &row, Decision_Node* tree){
    //   {{{
    //
    // returns a leaf associated with a particular row
    // - this is the used by the classify function, in order to 
    //   determine the classifcation of a particular node

    //cout << "[START NEW CALL]" << endl;
    //if (tree == false){cout << "Found a null pointer!!!" << endl;}

    // BASE CASE: at a leaf node
    if(DEBUG_GET_LEAF){cout << "about 1 ..." << endl;}
    if ( (tree -> leaf_node) != NULL ) {
        if(DEBUG_GET_LEAF){cout << "MADE IT TO HERE! 1" << endl;}
        (tree -> leaf_node)->store_prediction();
        return (tree -> leaf_node);
    }

    // [RECURSIVE CALL] True Branch
    if(DEBUG_GET_LEAF){cout << "about 2 ..." << endl;}
    if ( match((tree -> q),row) ) {
        if(DEBUG_GET_LEAF){cout << "MADE IT TO HERE! 2" << endl;}
        return get_leaf(row, tree -> true_branch);
    }

    // [RECURSIVE CALL] False Branch
    if(DEBUG_GET_LEAF){cout << "about 3 ..." << endl;}
    if(DEBUG_GET_LEAF){cout << "MADE IT TO HERE! 3" << endl;}
    return get_leaf(row, tree -> false_branch);

} // }}}

string classify(vector<string> &row, Decision_Node* tree){
    // {{{
    Leaf* leaf = get_leaf(row,tree);
    return leaf->prediction;
}   // }}}

vector<bool> classify_all(vector<vector<string>> &rows, Decision_Node* tree){
    // {{{

    vector<bool> classifications(rows.size());

    for (int i = 0; i < rows.size(); ++i) 
        classifications.push_back(classify(rows[i],tree) == rows[i][T_index]);
    
    return classifications;
} // }}}

void print_classification_results(vector<vector<string>> &rows, Decision_Node* tree){
    // {{{
    int i;
    vector<bool> bool_classifications = classify_all(rows,tree);
    int count_true = 0;
    for (i = 0; i < bool_classifications.size(); ++i) {
        if (bool_classifications[i]) 
            count_true++;
    }
    cout << 100 * float(count_true) / float(rows.size()) 
         << " percent correct" << endl;
} // }}}


// <-- Classes -->
// ----------------------------------------------------------------------------

// <-- Question Methods -->
// {{{
Question::Question(int col, string v){
    column = col;
    value  = v;
}

void Question::print(){
    string condition = " == ";
    //if (is_float(value)) {condition = " >= ";}
    cout << "[QUESTION] Is " << headers[column] << condition << value << "?";
} // }}}

// <-- Leaf Methods -->
// {{{
Leaf::Leaf(vector<vector<string>>& rows){
    this->rows = rows;
}

void Leaf::store_prediction(){
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
    } // end for
}

void Leaf::print(){
    cout << "\n[PREDICTION] " << prediction << endl;
}
// }}}

// <-- Decision Node Methods -->
// {{{

Decision_Node::Decision_Node(Question* q, float gain, float impurity, 
        Decision_Node* t_branch, Decision_Node* f_branch){

    this->q            = q;
    this->gain         = gain;
    this->impurity     = impurity;

    true_branch  = t_branch;
    false_branch = f_branch;
    leaf_node    = NULL;
}

Decision_Node::Decision_Node(){
    impurity = 0.0;
    gain     = 0.0;
    q            = NULL;
    true_branch  = NULL;
    false_branch = NULL;
    leaf_node    = NULL;
}

Decision_Node::~Decision_Node(){
    delete true_branch;
    delete false_branch;
    delete leaf_node;
    delete q;
}// }}}


// // <-- ALGORITHM -->
// ----------------------------------------------------------------------------

bool match(Question* q, vector<string>& row){
    // {{{
    // determines whether or note, row evaluates to either true or false,
    // when evaluated by the question.

    string compare = row[q -> column];

    if (PRINT_MATCH){

        // assume string type
        bool ans = (compare == (q -> value));
        string condition = " == ";

        // print results
        cout << "\n[MATCH] [" << headers[q -> column] << "] ";
        cout << "(" << compare << condition << q -> value << ") = " << ans << endl;
    }

    return compare == q -> value;
} // }}}

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

    // faster for small cases
    if (rows.size() == 1){return 0;}

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

    if (impurity ==1){return 0;}
    return impurity;
}   // }}}

void partition(vector<vector<string>>& rows, Question &q, 
        vector<vector<string>> &true_rows, vector<vector<string>> &false_rows){
    // {{{
    /*
     * Partitions a dataset based on a question, and stores the results into
     * true_rows and false_rows
     */
    int i;
    for (i = 0; i < rows.size(); ++i) {
        if (match(&q,rows[i]))
            true_rows.push_back(rows[i]);
        else
            false_rows.push_back(rows[i]);
    }
}   // }}}

float info_gain(vector<vector<string>> &true_rows, 
        vector<vector<string>> &false_rows, float curr_gini){
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
        cout << "[True-Gini = " << g1 << "] [False Gini = " << g2 << 
            "] [Curr Gini = "<<curr_gini<<"]"<<endl;
        cout << "Information Gain: "<< curr_gini - (frac_true*g1 + (1-frac_true)*g2) 
             << endl;
    }

    return curr_gini - (frac_true*g1 + (1-frac_true)*g2);
}   //}}}

Question find_best_split(vector<vector<string>> &rows, float &i_gain){
    /* {{{
     * [RETURNS]
     *      - the best question to split the data, or null, if the data could
     *        not be split, indicating that we are in a leaf node.
     */

    // Init variables

    // shared variables
    float best_gain = 1e-20;
    Question best_question;

    // private varaibles
    float impurity = gini(rows);


    float gain;
    int best_val_picked = -1;

    static int d_count = 0;
    d_count++;

    // Loop over all elements, and test all possible questions, store the best
    int j;
    #pragma omp parallel for private(gain, best_val_picked,j) 
    for (int i = 0; i < rows.size(); ++i) {

        int val_picked;

        for (j = 0; j < rows[i].size(); ++j) {

            if (j == T_index){continue;}

            // refference so I can decide which question to pick
            val_picked = i*rows[0].size()+j;

            // generate question
            Question q(j,rows[i][j]);

            // store true & false rows (based on question)
            vector<vector<string>> true_rows;
            vector<vector<string>> false_rows;
            partition(rows, q, true_rows, false_rows); 

            // eliminates useless questions & questions that have already been asked
            if (true_rows.size() == 0 || false_rows.size() == 0){continue;}

            // information gain of question
            gain = info_gain(true_rows, false_rows, impurity);

            // remove elements
            //true_rows.clear();
            //false_rows.clear();

            // store best results
            #pragma omp critical 
            {

                int tid = omp_get_thread_num();

                // check normal
                if (gain > best_gain){
                    best_gain     = gain; 
                    best_question = q;
                    best_val_picked = val_picked;
                    //cout << "[node-count = " << d_count << "]";
                    //cout << "[tid: " << tid 
                    //     << "] [val-picked= " << val_picked << "] ";
                    //cout << headers[j] << endl;
                }

                // check equals
                //else if ( gain == best_gain
                //      && best_val_picked < val_picked){
                //    best_gain       = gain; 
                //    best_question   = q;
                //    best_val_picked = val_picked;
                //    print_times("-",50); 
                //    cout << "[node-count = " << d_count << "]";
                //    cout << "[tid: " << tid << "] [val-picked= " << val_picked << "] ";
                //    cout << headers[j] << endl;
                //}

                // check approx
                else if ( (abs(gain - best_gain) <= 1e-21) 
                      && best_val_picked < val_picked){
                    best_gain       = gain; 
                    best_question   = q;
                    best_val_picked = val_picked;
                    //print_times("-",50); 
                    //cout << "[node-count = " << d_count << "]";
                    //cout << "[tid: " << tid 
                    //     << "] [val-picked= " << val_picked << "] ";
                    //cout << headers[j] << endl;
                    //cout << " (gain-best)="
                    
                }

            }
            

        } // inner for
    } // outer for

    /*
    if (PRINT_BEST_Q){
        true_rows.clear();
        false_rows.clear();
        cout <<"\n[BEST_QUESTION] "; 
        best_question.print();
        cout << "[GAIN] " << best_gain << endl;
        cout << "[PARTITION] ";
        partition(rows, best_question, true_rows, false_rows); 
        cout << "(true-size, false-size) =\t\t     (" 
            << true_rows.size()  << "," << false_rows.size() << ")\n";
        //cout << "[false rows size = " << false_rows.size() << "]" << endl;
    }
    */

    //print questions
    //best_question.print(); 
    //cout << "| best-gain="<<best_gain<<endl;

    // assign the best gain
    i_gain = best_gain;

    return best_question;
    
} // }}}

Decision_Node* build_tree(vector<vector<string>> &rows){
    // {{{

    float gain = 0;
    Question* q = new Question();
    *q = find_best_split(rows, gain);

    // by makeing epsilon very small, it will fully build the tree
    float epsilon = 1e-9;

    // base case
    if ( gain < epsilon){
        if(DEBUG_BUILD_TREE) {cout << "[FOUND LEAF]" <<endl;}
        Decision_Node* terminating_node = new Decision_Node();
        Leaf* l = new Leaf(rows);
        terminating_node -> leaf_node = l;
        return terminating_node;
    }

    // partition data based on question
    vector<vector<string>> true_rows, false_rows;
    partition(rows,*q,true_rows,false_rows);

    // used for base case.
    float curr_gini  = gini(rows);

    // create left and right branches & leaves
    Decision_Node* true_b  = new Decision_Node;
    Decision_Node* false_b = new Decision_Node;

    if(DEBUG_BUILD_TREE) {
        cout << "true-gini = " << gini(true_rows) << " | false-gini = " << gini(false_rows) << endl;
        cout << "number of elements: "<<rows.size()<<endl;
    }
    
    // < -- RECURSIE CALLS (on true and false branches) -->

    // true branch
    if(DEBUG_BUILD_TREE){cout << "\n[TRUE NODE] ------------------"<<endl;}
    node_count++;
    true_b = build_tree(true_rows);  

    // false branch
    if(DEBUG_BUILD_TREE){cout << "\n[FALSE NODE] -----------------"<<endl;}
    node_count++;
    false_b = build_tree(false_rows); 

    // <-- results -->
    
    // assign the final decision node
    Decision_Node* return_node = new Decision_Node(q, gain, curr_gini, true_b, false_b);

    return return_node;

} // }}}

int main(int argc, char *argv[]){

    // DEFINING DATA {{{
    // ------------------------------------------------------------------------
    
    // DEFINING VARIABLES
    float percent_training = 0.7;  // percent of the data to train on
    const char sep         = ',';  // what each element in file is sepperated by
    int num_elements       = 1000000000; // number of elements to read in
    if (argc > 1) num_elements = atoi(argv[1]);
    string delim           = "?"; // if any elements in each row of the file contain
                                  // this value, that row will be deleted
    
    // breast data set
    //string Target    = "irradiat"; // column name to classify on
    //string file_name = "data/breast-cancer/breast-cancer.data"; // file to read data from

    // real data set
    string Target    = "poisonous"; // column name to classify on
    string file_name = "data/agaricus-lepiota-shuffle.data"; // file to read data from

    // small test data set
    //string Target    = "fruit-type";
    //string file_name = "data/small/test.csv";

    double time;
    
    // variables to initialize
    vector<vector<string>> all_rows; 
    vector<vector<string>> training_data;
    vector<vector<string>> testing_data;
    // ---------------------------------------------------------------------}}}

    // READ IN DATA {{{
    //-------------------------------------------------------------------------
    // start timer
    initialize_timer();
    start_timer();

    // initialize all variables by reading in data
    cout << endl;
    print_times("-",50);
    cout << "[STARTED] READING IN DATA" << endl;
    get_file_data(file_name, sep, Target, num_elements, delim, percent_training, T_index,
                  headers, all_rows, training_data, testing_data);
    cout << "\n[FINISHED] READING IN DATA:\n";
    print_times("-",50);

    // end timer
    stop_timer();
    float read_time = elapsed_time();
    cout << "time to read in data: " << read_time << "\n\n\n" << endl;

    // }}}

    // RUN ALGORITHM {{{
    //-------------------------------------------------------------------------
    
    // start timer
    initialize_timer();
    start_timer();

    //build-tree
    print_times("-",50); 
    cout << "[STARTED] BUILDING DECISION TREE \n" << endl;
    if(DEBUG_BUILD_TREE){ cout << "\ngini initial = " << gini(training_data) << endl; }
    Decision_Node* ddd = build_tree(training_data);
    cout << "Number of Nodes Created = "<<node_count << endl;
    cout << "\n[FINISHED] BUILDING DECISION TREE:\n";
    print_times("-",50);


    // end timer
    stop_timer();
    float algo_time = elapsed_time();
    cout << "algorithm build time: " << algo_time << "\n\n\n" << endl;
    // ---------------------------------------------------------------------}}}

    // PRINT RESULTS {{{
    //-------------------------------------------------------------------------
    
    // start timer
    initialize_timer();
    start_timer();

    // print results
    print_times("-",50); 
    cout << "[RESULTS]"<< endl;
    cout << "[TRAINING DATA RESULTS] = ";
    print_classification_results(training_data, ddd);
    //cout << "[TESTING  DATA RESULTS] = ";
    //print_classification_results(testing_data,  ddd);
    print_times("-",50);

    // end timer
    stop_timer();
    float test_time = elapsed_time();
    cout << "time to test results: " << test_time << "\n\n\n" << endl;

    END:
    print_times("-",80);
    cout << "\nalgorithm build time: " << algo_time;
    printf("\n[Finished the Program!]\n");
    return 0;
    // ---------------------------------------------------------------------}}}

}

