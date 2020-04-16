#include <iostream>
#include <vector>
using namespace std;

int t = 0;

class teste{
public: int x;
        int y;
        int z = 5;


        void print() {
            cout<<x<<"   "<<y<<"   "<<z<<endl;
        }

        void f() {
            x+=t;
        }

        void somax(){
        x++;
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

int main () {

teste t1;
t1.x = 3;
t1.y = 3;

teste2 t2;
t2.x = 3;
t2.y = 3;

t1.somax();
t2.somax();

t1.print();
t2.print();

return 0;
}
