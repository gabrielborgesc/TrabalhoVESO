#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;


enum fila {
    a = 0,
    b = 3,
    c = 2
};

bool operator==(vector<int> v1, vector<int> v2){
    return v1[0]==v2[0];
}

int main () {

vector <int> v1, v2;
v1.push_back(1);
v1.push_back(2);
v1.push_back(3);

v2.push_back(1);
v2.push_back(3);
v2.push_back(3);
bool t = v1==v2;
cout<<t;

cout<<endl<<endl;
return 0;
}
