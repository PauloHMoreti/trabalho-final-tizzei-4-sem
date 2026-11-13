// ============================================================================
// Módulo 4 — Relatórios Estatísticos e Consultas Gerenciais
// Responsável: Luis
//
// Regra de ouro deste módulo: SÓ LEITURA. Nenhuma função aqui altera
// cargas, ponteiros ou remove elementos. Por isso tudo é recebido por
// referência constante (const &).
// ============================================================================

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iomanip>

using namespace std;

// ----------------------------------------------------------------------------
// Estruturas copiadas do codigo-base.cpp (NÃO alterar nomes dos campos)
// ----------------------------------------------------------------------------

enum TipoCarga { CONTEINER, GRANEL, REFRIGERADA, PERIGOSA };

struct Carga {
    string codigo;
    TipoCarga tipo;
    float peso;
    string cliente;
    string destino;
    string posicao;
    bool necessitaRefrigeracao;
    bool ehPerigosa;
};

struct NoLista {
    Carga carga;
    NoLista* anterior;
    NoLista* proximo;

    NoLista(Carga c) : carga(c), anterior(NULL), proximo(NULL) {}
};

struct ListaPatio {
    NoLista* inicio;
    NoLista* fim;

    ListaPatio() : inicio(NULL), fim(NULL) {}
};

struct NoFila {
    Carga carga;
    NoFila* proximo;

    NoFila(Carga c) : carga(c), proximo(NULL) {}
};

// PROVISÓRIO: o codigo-base ainda não define a struct Fila (Módulo 1 / Pedro).
// Seguimos a especificação: ponteiros de início e fim. Confirmar com o Pedro.
struct Fila {
    NoFila* inicio;
    NoFila* fim;

    Fila() : inicio(NULL), fim(NULL) {}
};

// ============================================================================
// FUNÇÕES DO MÓDULO 4  — é aqui que a gente trabalha
// ============================================================================

// Auxiliar: converte a categoria (enum) em texto para a tabela.
string tipoParaTexto(TipoCarga tipo) {
    switch (tipo) {
        case CONTEINER:   return "Conteiner";
        case GRANEL:      return "Granel";
        case REFRIGERADA: return "Refrigerada";
        case PERIGOSA:    return "Perigosa";
    }
    return "Desconhecido";
}

// Auxiliar: monta o texto dos requisitos especiais da carga.
string requisitosEspeciais(const Carga& carga) {
    if (carga.necessitaRefrigeracao && carga.ehPerigosa) {
        return "Refrigeracao + Perigosa";
    }
    if (carga.necessitaRefrigeracao) {
        return "Refrigeracao";
    }
    if (carga.ehPerigosa) {
        return "Perigosa";
    }
    return "Nenhum";
}

// Relatório de Contêineres Armazenados
// Percorre a lista do pátio a partir do nó inicial, conta os nós com
// tipo == CONTEINER e, ao final, exibe o total consolidado.
void relatorioConteineres(const ListaPatio& patio) {
    int totalConteineres = 0;

    NoLista* atual = patio.inicio;
    while (atual != NULL) {
        if (atual->carga.tipo == CONTEINER) {
            totalConteineres++;
        }
        atual = atual->proximo;
    }

    cout << "Total de conteineres armazenados no patio: "
         << totalConteineres << endl;
}

// Relatório de Peso de Cargas de Granel
// Percorre a lista do pátio filtrando tipo == GRANEL, acumula o peso em
// uma variável float e apresenta o total em toneladas.
void relatorioPesoGranel(const ListaPatio& patio) {
    float pesoTotal = 0.0f;

    NoLista* atual = patio.inicio;
    while (atual != NULL) {
        if (atual->carga.tipo == GRANEL) {
            pesoTotal += atual->carga.peso;
        }
        atual = atual->proximo;
    }

    cout << "Peso total de granel armazenado no patio: "
         << fixed << setprecision(2) << pesoTotal << " t" << endl;
}

// Relatório de Cargas Pendentes na Fila
// Percorre a fila do início ao fim SEM remover nenhum elemento e imprime
// em tabela: código, categoria, cliente e requisitos especiais.
void relatorioFilaPendentes(const Fila& fila) {
    if (fila.inicio == NULL) {
        cout << "Nenhuma carga pendente na fila de espera." << endl;
        return;
    }

    cout << left
         << setw(10) << "Codigo"
         << setw(14) << "Categoria"
         << setw(16) << "Cliente"
         << "Requisitos especiais" << endl;
    cout << string(64, '-') << endl;

    int totalPendentes = 0;
    NoFila* atual = fila.inicio;
    while (atual != NULL) {
        cout << left
             << setw(10) << atual->carga.codigo
             << setw(14) << tipoParaTexto(atual->carga.tipo)
             << setw(16) << atual->carga.cliente
             << requisitosEspeciais(atual->carga) << endl;
        totalPendentes++;
        atual = atual->proximo;
    }

    cout << string(64, '-') << endl;
    cout << "Cargas aguardando vaga: " << totalPendentes << endl;
}

