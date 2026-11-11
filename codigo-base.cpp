#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iomanip>

using namespace std;

// Tipos de Carga no Terminal
enum TipoCarga { CONTEINER, GRANEL, REFRIGERADA, PERIGOSA };

// Registro principal de informações da carga
struct Carga {
    string codigo;      // Ex: "CRG0012", "CRG0005"
    TipoCarga tipo;     // Categoria da carga
    float peso;         // Peso em toneladas
    string cliente;     // Nome do proprietário/cliente
    string destino;     // Cidade ou porto de destino
    string posicao;     // Posição no pátio (Ex: "B-02-01" ou "Fila")
    bool necessitaRefrigeracao;
    bool ehPerigosa;
};

// Nó para a Lista Duplamente Encadeada (Pátio de Armazenamento)
struct NoLista {
    Carga carga;
    NoLista* anterior;
    NoLista* proximo;

    NoLista(Carga c) : carga(c), anterior(NULL), proximo(NULL) {}
};

// Estrutura que representa o Pátio (guarda os ponteiros do início e do fim)
struct ListaPatio {
    NoLista* inicio;
    NoLista* fim;

    ListaPatio() : inicio(NULL), fim(NULL) {}
};

// Nó para a Fila de Espera (Cargas Pendentes)
struct NoFila {
    Carga carga;
    NoFila* proximo;

    NoFila(Carga c) : carga(c), proximo(NULL) {}
};

// Estrutura da Fila (ponteiros de início e fim)
struct Fila {
    NoFila* inicio;
    NoFila* fim;

    Fila() : inicio(NULL), fim(NULL) {}
};

// Registro de operação para o Histórico (Pilha Undo)
enum TipoOperacao { ALOCACAO, MOVIMENTACAO, EXPEDICAO };

struct Operacao {
    TipoOperacao tipoOp;
    string codigoCarga;
    string posicaoOrigem;
    string posicaoDestino;
    Carga carga;            // [NOVO] cópia dos dados, necessária para desfazer uma EXPEDICAO
};

// Nó para a Pilha de Histórico
struct NoPilha {
    Operacao op;
    NoPilha* proximo;

    NoPilha(Operacao o) : op(o), proximo(NULL) {}
};

// Estrutura da Pilha (ponteiro para o topo)
struct Pilha {
    NoPilha* topo;

    Pilha() : topo(NULL) {}
};

// Busca uma carga no pátio pelo código e retorna o ponteiro do nó (ou NULL)
NoLista* buscarNoPatio(ListaPatio& patio, const string& codigo) {
    NoLista* atual = patio.inicio;
    while (atual != NULL) {
        if (atual->carga.codigo == codigo) return atual;
        atual = atual->proximo;
    }
    return NULL;
}

// Empilha (push) o registro da operação no topo do histórico
void registrarAcao(Pilha& historico, Operacao op) {
    NoPilha* novo = new NoPilha(op);
    novo->proximo = historico.topo;
    historico.topo = novo;
}

// Desfaz a última operação registrada: lê o topo, executa a ação inversa,
void desfazerUltimaAcao(Pilha& historico, ListaPatio& patio, Fila& fila) {
    if (historico.topo == NULL) {
        cout << "Historico vazio. Nada a desfazer." << endl;
        return;
    }

    NoPilha* noTopo = historico.topo;       // top
    Operacao op = noTopo->op;

    switch (op.tipoOp) {
        case MOVIMENTACAO: {
            // Inversa: devolve a carga à posição de origem
            NoLista* no = buscarNoPatio(patio, op.codigoCarga);
            if (no == NULL) {
                cout << "Carga " << op.codigoCarga << " nao encontrada no patio." << endl;
                return;
            }
            no->carga.posicao = op.posicaoOrigem;
            cout << "Movimentacao de " << op.codigoCarga << " desfeita. Retornou para "
                 << op.posicaoOrigem << "." << endl;
            break;
        }

        case ALOCACAO: {
            // Inversa: remove do pátio e devolve ao início da fila
            NoLista* no = buscarNoPatio(patio, op.codigoCarga);
            if (no == NULL) {
                cout << "Carga " << op.codigoCarga << " nao encontrada no patio." << endl;
                return;
            }

            // Desencadeia o nó da lista duplamente encadeada
            if (no->anterior != NULL) no->anterior->proximo = no->proximo;
            else patio.inicio = no->proximo;
            if (no->proximo != NULL) no->proximo->anterior = no->anterior;
            else patio.fim = no->anterior;

            Carga c = no->carga;
            c.posicao = "Fila";
            delete no;

            // Reinsere no início da fila (era a próxima a sair)
            NoFila* nf = new NoFila(c);
            nf->proximo = fila.inicio;
            fila.inicio = nf;
            if (fila.fim == NULL) fila.fim = nf;

            cout << "Alocacao de " << op.codigoCarga << " desfeita. Carga voltou para a fila." << endl;
            break;
        }

        case EXPEDICAO: {
            // Inversa: reinsere a carga no fim do pátio, na posição de origem
            Carga c = op.carga;
            c.posicao = op.posicaoOrigem;

            NoLista* novo = new NoLista(c);
            novo->anterior = patio.fim;
            if (patio.fim != NULL) patio.fim->proximo = novo;
            else patio.inicio = novo;
            patio.fim = novo;

            cout << "Expedicao de " << op.codigoCarga << " desfeita. Carga voltou para "
                 << op.posicaoOrigem << "." << endl;
            break;
        }
    }

    historico.topo = noTopo->proximo;       // pop
    delete noTopo;                          // desaloca o nó
}

int main() {
    // Ponto de entrada do sistema. O menu principal será implementado aqui.
    return 0;
}
