#include <iostream>
#include <vector>

using namespace std;

int t = 0; ///instante em ms da simula��o (vari�vel global)
int lastt = 0; ///instante em ms da �ltima mudan�a na configura��o das filas (vari�vel global)
enum filas {
    Q0,
    Q1,
    ES,
    TERM
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
    int term; ///instante em que processo terminou toda sua execu��o
    filas f; ///fila � qual pertence o processo
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
private:
    static const int tmaxq1 = 30;
public:
    void update(int idex) { ///recebe id do processo em execu��o
        for(int i=0; i<p.size(); i++){
            if(p[i].id != idex){ ///significa que este processo da fila Q1 n�o est� em execu��o
                p[i].tq1 = t - p[i].tiq1; ///atualiza tempo em que cada processo est� em espera na fila Q1
            }
        }
    }

    void elapsedTime(fila0 &q0){
        vector <proc> paux;
        int i, j;
        for(i=1; i<p.size(); i++){ ///armazenando os processos de Q1 que chegaram a 30ms de espera
            if(p[i].tq1 == tmaxq1){
                paux.push_back(p[i]);
            }
        }

        for(i=0; i<paux.size(); i++){ ///retirando esses processos da fila Q1
            for(j=0; j<p.size(); j++){
                if(paux[i].id == p[j].id){
                    p.erase(p.begin()+j);
                    break;
                }
            }
        }

        for(i=0; i<paux.size(); i++){ ///inserindo esses processos em Q0
            paux[i].f=Q0;
            q0.ins(paux[i]);
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

    void update() {
        if(!p.empty()){
        p[0].tes = t - p[0].ties;
    }
    }

    void moveEStoQ0(fila0 &q0){
        proc paux = p[0];
        paux.res--;
        p.erase(p.begin());
        if(!p.empty()){
            p[0].ties = t;
        }
        paux.f = Q0;
        q0.ins(paux);
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

    bool stop(fila0 q0) { ///m�todo que indica se processo em execu��o � de Q1 e deve ser interrompido porque chegou processo em Q0
        return !p.empty() && !q0.empty() && (p[0].id != q0.p[0].id) && (p[0].f = Q1);
    }

    void update() {
        if(!p.empty()){
        p[0].tex = t - p[0].tiex;
        p[0].rbcpu--;
    }
    }

    void moveQ0toQ1(fila0 &q0, fila1 &q1){
        q0.pop();
        proc paux = p[0];
        p.erase(p.begin());
        paux.tiq1 = t;
        paux.tq1 = 0;
        paux.f = Q1;
        q1.ins(paux);
    }

    void moveQtoES(fila0 &q0, filaes &es){
        q0.pop();
        proc paux = p[0];
        p.erase(p.begin());
        if(es.empty()){
        paux.ties = t;
        }
        paux.tes = 0;
        paux.f = ES;
        es.ins(paux);
    }

    void moveQtoTerm(fila0 &q0, fila0 &term){
        q0.pop();
        proc paux = p[0];
        p.erase(p.begin());
        paux.term = t;
        paux.f = TERM;
        term.ins(paux);
    }

    void replaceInQ1(fila1 &q1){ ///m�todo para atualizar par�metros de um processo que sai da fila de ex mas permanece em Q1
        proc paux = p[0];
        p.erase(p.begin());

        for(int i=0; i<q1.size(); i++){
            if(q1.p[i].id == paux.id){
                paux.tiq1 = t;
                paux.tq1 = 0;
                q1.p[i] = paux;
                break;
            }
        }
    }

};

bool operator==(fila0 q, fila0 aux){
        vector <int> v, vaux;
        int i;
        for(i=0; i<q.size(); i++){
            v.push_back(q.p[i].id);
        }

        for(i=0; i<aux.size(); i++){
            vaux.push_back(aux.p[i].id);
        }
        return v==vaux;
    }

void print(fila0 q0, fila1 q1, filaex ex, filaes es, fila0 term, fila0 auxq0, fila1 auxq1, filaex auxex, filaes auxes, fila0 auxterm, bool fim){
int i;

if(fim){
cout<<endl;
cout<<"t = "<<t-1<<" (fim) "<<endl;
cout<<"em execucao: ";
for(i=0; i<ex.size(); i++){
    cout<<"   P"<<ex.p[i].id;//<<"  rbcpu: "<<auxex.p[i].rbcpu<<"  f = "<<auxex.p[i].f<<"   ";
}

cout<<endl<<"Fila Q0:";
for(i=0; i<q0.size(); i++){
    cout<<"   P"<<q0.p[i].id;//<<"  rbcpu: "<<auxq0.p[i].rbcpu<<"  f = "<<auxq0.p[i].f<<"   ";
}

cout<<endl<<"Fila Q1:";
for(i=0; i<q1.size(); i++){
    cout<<"   P"<<q1.p[i].id;//<<"  rbcpu: "<<auxq1.p[i].rbcpu<<"  f = "<<auxq1.p[i].f<<"   ";
}

cout<<endl<<"Fila E/S:";
for(i=0; i<es.size(); i++){
    cout<<"   P"<<es.p[i].id;
}

cout<<endl<<"Fila term:";
for(i=0; i<term.size(); i++){
    cout<<"   P"<<term.p[i].id;
}
cout<<endl;
}
else if(!(q0==auxq0 && q1==auxq1 && ex==auxex && es==es && term==auxterm)){
cout<<endl;
cout<<"t = "<<lastt<<" - "<<t<<":"<<endl;
cout<<"em execucao: ";
for(i=0; i<auxex.size(); i++){
    cout<<"   P"<<auxex.p[i].id;//<<"  rbcpu: "<<auxex.p[i].rbcpu<<"  f = "<<auxex.p[i].f<<"   ";
}

cout<<endl<<"Fila Q0:";
for(i=0; i<auxq0.size(); i++){
    cout<<"   P"<<auxq0.p[i].id;//<<"  rbcpu: "<<auxq0.p[i].rbcpu<<"  f = "<<auxq0.p[i].f<<"   ";
}

cout<<endl<<"Fila Q1:";
for(i=0; i<auxq1.size(); i++){
    cout<<"   P"<<auxq1.p[i].id;//<<"  rbcpu: "<<auxq1.p[i].rbcpu<<"  f = "<<auxq1.p[i].f<<"   ";
}

cout<<endl<<"Fila E/S:";
for(i=0; i<auxes.size(); i++){
    cout<<"   P"<<auxes.p[i].id;
}

cout<<endl<<"Fila term:";
for(i=0; i<auxterm.size(); i++){
    cout<<"   P"<<auxterm.p[i].id;
}
cout<<endl;
lastt = t;
}
}

int main () {

int i, x, idex, qtproc; ///quantidade total de processos

fila0 q0, auxq0; ///fila Q0
fila1 q1, auxq1; ///fila Q1
filaex ex, auxex; ///fila de execu��o (apenas 1 processo nesse vetor). � vetor apenas para verificar com empty array se h� processo em execu��o ou n�o
filaes es, auxes; ///fila de E/S
fila0 term, auxterm; ///fila de processos terminados

vector <int> dscpu; ///vetor com a dura��o de brust cpu de cada processo
vector <int> qesp; ///vetor com a quantidade de op de E/S de cada processo
proc aux;

/// pegando os dados iniciais
cout<<"Quantidade de processos: ";
cin>>qtproc;

cout<<endl<<endl;

for(i=0; i<qtproc; i++){ ///pegar dura��o de burst de cada processo
    cout<<"Duracao do surtos de cpu do processo "<<i+1<<": ";
    cin>>x;
    dscpu.push_back(x);
    }

cout<<endl<<endl;

for(i=0; i<qtproc; i++){ ///pegar qtd de E/S de cada processo
    cout<<"Quantidade de operacoes de E/S do processo "<<i+1<<": ";
    cin>>x;
    qesp.push_back(x);
    }

for(i=0; i<qtproc; i++){ ///criando os processos e adicionando-os � fila Q0
    aux.id = i;
    aux.bcpu = aux.rbcpu = dscpu[i];
    aux.qes = aux.res = qesp[i];
    aux.f = Q0;
    q0.ins(aux);
}

cout<<endl<<endl;

        aux = q0.p[0];
        aux.rbcpu = aux.bcpu;
        aux.tiex = t;
        ex.ins(aux);

///print(q0, q1, ex, es, term, auxq0, auxq1, auxex, auxes, auxterm);

while(1){
t++;
if(q0.empty() && q1.empty() && es.empty()){
print(q0, q1, ex, es, term, auxq0, auxq1, auxex, auxes, auxterm, true);
break;
}
auxq0 = q0;
auxq1 = q1;
auxex = ex;
auxes = es;
auxterm = term;
///atualizando tempos de execu��o/espera
ex.update();

if(!ex.empty()){
idex = ex.p[0].id;
}
else{
idex = -1;
}
q1.update(idex);

es.update();


///tratando E/S
if(!es.empty()){
    if(es.fex()){ ///processo executando E/S terminou sua opera��o
        es.moveEStoQ0(q0);
    }
}

///tratando Q1
q1.elapsedTime(q0); ///analisa se h� processos em Q1 esperando h� 30ms e os desloca de Q1 para Q0

///tratando ex
if(!ex.empty()) { ///existe processo em execu��o

if(ex.fexrr()){ ///processo que est� em execu��o � de Q0 e terminou por RR
ex.moveQ0toQ1(q0, q1); ///move processo da fila Q0 para Q1
}

else if(ex.fexQ0()){ ///processo que est� em execu��o � de Q0 e temrinou por bcpu
if(ex.p[0].res!=0){
ex.moveQtoES(q0, es); ///move processo da fila Q0 para executar opera��o de E/S
}
else{
ex.moveQtoTerm(q0, term); ///move processo da fila Q0 para fila de terminados
}
}

else if(ex.fexQ1()){ ///processo que est� em execu��o � de Q1 e temrinou por bcpu
if(ex.p[0].res!=0){
ex.moveQtoES(q1, es); ///move processo da fila Q1 para executar opera��o de E/S
}
else{
ex.moveQtoTerm(q1, term); ///move processo da fila Q1 para fila de terminados
}
}

else if(ex.stop(q0)){ ///processo que est� em execu��o � de Q1 e deve ser interrompido porque chegou processo em Q0
ex.replaceInQ1(q1); ///atualiza os par�metros do processo em Q1 e retira processo de ex
}

}


if(ex.empty()){ ///ningu�m est� em execu��o
    if(!q0.empty()){
        aux = q0.p[0];
        if(aux.rbcpu==0){
        aux.rbcpu = aux.bcpu;
    }
    }
    else if(!q1.empty()){
        aux = q1.p[0];
    }

    if(!q0.empty() || !q1.empty()){
        aux.tiex = t;
        aux.tex = 0;
        ex.ins(aux);
    }

}

print(q0, q1, ex, es, term, auxq0, auxq1, auxex, auxes, auxterm, false);

}
return 0;
}
