
#include<iostream>
#include<algorithm>
#include<vector>

using namespace std;


void print_vec(vector<string> myvector){
    for (int i = 0; i < myvector.size(); ++i)
        cout << myvector[i] << ", ";
    cout << endl;
}

bool myfunction (string i, string j) {
  return (i==j);
}

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

int main(int argc, char *argv[])
{
    vector<string> myvector;

    myvector.push_back("1");
    myvector.push_back("2");
    myvector.push_back("3");
    myvector.push_back("4");
    myvector.push_back("5");
    myvector.push_back("6");
    myvector.push_back("7");
    
    myvector.push_back("1");
    myvector.push_back("1");
    myvector.push_back("1");

    cout << "original" << endl;
    print_vec(myvector);

    // ----------------------------------------------------------------------------

    vector<string> newvector = get_unique(myvector);

    // ----------------------------------------------------------------------------
    cout << "modified" << endl;
    print_vec(newvector);

    cout << "[FINISHED]" << endl;
    return 0;
}


