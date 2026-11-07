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

// Registro de operação para o Histórico (Pilha Undo)
enum TipoOperacao { ALOCACAO, MOVIMENTACAO, EXPEDICAO };

struct Operacao {
    TipoOperacao tipoOp;
    string codigoCarga;
    string posicaoOrigem;
    string posicaoDestino;
};

// Nó para a Pilha de Histórico
struct NoPilha {
    Operacao op;
    NoPilha* proximo;

    NoPilha(Operacao o) : op(o), proximo(NULL) {}
};

int main() {
    // Ponto de entrada do sistema. O menu principal será implementado aqui.
    return 0;
}
