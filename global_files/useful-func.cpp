#include <iostream>
#include <iomanip>   // setw (set width on cout)
#include "useful-func.h"

void print_2D(vector<vector<string>>& vec_matrix){
    // {{{
    int i,j;
    for (i = 0; i < vec_matrix.size(); ++i) {
        for (j = 0; j < vec_matrix[i].size(); ++j) {
            cout<< setw(10) << vec_matrix[i][j] << "\t";
        }
        cout << endl;
    }
} // }}}


void print_2D(vector<vector<int>>& vec_matrix){
    // {{{
    int i,j;
    for (i = 0; i < vec_matrix.size(); ++i) {
        for (j = 0; j < vec_matrix[i].size(); ++j) {
            cout << vec_matrix[i][j] << ",";
        }
        cout << endl;
    }
} // }}}



bool is_float( const string &in) {
    // {{{
    /* checks if a string can be representd as a float*/
	stringstream sstr(in);
	float f;
	return bool(sstr >> f);
} // }}}

void print_times(string s, int times){
    // {{{
    int i;
    for (i = 0; i < times; ++i) 
        cout << s;
    cout << endl;
} // }}}
