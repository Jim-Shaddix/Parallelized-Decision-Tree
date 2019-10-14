

#include <iostream>
#include <map>
#include <vector>

using namespace std;

int main(int argc, char *argv[])
{
    vector<string> words;
    words.push_back("first");
    words.push_back("second");
    words.push_back("third");
    words.push_back("fourth");
    words.push_back("fifth");

    map<string,int> wordMap;

    // assighn variables to map
    for (int i = 0; i < words.size(); ++i) {
        wordMap[words[i]] = i;
    }

    // print results
    map<string,int>::iterator i;

    // Accessing the elements using iterators
    for (i = wordMap.begin(); i != wordMap.end(); ++i){
        cout << i->first <<"  ->  "<< i->second << endl;
    }

    cout << "[FINSHED]" <<endl;
    return 0;
}

  //map<string,int>::iterator iter;
  //for( iter = stringCounts.begin(); iter != stringCounts.end(); iter++ ) {
  //  cout << "word: " << iter->first << ", count: " << iter->second << endl;
  //}
