#include <iostream>
#include <string>
#include <limits>

using namespace std;

// ==========================================
// ESTRUTURA DA CARGA
// ==========================================

struct Carga {
    string codigo;
    string navio;
    string descricao;
    string posicao;
    float peso;
};

// ==========================================
// ESTRUTURA DO NO
// ==========================================

struct No {
    Carga carga;

    No* anterior;
    No* proximo;
};

// ==========================================
// LISTA DUPLAMENTE ENCADEADA
// ==========================================

class ListaDupla {

private:

    No* inicio;
    No* fim;

public:

    // Construtor
    ListaDupla() {
        inicio = NULL;
        fim = NULL;
    }

    // ======================================
    // INSERIR CARGA NO FINAL
    // ======================================

    void inserirFim(Carga novaCarga) {

        No* novo = new No;

        novo->carga = novaCarga;
        novo->anterior = fim;
        novo->proximo = NULL;

        if (inicio == NULL) {
            inicio = novo;
            fim = novo;
        }
        else {
            fim->proximo = novo;
            fim = novo;
        }

        cout << "\nCarga cadastrada com sucesso!\n";
    }

    // ======================================
    // EXIBIR LISTA DO INICIO AO FIM
    // ======================================

    void exibirInicioFim() {

        if (inicio == NULL) {
            cout << "\nPatio vazio!\n";
            return;
        }

        No* atual = inicio;

        cout << "\n===== CARGAS NO PATIO =====\n";

        while (atual != NULL) {

            cout << "\nCodigo: " << atual->carga.codigo;
            cout << "\nNavio: " << atual->carga.navio;
            cout << "\nDescricao: " << atual->carga.descricao;
            cout << "\nPosicao: " << atual->carga.posicao;
            cout << "\nPeso: " << atual->carga.peso << " kg";

            cout << "\n---------------------------";

            atual = atual->proximo;
        }
    }

    // ======================================
    // EXIBIR LISTA DO FIM AO INICIO
    // ======================================

    void exibirFimInicio() {

        if (fim == NULL) {
            cout << "\nPatio vazio!\n";
            return;
        }

        No* atual = fim;

        cout << "\n===== NAVEGACAO REVERSA =====\n";

        while (atual != NULL) {

            cout << "\nCodigo: " << atual->carga.codigo;
            cout << "\nPosicao: " << atual->carga.posicao;

            cout << "\n---------------------------";

            atual = atual->anterior;
        }
    }

    // ======================================
    // BUSCAR CARGA PELO CODIGO
    // ======================================

    void buscar(string codigo) {

        No* atual = inicio;

        while (atual != NULL) {

            if (atual->carga.codigo == codigo) {

                cout << "\nCarga encontrada!\n";

                cout << "\nCodigo: " << atual->carga.codigo;
                cout << "\nNavio: " << atual->carga.navio;
                cout << "\nDescricao: " << atual->carga.descricao;
                cout << "\nPosicao: " << atual->carga.posicao;
                cout << "\nPeso: " << atual->carga.peso << " kg\n";

                return;
            }

            atual = atual->proximo;
        }

        cout << "\nCarga nao encontrada!\n";
    }

    // ======================================
    // REMOVER CARGA PELO CODIGO
    // ======================================

    void removerPorCodigo(string codigo) {

        No* atual = inicio;

        while (atual != NULL && atual->carga.codigo != codigo) {
            atual = atual->proximo;
        }

        if (atual == NULL) {
            cout << "\nCarga nao encontrada!\n";
            return;
        }

        // Se existir um no anterior
        if (atual->anterior != NULL) {
            atual->anterior->proximo = atual->proximo;
        }
        else {
            inicio = atual->proximo;
        }

        // Se existir um proximo no
        if (atual->proximo != NULL) {
            atual->proximo->anterior = atual->anterior;
        }
        else {
            fim = atual->anterior;
        }

        delete atual;

        cout << "\nCarga removida com sucesso!\n";
    }


    // LOCALIZAR NO PELO CODIGO (retorna ponteiro)
    No* localizar(const string& codigo) {

        No* atual = inicio;

        while (atual != NULL && atual->carga.codigo != codigo) {
            atual = atual->proximo;
        }

        return atual;
    }

    // ALTERAR POSICAO DE UMA CARGA
    bool alterarPosicao(const string& codigo, const string& novaPosicao) {

        No* alvo = localizar(codigo);

        if (alvo == NULL) {
            cout << "\nCarga nao encontrada!\n";
            return false;
        }

        alvo->carga.posicao = novaPosicao;
        return true;
    }

    // LIBERAR MEMORIA

    ~ListaDupla() {

        No* atual = inicio;

        while (atual != NULL) {

            No* auxiliar = atual;

            atual = atual->proximo;

            delete auxiliar;
        }

        inicio = NULL;
        fim = NULL;
    }
};

// ALIAS PARA OS NOMES USADOS NAS ASSINATURAS

typedef ListaDupla Lista;

// FILA (placeholder: troque pela sua Fila real)
struct Fila {
};


// OPERACAO REGISTRADA NO HISTORICO

enum TipoOperacao { CADASTRO, REMOCAO, MOVIMENTO };

struct Operacao {
    TipoOperacao tipo;
    string operador;
    Carga carga;            // dados da carga afetada
    string posicaoOrigem;   // usada no MOVIMENTO
};

// PILHA (encadeada) DO HISTORICO
struct NoPilha {
    Operacao op;
    NoPilha* proximo;
};

class Pilha {

private:

    NoPilha* topo;

public:

    Pilha() {
        topo = NULL;
    }