// ============================================================================
// ÁREA DE TESTE — só para testar o módulo sozinho.
// Na integração final, estas funções e o main abaixo NÃO vão junto.
// ============================================================================

Carga novaCarga(string codigo, TipoCarga tipo, float peso, string cliente,
                string destino, string posicao, bool refrigerada, bool perigosa) {
    Carga c;
    c.codigo = codigo;
    c.tipo = tipo;
    c.peso = peso;
    c.cliente = cliente;
    c.destino = destino;
    c.posicao = posicao;
    c.necessitaRefrigeracao = refrigerada;
    c.ehPerigosa = perigosa;
    return c;
}

// Insere no fim da lista (versão simplificada, sem as validações do Módulo 2).
void testeInserirNoPatio(ListaPatio& patio, Carga c) {
    NoLista* novo = new NoLista(c);
    if (patio.inicio == NULL) {
        patio.inicio = novo;
        patio.fim = novo;
    } else {
        novo->anterior = patio.fim;
        patio.fim->proximo = novo;
        patio.fim = novo;
    }
}

// Enfileira no fim (versão simplificada do Módulo 1).
void testeEnfileirar(Fila& fila, Carga c) {
    NoFila* novo = new NoFila(c);
    if (fila.inicio == NULL) {
        fila.inicio = novo;
        fila.fim = novo;
    } else {
        fila.fim->proximo = novo;
        fila.fim = novo;
    }
}

void testeLiberarMemoria(ListaPatio& patio, Fila& fila) {
    while (patio.inicio != NULL) {
        NoLista* aux = patio.inicio;
        patio.inicio = patio.inicio->proximo;
        delete aux;
    }
    patio.fim = NULL;

    while (fila.inicio != NULL) {
        NoFila* aux = fila.inicio;
        fila.inicio = fila.inicio->proximo;
        delete aux;
    }
    fila.fim = NULL;
}

int main() {
    ListaPatio patio;
    Fila fila;

    // ---- Cenário 1: estruturas vazias (não pode travar) ----
    cout << "===== CENARIO 1: patio e fila vazios =====" << endl;
    relatorioConteineres(patio);
    cout << "  (esperado: 0)" << endl;
    relatorioPesoGranel(patio);
    cout << "  (esperado: 0.00 t)" << endl;
    relatorioFilaPendentes(fila);
    cout << "  (esperado: aviso de nenhuma carga pendente)" << endl << endl;

    // ---- Cenário 2: dados de exemplo ----
    testeInserirNoPatio(patio, novaCarga("CRG0001", CONTEINER,   22.0f,  "Agro Sul",     "Santos",     "R-01-01", true,  false));
    testeInserirNoPatio(patio, novaCarga("CRG0002", GRANEL,    1500.0f, "Graos Brasil", "Paranagua",  "G-03-02", false, false));
    testeInserirNoPatio(patio, novaCarga("CRG0003", CONTEINER,   18.5f,  "Textil Norte", "Recife",     "B-02-01", false, false));
    testeInserirNoPatio(patio, novaCarga("CRG0004", GRANEL,     850.5f, "Minerios SA",  "Vitoria",    "G-01-04", false, false));
    testeInserirNoPatio(patio, novaCarga("CRG0005", PERIGOSA,    12.0f,  "Quimica Leste","Rio Grande", "X-01-01", false, true));

    testeEnfileirar(fila, novaCarga("CRG0006", REFRIGERADA,  9.0f,  "Frigo Centro", "Itajai",   "Fila", true,  false));
    testeEnfileirar(fila, novaCarga("CRG0007", CONTEINER,   20.0f,  "Moveis Sul",   "Salvador", "Fila", false, false));
    testeEnfileirar(fila, novaCarga("CRG0008", PERIGOSA,     5.5f,  "Gas Norte",    "Manaus",   "Fila", false, true));

    cout << "===== CENARIO 2: dados de exemplo =====" << endl;
    relatorioConteineres(patio);
    cout << "  (esperado: 2)" << endl;
    relatorioPesoGranel(patio);
    cout << "  (esperado: 2350.50 t)" << endl;
    relatorioFilaPendentes(fila);
    cout << "  (esperado: tabela com CRG0006, CRG0007, CRG0008 e 3 aguardando)" << endl << endl;

    // Os relatórios são só leitura: rodar de novo tem que dar o mesmo resultado
    // (prova de que a fila não foi esvaziada pelo relatório).
    cout << "===== CENARIO 3: relatorios nao alteram as estruturas =====" << endl;
    relatorioFilaPendentes(fila);
    cout << "  (esperado: mesma tabela, ainda 3 aguardando)" << endl;

    testeLiberarMemoria(patio, fila);
    return 0;
}
