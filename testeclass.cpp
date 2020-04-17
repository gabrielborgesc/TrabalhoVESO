#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int t = 0;

class teste{
public: int x;
        int y;
        int z = 5;
        vector <int> v;


        void print() {
            cout<<x<<"   "<<y<<"   "<<z<<endl;
        }

        void f() {
            x+=t;
        }

        void somax(){
        x++;
        }

        bool empty(){
        return v.empty();
            }

        int size() {
            return v.size();
        }


};

class teste2 : public teste{
  public:
    void somax() {
        if(x==2){
            teste::somax();
        }
        else{
           x+=2;
        }
    }
};


void f(teste &t){
    t.x++;
}

int main () {

teste t1, t2, t3, t4;
t1.x = 1;
t2.x = 2;
t3 = t2;

vector<teste> v;
v.push_back(t1);
v.push_back(t2);
v.push_back(t3);

for(int i=0; i<v.size(); i++){
    cout<<v[i].x<<"  ";
}





return 0;
}
