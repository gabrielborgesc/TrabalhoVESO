#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int t = 0;

class teste{
public: int x;
        static int y;
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

teste t1, t2;


t1.x = 1;
t2.x = 5;





return 0;
}
