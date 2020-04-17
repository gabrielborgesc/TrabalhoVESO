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
    int tiex = 0; ///instante em que entrou em execu��o
    int tes; /// tempo em que o processo est� executando op de E/S
    int ties = 0; /// instante em que o processo iniciou a opera��o de E/S
    int tq1; /// tempo em que o processo est� na fila q1
    int tiq1 = 0; ///tempo em que o processo entrou na fila q1
    int qes; /// quantidade total de opera��es de E/S do processo
    int res; /// qauntidade restante de opera��es de E/S do processo
    filas f; ///fila � qual pertence o processo

    void ex() { ///m�todo que represnta execu��o de 1ms do processo
    rbcpu--;
    tex = t - tiex;
    }

    void es(){ ///m�todo que represnta execu��o de 1ms de op de E/S do processo
    tes = t - ties;
    }

};

class fila0{ ///classe para fila Q0 e fila de terminados
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



    int size(){
        return p.size();
    }

     proc pop(){
        proc paux = p[0];
        p.erase(p.begin());
        return paux;
    }

    bool fex() { ///m�todo que indica que processo terminou seu burst
        return p[0].rbcpu==0;
    }

    bool fexrr(){ ///m�todo que indica que processo deve parar execu��o porque chegou ao tempo de RR sem acabar seu burst
        return p[0].tex == trr && p[0].rbcpu!=0;
    }
};

class fila1 : public fila0 { ///classe para fila Q1
public:
    void update(int idex) { ///recebe id do processo em execu��o
        for(int i=0; i<p.size(); i++){
            if(p[i].id != idex){ ///significa que este processo da fila Q1 n�o est� em execu��o
                p[i].tq1 = t - p[i].tiq1; ///atualiza tempo em que cada processo est� em espera na fila Q1
            }
        }
    }
};

class filaes : public fila0 { ///classe para fila de E/S
private:
    static const int totales = 20; ///dura��o de todas as op de E/S

public:
     bool fex() { ///m�todo que verifica fim de op de E/S
        return p[0].tes == totales;
    }

    void exes() {
        p[0].tes = t - p[0].ties;
    }

    void update() {
        if(!p.empty())
        p[0].tes = t - p[0].ties;
    }
};

class filaex : public fila1 { ///classe o processo em execu��o
public:

    bool fexrr(){ ///m�todo que indica se processo em execu��o � de Q0 e terminou por RR
        return p[0].f == Q0 && fila0::fexrr();
    }

    bool fexQ0(){ ///m�todo que indica se processo em execu��o � de Q0 e terminou seu burst
        return p[0].f == Q0 && fex();
    }

    bool fexQ1(){ ///m�todo que indica se processo em execu��o � de Q1 e terminou seu burst
        return p[0].f == Q1 && fex();
    }

    bool intpriority(fila0 q0) { ///m�todo que indica se processo em execu��o � de Q1 e foi interrompido porque chegou processo em Q0
        return !p.empty() && !q0.empty() && (p[0].id != q0.p[0].id) && (p[0].f = Q1);
    }

    void update() {
        if(!p.empty())
        p[0].tex = t - p[0].tiex;
        p[0].rbcpu--;
    }

    void moveQ0toQ1(fila0 &q0, fila1 &q1){
        q0.pop();
        proc paux = p[0];
        p.pop_back();
        q1.ins(paux);
    }

    void moveQ0toES(fila0 &q0, filaes &es){
        q0.pop();
        proc paux = p[0];
        p.pop_back();
        es.ins(paux);
    }

    void moveQ1toES(fila1 &q1, filaes &es){
        q1.pop();
        proc paux = p[0];
        p.pop_back();
        es.ins(paux);
    }

    void replaceInQ1(fila1 &q1){ ///m�todo para atualizar par�metros de um processo que sai da fila de ex mas permanece em Q1
        proc paux = p[0];
        p.pop_back();

        for(int i=0; i<q1.size(); i++){
            if(q1.p[i].id == paux.id){
                q1.p[i] = paux;
                break;
            }
        }
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
cout<<"Quantidade de processos: ";
cin>>qtproc;

cout<<endl<<endl;

for(i=0; i<qtproc; i++){ ///pegar dura��o de burst de cada processo
    cout<<"Duracao do surtos de cpu do processo "<<i<<": ";
    cin>>x;
    dscpu.push_back(x);
    }

cout<<endl<<endl;

for(i=0; i<qtproc; i++){ ///pegar qtd de E/S de cada processo
    cout<<"Quantidade de operacoes de E/S do processo "<<i<<": ";
    cin>>x;
    qesp.push_back(x);
    }

for(i=0; i<qtproc; i++){ ///criando os processos e adicionando-os � fila Q0
    aux.id = i+1;
    aux.bcpu = aux.rbcpu = dscpu[i];
    aux.qes = aux.res = qesp[i];
    aux.f = Q0;
    q0.ins(aux);
}

cout<<endl<<endl;

///atualizando tempos de execu��o/espera
ex.update();
q1.update(ex.p[0].id);
es.update();

if(ex.empty()){ ///ningu�m est� em execu��o
    if(!q0.empty()){
        aux = q0.p[0];
        aux.tiex = t;
        ex.ins(aux);
    }
    else if(!q1.empty()){
        aux = q1.p[0];
        aux.tiex = t;
        ex.ins(aux);
    }
}

else { ///existe processo em execu��o

if(ex.fexrr()){ ///processo que est� em execu��o � de Q0 e terminou por RR
ex.moveQ0toQ1(q0, q1); ///move processo da fila Q0 para Q1
}

else if(ex.fexQ0()){ ///processo que est� em execu��o � de Q0 e temrinou por bcpu
ex.moveQ0toES(q0, es); ///move processo da fila Q0 para executar opera��o de E/S


///aqui falta conferir se o processo ainda tem E/S pra executar. Se n, mover pra term

 }

else if(ex.fexQ1()){ ///processo que est� em execu��o � de Q1 e temrinou por bcpu
ex.moveQ1toES(q1, es); ///move processo da fila Q1 para executar opera��o de E/S
}

else if(ex.intpriority()){ ///processo que est� em execu��o � de Q1 e deve ser interrompido porque chegou processo em Q0
ex.replaceInQ1(q1); ///atualiza os par�metros do processo em Q1
}

}
t++;
}
return 0;
}
