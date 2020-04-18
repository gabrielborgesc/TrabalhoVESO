#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;


enum fila {
    a = 0,
    b = 3,
    c = 2
};

int main () {

vector <int> v;
v.push_back(1);
v.push_back(2);
v.push_back(3);
v.pop_back();

for(int i=0; i<v.size(); i++){
    cout<<v[i]<<"  ";
}

for(i=0; i<5; i++){
    cout<<i<<endl;
}

cout<<endl<<endl;
return 0;
}
