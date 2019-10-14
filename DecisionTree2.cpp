#include <iostream>  // C++ output 
#include <fstream>   // reading files
#include <sstream>   // reading strings [as streams]

#include <cstdlib>   // used for random number generator
#include <numeric>   // accumulate (concatenate vector of strings) 
#include <iomanip>   // setw (set width on cout)
#include <string>    // allows me to use stoi (string -> int)
#include <algorithm> // allows me to check for unique values in a vector

#include "global_files/timer.h"  
#include "global_files/useful-func.h"
#include "global_files/read_csv.h"
#include "DecisionTree2.h"
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
int T_index    = 0;
int node_count = 1;

// [headers][unique-elements][unique elements from target]
vector<vector<vector<int>>> blanck_matrices;

// <-- CLASSIFICATION -->
// ----------------------------------------------------------------------------

 
Leaf* get_leaf(vector<int> &row, Decision_Node* tree){
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

int classify(vector<int> &row, Decision_Node* tree){
    // {{{
    Leaf* leaf = get_leaf(row,tree);
    return leaf->prediction;
}   // }}}

vector<bool> classify_all(vector<vector<int>> &rows, Decision_Node* tree){
    // {{{

    vector<bool> classifications(rows.size());

    for (int i = 0; i < rows.size(); ++i) 
        classifications.push_back(classify(rows[i],tree) == rows[i][T_index]);
    
    return classifications;
} // }}}

void print_classification_results(vector<vector<int>> &rows, Decision_Node* tree){
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
Question::Question(int col, int v){
    column = col;
    value = v;
}

void Question::print(){
    string condition = " == ";
    //if (is_float(value)) {condition = " >= ";}
    cout << "[QUESTION] Is " << headers[column] << condition << value << "?";
} // }}}

// <-- Leaf Methods -->
// {{{
Leaf::Leaf(vector<vector<int>> &rows, vector<vector<vector<int>>> &count_matrix){
    this->count_matrix = count_matrix;
    this->rows = rows;
}

void Leaf::store_prediction(){
    get_class_prediction(); // store the prediction
}


void Leaf::get_class_prediction(){
    
    // [STORE]: The counts associated with each class of the target.
    //  - just walk across the diagonal of the target count matrix,
    //    and store the values into a vector.
    int val, j;
    int max = 0;
    for (j = 0; j < count_matrix[T_index].size(); ++j) {
        val = count_matrix[T_index][j][j]; 
        if (max < val) {
            max = val;
            prediction = j;
        }
    }

}

void Leaf::print(){
    cout << "\n[PREDICTION] " << prediction << endl;
}
// }}}

// <-- Decision Node Methods -->
// {{{

Decision_Node::Decision_Node(Question* q, 
                             Decision_Node* t_branch, 
                             Decision_Node* f_branch){

    this->q            = q;
    //this->gain         = gain;
    //this->impurity     = impurity;

    true_branch  = t_branch;
    false_branch = f_branch;
    leaf_node    = NULL;
}

Decision_Node::Decision_Node(){
    //impurity = 0.0;
    //gain     = 0.0;
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

Decision_Node* build_tree(vector<vector<int>> &rows, vector<vector<vector<int>>> &count_matrices){
    // {{{

    float gain = 0;
    Question* q = new Question();
    *q = find_best_question(count_matrices, rows.size(), gain);


    // by makeing epsilon very small, it will fully build the tree
    float epsilon = 0.0000000001;

    // base case
    if ( gain < epsilon){
        if(DEBUG_BUILD_TREE) {cout << "[FOUND LEAF]" <<endl;}
        Decision_Node* terminating_node = new Decision_Node();
        Leaf* l = new Leaf(rows, count_matrices);
        terminating_node -> leaf_node = l;
        return terminating_node;
    }

    // partition data based on question
    vector<vector<int>> true_rows, false_rows;
    vector<vector<vector<int>>> true_count_matrices   = blanck_matrices;
    vector<vector<vector<int>>> false_count_matrices  = blanck_matrices;

    partition(rows, *q, true_rows,           false_rows, 
                        true_count_matrices, false_count_matrices);

    // used for base case.
    //float curr_gini  = gini(rows);

    // create left and right branches & leaves
    Decision_Node* true_b  = new Decision_Node;
    Decision_Node* false_b = new Decision_Node;

    //if(DEBUG_BUILD_TREE) {
    //    cout << "true-gini = " << gini(true_rows) << " | false-gini = " << gini(false_rows) << endl;
    //    cout << "number of elements: "<<rows.size()<<endl;
    //}
    
    // < -- RECURSIE CALLS (on true and false branches) -->

    // true branch
    if(DEBUG_BUILD_TREE){cout << "\n[TRUE NODE] ------------------"<<endl;}
    node_count++;
    true_b = build_tree(true_rows, true_count_matrices);  

    // false branch
    if(DEBUG_BUILD_TREE){cout << "\n[FALSE NODE] -----------------"<<endl;}
    node_count++;
    false_b = build_tree(false_rows, false_count_matrices); 

    // <-- results -->
    
    // assign the final decision node
    Decision_Node* return_node = new Decision_Node(q, true_b, false_b);

    // *return_node_p = return_node;
    return return_node;

} // }}}


// <-- NEW -->
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// <-- COUNT MATRICES -->
// ----------------------------------------------------------------------------
// used by convert rows
vector<string> get_unique(vector<string> &myvector){
    // {{{

    int i,j;
    vector<string> newvector;

    for (i = 0; i < myvector.size(); ++i) {
       if (find(newvector.begin(), newvector.end(), myvector[i]) != newvector.end()){
           continue;
       }
       else{newvector.push_back(myvector[i]);}
    }
    return newvector;
} // }}}

vector<int> get_unique(vector<int> &myvector){
    // {{{

    int i,j;
    vector<int> newvector;

    for (i = 0; i < myvector.size(); ++i) {
       if (find(newvector.begin(), newvector.end(), myvector[i]) != newvector.end()){
           continue;
       }
       else{newvector.push_back(myvector[i]);}
    }
    return newvector;
} // }}}

void convert_rows(vector<vector<string>> &all_rows, 
                  vector<vector<int>>    &all_rows_converted) {
    // {{{

    int i,j;
    
    // create map of headers[int], unique-element[string] -> index[int]
    map<int, map<string,int>> element_index;

    // 1. create columns
    // 2. get unique elements from columns
    // 3. store elements in map
    
    int size = all_rows.size();
    vector<string> column;

    // for each header create a mapping
    for (i = 0; i < headers.size(); ++i) {

        // store elements in column
        for (j = 0; j < size; ++j) 
            column.push_back(all_rows[j][i]);
        
        // get unique elements
        vector<string> unique_col =  get_unique(column);

        // map elements to their index
        for (j = 0; j < unique_col.size(); ++j) 
            element_index[i][unique_col[j]] = j;

        // clear the current column
        column.clear();
    }

    // convert all strings to integers
    for (i = 0; i < all_rows.size(); ++i) {
        vector<int> row;
        for (j = 0; j < all_rows[0].size(); ++j) {
            row.push_back(element_index[j][all_rows[i][j]]);
        }
       all_rows_converted.push_back(row);
    }

} // }}}

vector<vector<int>> get_all_unique_elements(vector<vector<int>>  &all_rows_converted){
    // {{{

    int i,j;

    // get unique elements associated with each row
    vector<vector<int>> row_to_uniques;

    // for each header create a mapping
    for (i = 0; i < headers.size(); ++i) {

        vector<int> column;

        // store elements in column
        for (j = 0; j < all_rows_converted.size(); ++j) 
            column.push_back(all_rows_converted[j][i]);
        
        // get unique elements
        vector<int> unique_col =  get_unique(column);

        // store unique elements in the column
        row_to_uniques.push_back(unique_col);
    }

    if (false) {
        for (i = 0; i < headers.size(); ++i) {
            vector<int> unic = row_to_uniques[i];
            cout << "[COLUMN-" << i+1 << "] ";
            for (j = 0; j < unic.size(); ++j) {
                cout << unic[j] << ",";
            } 
            cout << endl;
        }
    }

    return row_to_uniques;
} // }}}

vector<vector<vector<int>>> set_count_matrices(vector<vector<int>>  &all_rows_converted){
    // {{{
    
    int i,j;

    vector<vector<vector<int>>> count_matrices = blanck_matrices;
    
    // start counting values for the count matrices
    bool DEBUG_SET_COUNTS = false;
    for (j = 0; j < all_rows_converted.size(); ++j) {
        for (i = 0; i < headers.size(); ++i) {

            int target = all_rows_converted[j][T_index];
            int val = all_rows_converted[j][i];

            if (DEBUG_SET_COUNTS){
                cout << "\n# of elements: " << count_matrices[i].size() 
                     << " by " << count_matrices[i][target].size();
                cout << "\nindex (" << val << "," << target << ")" << endl;
            }

            count_matrices[i][val][target]++;
            //cout << "value = " << count_matrices[i][val][val1]<<endl;;
            
        }
    }

    return count_matrices;
    

} // }}}

void print_CM(vector<vector<vector<int>>> count_matrices){
    // {{{

    int i,j,k; // variables to iterate over

    cout << endl;

    for (i = 0; i < headers.size(); ++i) {

        int total = 0;
        vector<int> num_in_class(count_matrices[i][0].size(),0);
        cout << "Count Matrix [" << i << "]: " << headers[i] << endl;

        for (j = 0; j < count_matrices[i].size(); ++j) {

            cout << setw(3) << j << " | ";
            int num_in_row = 0;

            for (k = 0; k < count_matrices[i][j].size(); ++k) {

                int val = count_matrices[i][j][k];
                num_in_row      += val;
                total           += val;
                num_in_class[k] += val;
                cout << setw(8) << val;

            } 
            cout << "   | " << setw(8) << num_in_row;
            cout << endl;
        }

        print_times("-",11+8*(count_matrices[i][0].size()+1));
        cout << setw(3) << "~" << " | ";
        for (k = 0; k < num_in_class.size(); ++k){
            cout << setw(8) << num_in_class[k];
        }
        cout << "   | " << setw(8) << total;
        cout << "\n\n" << endl;
    }

} // }}}

void print_CM_sizes(vector<vector<vector<int>>> count_matrices){
    // {{{ 
    int i;
    cout << "\nNumber of Matrices: " << count_matrices.size() << endl;
    for (i = 0; i < blanck_matrices.size(); ++i) {
        cout << "matrix[" << i << "] size: (" << blanck_matrices[i].size() 
             << "," << blanck_matrices[i][0].size() << ")" << endl;
    }
} // }}}

void store_blanck_matrices(vector<vector<int>> all_rows_converted){
    // {{{
    // initializes all of the blanck matrix elements to zero.
    // - there is a blanck vector associated with each feature
    // - each vector in the blanck matrix contains a size that corresponds to
    //   the number of unique elements that are inside the associated feature

    int i;

    // get unique elements
    vector<vector<int>> unique_elements = get_all_unique_elements(all_rows_converted);

    // store blanck matrix
    vector<vector<int>> count_matrix;
    for (i = 0; i < unique_elements.size(); ++i) {
        vector<vector<int>> count_matrix(unique_elements[i].size(),
                vector<int>(unique_elements[T_index].size(),0));
        blanck_matrices.push_back(count_matrix);
    }

} // }}}
// ----------------------------------------------------------------------------

// <-- ALGORITHM -->
// ----------------------------------------------------------------------------
Question find_best_question(vector<vector<vector<int>>> &count_matrices, int row_count, float &best_gain){
    // {{{

    // PARAMETERS
    // 1. count_matrices: used for finding the best question
    // 2. row_count: needed in order to calculate the gini impurity

    int i,j,k; // used to iterate over
    float gini_true, gini_false, gain; // variables for the cost function

    // [STORE]: The counts associated with each class of the target.
    //  - just walk across the diagonal of the target count matrix,
    //    and store the values into a vector.
    vector<int> unique_target_counts;
    for (j = 0; j < count_matrices[T_index].size(); ++j) {
        unique_target_counts.push_back(count_matrices[T_index][j][j]);        
    }

    // gini current
    float gini_curr = 1;
    for (i = 0; i < unique_target_counts.size(); ++i) {
        gini_curr -= (float(unique_target_counts[i])) / (row_count) * 
                     (float(unique_target_counts[i])) / (row_count);
    }
    if (false) cout << "current gini "<< gini_curr << endl;
    
    // these values will be updated by the loop
    Question best_question;
    best_gain = 0;

    // loop over matrices (features)
    for (i = 0; i < headers.size(); ++i) {

        // skip over the target index
        if (i == T_index){continue;}

        // base case:
        // - if there is only one unique element, there is not point
        //   in splitting there
        if (count_matrices[i].size() == 1){continue;}

        // loop over the rows in each matrix
        for (j = 0; j < count_matrices[i].size(); ++j) {
            
            // a question is proposed for each row
            
            // determine gini true
            float gini_true  = 1.0;
            
            // 1.) find how many elements our in the corresponding row
            int true_row_count = 0;
            for (k = 0; k < count_matrices[i][j].size(); ++k){
                true_row_count += count_matrices[i][j][k];
            }

            // handle empty row case
            if (true_row_count == 0 || true_row_count == row_count) {
                continue;
            }
            
            // 2.) subtract off prababilites squared
            for (k = 0; k < count_matrices[i][j].size(); ++k) {
                gini_true -= (float(count_matrices[i][j][k]) / true_row_count)   *
                             (float(count_matrices[i][j][k]) / true_row_count);
            }

            // determine gini false
            float gini_false = 1.0;

            // 1.) find how many false rows
            int false_row_count = row_count - true_row_count;

            // 2.) subtract off prababilites squared
            for (k = 0; k < unique_target_counts.size(); ++k) {
                float val = (float((unique_target_counts[k] - count_matrices[i][j][k]))
                            / false_row_count); 
                gini_false -=  val*val;
            }

            // find information gain
            gain = gini_curr - (float(true_row_count)  / row_count) * gini_true
                             - (float(false_row_count) / row_count) * gini_false;

            if (false){
                cout << "size: " << count_matrices[i].size() << "   ";
                cout << "gini-T: " << setw(12) << gini_true  << "  ";
                cout << "gini-F: " << setw(12) << gini_false << "  ";
                //cout << "t_count: " << setw(8) << true_row_count  << "  ";
                //cout << "f_count: " << setw(8) << false_row_count << "  ";
                //cout << "count:   " << setw(8) << row_count       << "  ";
                //cout << "gini_curr: "<< setw(8) << gini_curr << "  "; 
                //cout << "best_gain: "<< setw(8) << best_gain << "  "; 
                cout << "gain:      "<< setw(8) << gain << endl;
            }

            // i: specifies the feature we are looking at
            // j: specifies the row we are looking at 
            //    - (which also cooresponds to a unique value in that feature)
            Question q(i,j);

            // store best information gain
            if (gain > best_gain){
                best_gain = gain;
                best_question = q;
            }

        } // end: loop over rows
    } // end: loop over matrices

    //best_question.print(); cout << endl;
    
    return best_question;

} // }}}

bool match(Question* q, vector<int>& row){
    // {{{
    // determines whether or note, row evaluates to either true or false,
    // when evaluated by the question.

    int compare = row[q -> column];

    if (PRINT_MATCH){

        // assume string type
        bool ans = (compare == (q -> value));
        string condition = " == ";

        // print results
        cout << "\n[MATCH] [" << headers[q -> column] << "] ";
        cout << "(" << compare << condition << q -> value << ") = " << ans << endl;
    }

    return compare == (q -> value);
} // }}}

void partition(vector<vector<int>>& rows, Question &q, 
        vector<vector<int>> &true_rows, 
        vector<vector<int>> &false_rows,
        vector<vector<vector<int>>> &true_count_matrix,
        vector<vector<vector<int>>> &false_count_matrix){
    // {{{
    //  Partitions a dataset based on a question, and stores the results into
    //  true_rows and false_rows
    
    int i, j, classification;
    for (i = 0; i < rows.size(); ++i) {
        classification = rows[i][T_index];
        if (match(&q,rows[i])){
            true_rows.push_back(rows[i]);

            // update true count matrix
            for (j = 0; j < rows[i].size(); ++j) {
                true_count_matrix[j][rows[i][j]][classification]++;
            }

        }
        else {
            false_rows.push_back(rows[i]);

            // update false count matrix
            for (j = 0; j < rows[i].size(); ++j) {
                false_count_matrix[j][rows[i][j]][classification]++;
            }

        }
    }

    if (false){
        cout << "[PARTITION]" << endl;
        cout << "Number of [True]  Elements: " << true_rows.size() << endl;
        cout << "Number of [False] Elements: " << false_rows.size() << endl;
    }

}   // }}}


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
    //string Target    = "Fruit";
    //string file_name = "data/test.csv";
    //float percent_training = 0.7;
    // ---------------------------------------------------------------------}}}

    // READ IN DATA {{{
    //-------------------------------------------------------------------------
    
    // variables to initialize
    vector<vector<string>> all_rows; 
    vector<vector<string>> training_data;
    vector<vector<string>> testing_data;

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
    // ---------------------------------------------------------------------}}}

    // Init {{{
    // ------------------------------------------------------------------------
    // convert matrices to integers
    vector<vector<int>> training_data_converted;
    vector<vector<int>> testing_data_converted;
    vector<vector<int>> all_rows_converted;

    convert_rows(training_data, training_data_converted);
    convert_rows(testing_data,  testing_data_converted);
    convert_rows(all_rows,      all_rows_converted);

    // print the converted rows
    //print_2D(training_data_converted);

    // store blanck matrix sizes
    store_blanck_matrices(all_rows_converted);
    //print_CM_sizes(blanck_matrices);

    // set count matrices
    vector<vector<vector<int>>> count_matrices = set_count_matrices(training_data_converted);
    //print_CM(count_matrices);
    // ---------------------------------------------------------------------}}}

    // RUN ALGORITHM {{{
    // ------------------------------------------------------------------------

    // start timer
    initialize_timer();
    start_timer();

    //build-tree
    print_times("-",50); 
    cout << "[STARTED] BUILDING DECISION TREE \n" << endl;
    Decision_Node* ddd = build_tree(training_data_converted, count_matrices);
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
    print_classification_results(training_data_converted, ddd);
    //cout << "[TESTING  DATA RESULTS] = ";
    //print_classification_results(testing_data_converted,  ddd);
    print_times("-",50);

    // end timer
    stop_timer();
    float test_time = elapsed_time();
    cout << "time to test results: " << test_time << "\n\n\n" << endl;
    // ---------------------------------------------------------------------}}}
    
    // CHECK {{{
    // ------------------------------------------------------------------------
    // - used to make sure each function is running appropriately
    if (false){

        // find best question / information gain for a count matrix
        float best_gain = 0;
        Question q = find_best_question(count_matrices, 
                                        all_rows_converted.size(),
                                        best_gain);

        // print results from finding the best question
        cout << "gain found: "<< best_gain << endl;
        q.print(); cout << endl;

        // <-- partition the data -->
        vector<vector<int>> true_rows;
        vector<vector<int>> false_rows;
        vector<vector<vector<int>>> true_count_matrix  = blanck_matrices;
        vector<vector<vector<int>>> false_count_matrix = blanck_matrices;

        partition( all_rows_converted, q,
                   true_rows,          false_rows,
                   true_count_matrix,  false_count_matrix );

        print_CM(true_count_matrix);

        // create leaf node
        Leaf l_t(true_rows, true_count_matrix);
        l_t.store_prediction();
        cout << "[True Prediction]  = " << l_t.prediction << endl;

        Leaf l_f(false_rows, false_count_matrix);
        l_f.store_prediction();
        cout << "[False Prediction] = " << l_f.prediction << endl;
    }
    // ---------------------------------------------------------------------}}}

    END:
    print_times("-",80);
    cout << "\nalgorithm build time: " << algo_time;
    cout << "\n[Finished the Program!]" << endl;
    return 0;

}

