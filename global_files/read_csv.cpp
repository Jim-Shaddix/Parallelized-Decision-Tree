#include <iostream> // C++ output 
#include <fstream>  // reading files
#include <sstream>  // reading strings [as streams]
#include <iomanip>   // setw (set width on cout)
#include <algorithm> // random_shuffle
#include <map>

#include "read_csv.h" 

#define PRINT_HEADERS      0
#define PRINT_ELEMENTS     0
#define PRINT_TARGET_INDEX 0
#define PRINT_REMOVED_ROWS 0
#define PRINT_STRATIFIED   0
#define PRINT_GET_FILE_DATA 1

vector<string> get_file_headers(string &file_name, const char sep){
    // {{{

    ifstream myFile;
    myFile.open(file_name);

    // read first line [header line]
    string header_line;
    getline(myFile,header_line,'\n'); // read a line until you get to a comma

    // tokenize the first line
    stringstream ss(header_line);
    string token;
    vector<string> headers;
    while(getline(ss,token,sep)){headers.push_back(token);}
    myFile.close();

    // [PRINT] headers
    if (PRINT_HEADERS){
        cout <<"\nHEADERS: [COLUMNS = "<<headers.size()<<"]"<<endl;
        for (int i = 0; i < headers.size() ; ++i) {
            cout << setw(20) << headers[i];
        }
        cout<<endl;
    }

    return headers;
} // }}}

vector<vector<string>> get_row_data(string &file_name, const char sep, int header_size, int max_elements){ 
    //{{{
    
    // open file
    ifstream myFile;
    myFile.open(file_name);

    // [CREATE] row vectors
    vector<vector<string>> all_rows;

    // these two lines of code make sure that I skip over the header line.
    string header_line;
    getline(myFile,header_line,'\n'); 

    // [STORE] elements into vectors
    // - read file while there is something in the file
    int element_count = -1;
    
    while(myFile.good() && element_count < max_elements) { 

        // read a line until you get to a newline
        string line;
        getline(myFile,line,'\n'); 

        // variables for tokenization 
        stringstream ss(line);
        string token;

        // tokenize every row, & store elements in column array
        vector<string> row;
        while(getline(ss,token,sep)){
            row.push_back(token);
        }
        all_rows.push_back(row);
        element_count++;
    }

    // < -- [ERROR] -- >
    // for some reason, I am storing one to many elements, 
    // so I am deleting one here
    all_rows.pop_back();

    // [PRINT]
    if (PRINT_ELEMENTS){
        cout<<"\nELEMENTS: [ROWS = "<<all_rows.size()<<"] [COLUMNS =  "<<
              all_rows[0].size()<<"]"<<endl;
        for (int i = 0; i < all_rows.size(); ++i) {
            for (int j = 0; j < all_rows[i].size(); ++j) {
                cout << setw(20) << all_rows[i][j];
            }
            cout << endl;
        }
    }

    // close & return
    myFile.close();
    return all_rows;
}   // }}}

int get_T_index(vector<string> &headers, string Target){
   // {{{
    int T_index = -1;
    for (int i = 0; i < headers.size(); ++i) 
        if (headers[i] == Target){
            T_index = i; break;}
    
    // check that the target was found in the files.
    if (T_index == -1){
        cout << "the target variable was not found in the file ..."<<endl;
        exit(0);
    }

    if (PRINT_TARGET_INDEX){
        cout << "\nTARGET INDEX = [" << T_index<<"]" <<
            " :  HEADER = ["<<headers[T_index]<<"]"<< endl;
    }
    return T_index;

}   // }}}

void remove_rows(vector<vector<string>> &rows, string delimiter){
    // {{{
    
    /* removes all rows that contain a value specified by the delimiter. */

    int i,j;   // to iterate width
    bool flag; // set if delimeter found
    int count_removed = 0;

    if (PRINT_REMOVED_ROWS){{
        cout << "\n[REMOVING ROWS]" << endl;
        cout << "Initial Row Count: " << rows.size()<< "\n" << endl;
    }}
    
    // iterate through all of the rows
    int index = 2;
    vector<vector<string>>:: iterator iter;
    for (iter = rows.begin(); iter != rows.end(); ++iter) {

        // iterate through each element in a row, and set the flag
        flag = false;
        for (j = 0; j < (*iter).size(); ++j){
            if ((*iter)[j] == delimiter) flag = true;
        }

        if (flag){

            // print row
            if (PRINT_REMOVED_ROWS){
                cout << setw(15) << "REMOVED ROW[" << index << "]: ";
                for (i = 0; i < (*iter).size(); ++i)
                    cout << setw(20) << (*iter)[i];
                cout << endl;
            }

            // remove row
            rows.erase(iter); count_removed++;
            iter--;
        }

        index++;
    } // outer iterating loop

    if(PRINT_REMOVED_ROWS){
        cout << "\nREMOVED: " << count_removed << " rows" << endl;
        cout << "Final Row Count: " << rows.size() << endl;
    }

} // }}}

