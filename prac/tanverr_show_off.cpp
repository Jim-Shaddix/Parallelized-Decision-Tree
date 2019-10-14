

#include <iostream> //C++
#include<vector>

using namespace std;

int main(int argc, char *argv[])
{


    vector<vector<int>> rows;

    int i, j;

    // append elemnts [1]
    vector<int> num_1;
    for (i = 0; i < 11; ++i) {
        num_1.push_back(i);
    }

    // append elemnts [2]
    vector<int> num_2;
    for (i = 11; i < 21; ++i) {
        num_2.push_back(i);
    }

    rows.push_back(num_1);
    rows.push_back(num_2);

    // print elements
    for (i = 0; i < rows.size(); ++i) {
       for (j = 0; j < rows[i].size(); ++j) {
           cout << rows[i][j] << " ";
       }
       cout << endl;
    }

    cout << "[FINISHED]\n" << endl;
    return 0;
}
