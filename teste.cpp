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
v.push_back(2);
v.push_back(2);
v.push_back(3);
replace(v.begin(), v.end(),  2, 5);

for(int i=0; i<v.size(); i++){
    cout<<v[i]<<"  ";
}

cout<<endl<<endl;
return 0;
}
