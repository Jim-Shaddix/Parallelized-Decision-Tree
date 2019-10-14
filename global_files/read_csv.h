#ifndef READ_CSV
#define READ_CSV

#include <vector>   // dynamic arrays

using namespace std;

vector<string> get_file_headers(string &file_name, const char sep);

vector<vector<string>> get_row_data(string &file_name, const char sep, int header_size);

int get_T_index(vector<string> &headers, string Target);

void remove_rows(vector<vector<string>> &rows, string delimiter);

void stratified_random_sample(
        vector<vector<string>>& rows, 
        float per_train, 
        vector<vector<string>>& training_data, 
        vector<vector<string>>& testing_data, 
        int T_index);

void get_file_data( 
        string file_name, // file name to to read data from
        const char sep,   // delimiter used in the file between values
        string Target,    // (Target) column name that I will be predicting on
        int num_elements, // number of elements to read from the file
        string delimiter, // if columns contain these values, they will removed
        float percent_training, // percentage of data to turn into training data
        int  &T_index,    // index of the target column
        vector<string> &headers, // headers used in the file
        vector<vector<string>> &all_rows,       // data stored as rows
        vector<vector<string>> &training_data,  // data to train with
        vector<vector<string>> &testing_data    // data to test with
        );

void linear_sample(
        vector<vector<string>>& rows, 
        float per_train, 
        vector<vector<string>>& training_data, 
        vector<vector<string>>& testing_data);

# endif /* TIMER_H */