    // Empilhar
    void push(const Operacao& op) {

        NoPilha* novo = new NoPilha;

        novo->op = op;
        novo->proximo = topo;
        topo = novo;
    }

    // Acessar topo (retorna ponteiro; NULL se vazia)
    NoPilha* top() {
        return topo;
    }

    // Desempilhar e desalocar o no
    void pop() {

        if (topo == NULL) {
            return;
        }

        NoPilha* auxiliar = topo;
        topo = topo->proximo;

        delete auxiliar;
    }

    ~Pilha() {
        while (topo != NULL) {
            pop();
        }
    }
};

// REGISTRAR ACAO (push)
void registrarAcao(Pilha& historico, Operacao op) {

    historico.push(op);
}


// DESFAZER ULTIMA ACAO (top + inversa + pop)
void desfazerUltimaAcao(Pilha& historico, Lista& patio, Fila& fila) {

    NoPilha* ultimo = historico.top();

    if (ultimo == NULL) {
        cout << "\nNenhuma acao para desfazer!\n";
        return;
    }

    Operacao& op = ultimo->op;

    switch (op.tipo) {

        case MOVIMENTO:
            // Retorna a carga para a posicao de origem
            patio.alterarPosicao(op.carga.codigo, op.posicaoOrigem);
            cout << "\nMovimento da carga " << op.carga.codigo
                 << " (operador " << op.operador << ") desfeito."
                 << "\nCarga retornou para: " << op.posicaoOrigem << "\n";
            break;

        case CADASTRO:
            // Inversa do cadastro: remover a carga
            patio.removerPorCodigo(op.carga.codigo);
            break;

        case REMOCAO:
            // Inversa da remocao: reinserir a carga
            patio.inserirFim(op.carga);
            break;
    }

    // Remove o registro da pilha e desaloca o no
    historico.pop();
}

// ==========================================
// PROGRAMA PRINCIPAL
// ==========================================

int main() {

    ListaDupla patio;

    Pilha historico;
    Fila fila;

    int opcao;
    string codigo;

    Carga carga;

    do {

        cout << "\n\n================================";
        cout << "\n       TERMINAL PORTO SUL";
        cout << "\n     PATIO DE ARMAZENAMENTO";
        cout << "\n================================";

        cout << "\n1 - Cadastrar carga";
        cout << "\n2 - Exibir cargas";
        cout << "\n3 - Buscar carga";
        cout << "\n4 - Remover carga";
        cout << "\n5 - Exibir cargas em ordem reversa";
        cout << "\n6 - Movimentar carga";
        cout << "\n7 - Desfazer ultima acao";
        cout << "\n0 - Encerrar programa";

        cout << "\n\nEscolha uma opcao: ";
        cin >> opcao;

        switch (opcao) {

            // CADASTRAR CARGA
            case 1:
            {
                Operacao op;

                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                cout << "\nCodigo da carga: ";
                getline(cin, carga.codigo);

                cout << "Nome do navio: ";
                getline(cin, carga.navio);

                cout << "Descricao da carga: ";
                getline(cin, carga.descricao);

                cout << "Posicao no patio: ";
                getline(cin, carga.posicao);

                cout << "Peso da carga (kg): ";
                cin >> carga.peso;

                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Operador: ";
                getline(cin, op.operador);

                patio.inserirFim(carga);

                op.tipo = CADASTRO;
                op.carga = carga;
                registrarAcao(historico, op);

                break;
            }

            // EXIBIR CARGAS
            case 2:
                patio.exibirInicioFim();
                break;

            // BUSCAR CARGA
            case 3:
                cout << "\nDigite o codigo da carga: ";
                cin >> codigo;

                patio.buscar(codigo);
                break;

            // REMOVER CARGA
            case 4:
            {
                cout << "\nDigite o codigo da carga: ";
                cin >> codigo;

                No* alvo = patio.localizar(codigo);

                if (alvo != NULL) {
                    Operacao op;
                    op.tipo = REMOCAO;
                    op.carga = alvo->carga;            // guarda antes de apagar

                    cout << "Operador: ";
                    cin >> op.operador;

                    patio.removerPorCodigo(codigo);
                    registrarAcao(historico, op);
                }
                else {
                    patio.removerPorCodigo(codigo);    // exibe "nao encontrada"
                }
                break;
            }

            // EXIBIR EM ORDEM REVERSA
            case 5:
                patio.exibirFimInicio();
                break;

            // MOVIMENTAR CARGA
            case 6:
            {
                string novaPosicao;

                cout << "\nDigite o codigo da carga: ";
                cin >> codigo;

                No* alvo = patio.localizar(codigo);

                if (alvo == NULL) {
                    cout << "\nCarga nao encontrada!\n";
                    break;
                }
                Operacao op;
                op.tipo = MOVIMENTO;
                op.carga = alvo->carga;
                op.posicaoOrigem = alvo->carga.posicao;   // guarda a origem

                cout << "Operador: ";
                cin >> op.operador;

                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Nova posicao: ";
                getline(cin, novaPosicao);

                patio.alterarPosicao(codigo, novaPosicao);
                registrarAcao(historico, op);

                cout << "\nCarga movimentada com sucesso!\n";
                break;
            }

			// DESFAZER ULTIMA AÇÃO
            case 7:
                desfazerUltimaAcao(historico, patio, fila);
                break;

            // ENCERRAR
            case 0:
                cout << "\nEncerrando o sistema...\n";
                break;

            default:
                cout << "\nOpcao invalida!\n";
        }
    } while (opcao != 0);
    return 0;
}