void stratified_random_sample(
        vector<vector<string>>& rows, 
        float per_train, 
        vector<vector<string>>& training_data, 
        vector<vector<string>>& testing_data, 
        int T_index){
    // {{{
    
    /* Returns a stratified random sample of training and testing data*/

    int i; // index iterator
    
    // randomize the rows
    random_shuffle(rows.begin(), rows.end());

    // map [class, rows belonging to that class]
    map < string,vector<vector<string>> > class_rows_map;

    // store map values [class, rows]
    for (i = 0; i < rows.size(); ++i) 
        class_rows_map[rows[i][T_index]].push_back(rows[i]);

    // pick random elements
    map < string,vector<vector<string>> >::iterator iter;
    for (iter = class_rows_map.begin(); iter != class_rows_map.end(); ++iter) {

        // number of elements to store for a particular class
        int num_to_pick = int(per_train * float((iter -> second).size()));

        // store elements in training and testing data
        for (i = 0; i < (iter->second).size(); ++i) {

            if (i < num_to_pick)
                training_data.push_back((iter->second)[i]);
            else
                testing_data.push_back((iter->second)[i]);
        }
    }

    if (PRINT_STRATIFIED){
        cout << "\n[TRAINING DATA] (" << per_train <<")"<< endl;
        cout << "training size: "   << training_data.size() 
             << "\tout of:  "       << rows.size()
             << "\tfrac: "<< float(training_data.size()) / float(rows.size())
             << endl;

        cout << "\n[TESTING  DATA] (" << 1.0-per_train << ")" << endl;
        cout << "testing  size: "  << testing_data.size() 
             << "\tout of:  "      << rows.size()
             << "\tfrac: "<< float(testing_data.size()) / float(rows.size())
             << endl;
    }

    
} // }}}

void linear_sample(
        vector<vector<string>>& rows, 
        float per_train, 
        vector<vector<string>>& training_data, 
        vector<vector<string>>& testing_data)
        {
    // {{{
    int i;
    int s = rows.size();
    for (i = 0; i < s; ++i) {
        
        if( float((i+1))/s <= per_train){
            training_data.push_back(rows[i]);
        }
        else{
            testing_data.push_back(rows[i]);
        }
    }
    
} // }}}

void get_file_data( 
        string file_name, // file name to to read data from
        const char sep,   // delimiter used in the file between values
        string Target,    // (Target) column name that I will be predicting on
        int max_elements, // number of elements to read from the file
        string delimiter, // if columns contain these values, they will removed
        float percent_training, // percentage of data to turn into training data
        int  &T_index,    // index of the target column
        vector<string> &headers, // headers used in the file
        vector<vector<string>> &all_rows,       // data stored as rows
        vector<vector<string>> &training_data,  // data to train with
        vector<vector<string>> &testing_data    // data to test with
        ){
        // {{{

    // assign global variables
    headers = get_file_headers(file_name,sep);
    T_index = get_T_index(headers,Target);

    // read in data
    all_rows = get_row_data(file_name,sep,headers.size(),max_elements);
    int size_total = all_rows.size();

    // remove bad rows
    remove_rows(all_rows, string("?"));
    int remove_total = size_total - all_rows.size();

    // stratified random sample
    //stratified_random_sample(all_rows, percent_training, training_data, 
    //testing_data, T_index);
    linear_sample(all_rows, percent_training, training_data, testing_data);

    if (PRINT_GET_FILE_DATA ){
        cout << "\nTarget:                   " << Target
             << "\nTarget index:             " << T_index
             << "\n# of attributes:          " << headers.size()
             << "\n# of rows read in:        " << size_total
             << "\n# of rows removed:        " << remove_total
             << "\n# of rows used:           " << all_rows.size()
             << "\ntotal # of elements used: " << all_rows.size()*headers.size()
             << "\ntraining data size:       " << training_data.size() 
                << "  " << float(training_data.size()) /
                           float(all_rows.size()) * 100 << "%"
             << "\ntesting  data size:       " << testing_data.size() 
                << "  " << float(testing_data.size()) / 
                           float(all_rows.size()) * 100 << "%"
             
             << endl;
    }
    
} // }}}

