#include <iostream>
#include <vector>

using namespace std;

int t = 0; ///instante em ms da simulação (variável global)

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
    int tex; ///tempo em que o processo está em execução
    int tiex = 0; ///instante em que entrou em execução
    int tes; /// tempo em que o processo está executando op de E/S
    int ties = 0; /// instante em que o processo iniciou a operação de E/S
    int tq1; /// tempo em que o processo está na fila q1
    int tiq1 = 0; ///tempo em que o processo entrou na fila q1
    int qes; /// quantidade total de operações de E/S do processo
    int res; /// qauntidade restante de operações de E/S do processo
    int term; ///instante em que processo terminou toda sua execução
    filas f; ///fila à qual pertence o processo

    void ex() { ///método que represnta execução de 1ms do processo
    rbcpu--;
    tex = t - tiex;
    }

    void es(){ ///método que represnta execução de 1ms de op de E/S do processo
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

    bool fex() { ///método que indica que processo terminou seu burst
        return p[0].rbcpu==0;
    }

    bool fexrr(){ ///método que indica que processo deve parar execução porque chegou ao tempo de RR sem acabar seu burst
        return p[0].tex == trr && p[0].rbcpu!=0;
    }
};

class fila1 : public fila0 { ///classe para fila Q1
private:
    static const int tmaxq1 = 30;
public:
    void update(int idex) { ///recebe id do processo em execução
        for(int i=0; i<p.size(); i++){
            if(p[i].id != idex){ ///significa que este processo da fila Q1 não está em execução
                p[i].tq1 = t - p[i].tiq1; ///atualiza tempo em que cada processo está em espera na fila Q1
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
            q0.ins(paux[i]);
        }

    }

};

class filaes : public fila0 { ///classe para fila de E/S
private:
    static const int totales = 2; ///duração de todas as op de E/S

public:
     bool fex() { ///método que verifica fim de op de E/S
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
        q0.ins(paux);
    }
};

class filaex : public fila1 { ///classe o processo em execução
public:

    bool fexrr(){ ///método que indica se processo em execução é de Q0 e terminou por RR
        return p[0].f == Q0 && fila0::fexrr();
    }

    bool fexQ0(){ ///método que indica se processo em execução é de Q0 e terminou seu burst
        return p[0].f == Q0 && fex();
    }

    bool fexQ1(){ ///método que indica se processo em execução é de Q1 e terminou seu burst
        return p[0].f == Q1 && fex();
    }

    bool stop(fila0 q0) { ///método que indica se processo em execução é de Q1 e foi interrompido porque chegou processo em Q0
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
        es.ins(paux);
    }

    void moveQtoTerm(fila0 &q0, fila0 &term){
        q0.pop();
        proc paux = p[0];
        p.erase(p.begin());
        paux.term = t;
        term.ins(paux);
    }

    void replaceInQ1(fila1 &q1){ ///método para atualizar parâmetros de um processo que sai da fila de ex mas permanece em Q1
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


void print(fila0 q0, fila1 q1, filaex ex, filaes es, fila0 term){
int i;
cout<<endl;
cout<<"t = "<<t<<" - "<<t+1<<":"<<endl;
cout<<"em execucao: ";
for(i=0; i<ex.size(); i++){
    cout<<"   P"<<ex.p[i].id;
}

cout<<endl<<"Fila Q0:";
for(i=0; i<q0.size(); i++){
    cout<<"   P"<<q0.p[i].id;
}

cout<<endl<<"Fila Q1:";
for(i=0; i<q1.size(); i++){
    cout<<"   P"<<q1.p[i].id;
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

int main () {

int i, x, idex, qtproc; ///quantidade total de processos

fila0 q0; ///fila Q0
fila1 q1; ///fila Q1
filaex ex; ///fila de execução (apenas 1 processo nesse vetor). É vetor apenas para verificar com empty array se há processo em execução ou não
filaes es; ///fila de E/S
fila0 term; ///fila de processos terminados

vector <int> dscpu; ///vetor com a duração de brust cpu de cada processo
vector <int> qesp; ///vetor com a quantidade de op de E/S de cada processo
proc aux;

/// pegando os dados iniciais
cout<<"Quantidade de processos: ";
cin>>qtproc;

cout<<endl<<endl;

for(i=0; i<qtproc; i++){ ///pegar duração de burst de cada processo
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

for(i=0; i<qtproc; i++){ ///criando os processos e adicionando-os à fila Q0
    aux.id = i+1;
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

print(q0, q1, ex, es, term);

while(1){
t++;
if(q0.empty() && q1.empty() && es.empty()) break;

///atualizando tempos de execução/espera
ex.update();

if(!ex.empty()){
idex = ex.p[0].id;
}
else{
idex = -1;
}
q1.update(idex);

es.update();

///tratando ex
if(!ex.empty()) { ///existe processo em execução

if(ex.fexrr()){ ///processo que está em execução é de Q0 e terminou por RR
ex.moveQ0toQ1(q0, q1); ///move processo da fila Q0 para Q1
}

else if(ex.fexQ0()){ ///processo que está em execução é de Q0 e temrinou por bcpu
if(ex.p[0].res!=0){
ex.moveQtoES(q0, es); ///move processo da fila Q0 para executar operação de E/S
}
else{
ex.moveQtoTerm(q0, term); ///move processo da fila Q0 para fila de terminados
}
}

else if(ex.fexQ1()){ ///processo que está em execução é de Q1 e temrinou por bcpu
if(ex.p[0].res!=0){
ex.moveQtoES(q1, es); ///move processo da fila Q1 para executar operação de E/S
}
else{
ex.moveQtoTerm(q1, term); ///move processo da fila Q1 para fila de terminados
}
}

else if(ex.stop(q0)){ ///processo que está em execução é de Q1 e deve ser interrompido porque chegou processo em Q0
ex.replaceInQ1(q1); ///atualiza os parâmetros do processo em Q1 e retira processo de ex
}

}

///tratando Q1
q1.elapsedTime(q0); ///analisa se há processos em Q1 esperando há 30ms e os desloca de Q1 para Q0

///tratando E/S
if(!es.empty()){
    if(es.fex()){ ///processo executando E/S terminou sua operação
        es.moveEStoQ0(q0);
    }
}

if(ex.empty()){ ///ninguém está em execução
    if(!q0.empty()){
        aux = q0.p[0];
    }
    else if(!q1.empty()){
        aux = q1.p[0];
    }

    if(!q0.empty() || !q1.empty()){
        aux.rbcpu = aux.bcpu;
        aux.tiex = t;
        aux.tex = 0;
        ex.ins(aux);
    }

}

print(q0, q1, ex, es, term);

}
return 0;
}
