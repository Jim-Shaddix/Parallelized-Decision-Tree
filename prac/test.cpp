#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char *argv[])
{

    vector<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
 
    vector<vector<int>> vv;
    vv.push_back(v);
    vv.push_back(v);
    vv.push_back(v);

    vector<vector<vector<int>>> vvv;
    vvv.push_back(vv);
    vvv.push_back(vv);
    vvv.push_back(vv);

    cout << "[FINISHED]" << endl;
    return 0;
}

