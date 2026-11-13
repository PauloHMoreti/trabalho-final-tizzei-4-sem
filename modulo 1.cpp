
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

    // ======================================
    // LIBERAR MEMORIA
    // ======================================

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

// ==========================================
// PROGRAMA PRINCIPAL
// ==========================================

int main() {

    ListaDupla patio;

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
        cout << "\n0 - Encerrar programa";

        cout << "\n\nEscolha uma opcao: ";
        cin >> opcao;

        switch (opcao) {

            // CADASTRAR CARGA
            case 1:

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

                patio.inserirFim(carga);

                break;

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

                cout << "\nDigite o codigo da carga: ";
                cin >> codigo;

                patio.removerPorCodigo(codigo);

                break;

            // EXIBIR EM ORDEM REVERSA
            case 5:

                patio.exibirFimInicio();

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
