

#include<iostream>
#include<map>
#include<vector>


using namespace std;



int main(int argc, char *argv[])
{
    map<string, vector<string>> map_vec;

    map_vec["t"].push_back("c");
    map_vec["tt"].push_back("cc");
    map_vec["ttt"].push_back("ccc");
    map_vec["tttt"].push_back("cccc");
    map_vec["ttttt"].push_back("ccccc");

    map<string, vector<string>> :: iterator iter;

    int i;
    for (iter = map_vec.begin(); iter != map_vec.end(); ++iter) {

        cout << iter -> first<< "\t";
        vector<string> vec = iter -> second;

        for (i = 0; i < vec.size(); ++i) {
            cout << vec[i] << endl;
        }

    }

    // testing vectors copy by refference
    vector<int> stuff_1;
    stuff_1.push_back(1);

    vector<int> stuff_2 = stuff_1;
    stuff_2.push_back(2);

    for (i = 0; i < stuff_1.size(); ++i) {
        cout << "[ELEMENT] " << stuff_1[i] << endl;
    }
    float a  = 5 * 4 /
              2;
    cout << "a = " << a <<endl;


    cout << "[FINIHED]"<<endl;
    return 0;
}
