#include<iostream>
#include<vector>

using namespace std;

int main(int argc, char *argv[])
{
    int i;
    vector<int> vec(10);
    for (i = 0; i < vec.size(); ++i) {
        cout << vec[i] << ",";
    }
    
    cout << "[FINISHED]" << endl;
    return 0;
}




