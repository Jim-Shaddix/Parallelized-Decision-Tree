#include<iostream>

using namespace std;

int volume(int l, int w =2, int h=3){
    return l*w*h;
}

int main(int argc, char *argv[])
{
    cout << volume(4) << endl; 
    cout << "[FINISHED]" << endl;
    return 0;
}

