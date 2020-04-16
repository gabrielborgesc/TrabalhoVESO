#include <iostream>
#include <vector>

using namespace std;

int t = 0; ///instante em ms da simula��o (vari�vel global)

enum filas {
    Q0,
    Q1,
    ES
};

class proc{
public:
    int id;
    int bcpu; /// tempo de cada surto de cpu do processo
    int rbcpu; /// burst cpu ainda restante
    int tex; ///tempo em que o processo est� em execu��o
    int tiex; ///instante em que entrou em execu��o
    int tes =0; /// tempo em que o processo est� executando op de E/S
    int ties; /// instante em que o processo iniciou a opera��o de E/S
    int tq1 =0; /// tempo em que o processo est� na fila q1
    int qes; /// quantidade de opera��es de E/S do processo
    filas f; ///fila � qual pertence o processo

    void ex() { ///m�todo que represnta execu��o de 1ms do processo
    rbcpu--;
    tex = t - tiex;
    }

    void es(){ ///m�todo que represnta execu��o de 1ms de op de E/S do processo
    tes = t - ties;
    }

};

class fila0{
private:
    static const int trr = 10;

public:
    vector <proc> p;

    void ins(proc x){
        p.push_back(x);
    }

    bool empty(){
        return p.empty();
    }

    proc get(int pos) {
        return p[pos];
    }

protected:
    bool fex() {
        return p[0].tex == trr;
    }
};

class fila1 : public fila0 {
protected:
    bool fex() {
        return p[0].rbcpu==0;
    }
};

class filaes : public fila0 {
private:
    static const int totales = 20; ///dura��o de todas as op de E/S

public:
     bool fex() { ///m�todo que verifica fim de op de E/S
        return p[0].tes == totales;
    }
};

class filaex : public fila1 {

    bool fex() {
        if(p[0].f == Q0)
            return fila0::fex();

        if(p[0].f == Q1)
            return fila1::fex();

        cout<<endl<<endl<<"Fez algo de errado, pois este processo consta na fila de ES"<<endl<<endl;
        return false;
    }
};

int main () {

int i, x, qtproc; ///quantidade total de processos

fila0 q0; ///fila Q0
fila1 q1; ///fila Q1
filaex ex; ///fila de execu��o (apenas 1 processo nesse vetor). � vetor apenas para verificar com empty array se h� processo em execu��o ou n�o
filaes es; ///fila de E/S
fila0 term; ///fila de processos terminados

vector <int> dscpu; ///vetor com a dura��o de brust cpu de cada processo
vector <int> qesp; ///vetor com a quantidade de op de E/S de cada processo
proc aux;


while(1){
t = 0;
cout<<"Quantidade de processos: ";
cin>>qtproc;

cout<<endl<<endl;

for(i=0; i<qtproc; i++){
    cout<<"Duracao do surtos de cpu do processo "<<i<<": ";
    cin>>x;
    dscpu.push_back(x);
    }

cout<<endl<<endl;

for(i=0; i<qtproc; i++){
    cout<<"Quantidade de operacoes de E/S do processo "<<i<<": ";
    cin>>x;
    qesp.push_back(x);
    }

for(i=0; i<qtproc; i++){
    aux.id = i+1;
    aux.bcpu = aux.rbcpu = dscpu[i];
    aux.qes = qesp[i];
    aux.f = Q0;
    q0.ins(aux);
}

cout<<endl<<endl;

if(ex.empty()){ ///ningu�m est� em execu��o
    if(!q0.empty()){
        ex.ins(q0.get(0));
    }
    else if(!q1.empty()){
        ex.ins(q1.get(0));
    }
t++;
}

else { ///existe processo em execu��o

}

}
return 0;
}
