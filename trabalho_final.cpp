// ============================================================================
// TRABALHO FINAL - Sistema de Gerenciamento de Cargas (Terminal Porto Sul)
//
// Unificacao dos modulos:
//   Modulo 1 - Fila de Espera de Cargas Pendentes (Pedro)
//   Modulo 2 - Patio de Armazenamento - Lista Duplamente Encadeada (Paulo)
//   Modulo 3 - Historico de Operacoes e Desfazer - Pilha (Angelo)
//   Modulo 4 - Relatorios Estatisticos e Consultas Gerenciais (Luis)
// ============================================================================

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iomanip>

using namespace std;

// ============================================================================
// ESTRUTURAS DE DADOS (codigo-base)
// ============================================================================

// Tipos de Carga no Terminal
enum TipoCarga { CONTEINER, GRANEL, REFRIGERADA, PERIGOSA };

// Registro principal de informacoes da carga
struct Carga {
    string codigo;      // Ex: "CRG0012", "CRG0005"
    TipoCarga tipo;     // Categoria da carga
    float peso;         // Peso em toneladas
    string cliente;     // Nome do proprietario/cliente
    string destino;     // Cidade ou porto de destino
    string posicao;     // Posicao no patio (Ex: "B-02-01" ou "Fila")
    bool necessitaRefrigeracao;
    bool ehPerigosa;
};

// No para a Lista Duplamente Encadeada (Patio de Armazenamento)
struct NoLista {
    Carga carga;
    NoLista* anterior;
    NoLista* proximo;

    NoLista(Carga c) : carga(c), anterior(NULL), proximo(NULL) {}
};

// Estrutura que representa o Patio (guarda os ponteiros do inicio e do fim)
struct ListaPatio {
    NoLista* inicio;
    NoLista* fim;

    ListaPatio() : inicio(NULL), fim(NULL) {}
};

// No para a Fila de Espera (Cargas Pendentes)
struct NoFila {
    Carga carga;
    NoFila* proximo;

    NoFila(Carga c) : carga(c), proximo(NULL) {}
};

// Estrutura da Fila (ponteiros de inicio e fim)
struct Fila {
    NoFila* inicio;
    NoFila* fim;

    Fila() : inicio(NULL), fim(NULL) {}
};

// Registro de operacao para o Historico (Pilha Undo)
enum TipoOperacao { ALOCACAO, MOVIMENTACAO, EXPEDICAO };

struct Operacao {
    TipoOperacao tipoOp;
    string codigoCarga;
    string posicaoOrigem;
    string posicaoDestino;
    Carga carga;            // copia dos dados, necessaria para desfazer uma EXPEDICAO
};

// No para a Pilha de Historico
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

// ============================================================================
// MODULO 1 - FILA DE ESPERA DE CARGAS PENDENTES (FIFO)
// ============================================================================

// Configura os ponteiros de inicio e fim da fila como nulos
void inicializarFila(Fila& fila) {
    fila.inicio = NULL;
    fila.fim = NULL;
}

// Verifica se existem cargas aguardando triagem
bool filaVazia(const Fila& fila) {
    return fila.inicio == NULL;
}

// Insere uma nova carga no final da fila de espera
void enfileirar(Fila& fila, Carga c) {
    c.posicao = "Fila";

    NoFila* novo = new NoFila(c);

    if (fila.inicio == NULL) {
        // Fila vazia: o novo no e o inicio e o fim
        fila.inicio = novo;
        fila.fim = novo;
    } else {
        // Conecta o antigo fim ao novo no e atualiza o fim
        fila.fim->proximo = novo;
        fila.fim = novo;
    }
}

// Remove a carga que esta ha mais tempo aguardando (inicio da fila)
Carga desenfileirar(Fila& fila) {
    Carga removida;

    if (filaVazia(fila)) {
        cout << "[ERRO] A fila de espera esta vazia!" << endl;
        removida.codigo = "";
        removida.tipo = CONTEINER;
        removida.peso = 0;
        removida.necessitaRefrigeracao = false;
        removida.ehPerigosa = false;
        return removida;
    }

    NoFila* temp = fila.inicio;
    removida = temp->carga;

    fila.inicio = fila.inicio->proximo;
    delete temp;

    // Se a fila ficou vazia, o fim tambem passa a ser nulo
    if (fila.inicio == NULL) {
        fila.fim = NULL;
    }

    return removida;
}

// ============================================================================
// MODULO 2 - PATIO DE ARMAZENAMENTO (LISTA DUPLAMENTE ENCADEADA)
// ============================================================================

// Retorna o ponteiro para o no de carga encontrada ou NULL se nao existir
NoLista* buscarPorCodigo(const ListaPatio& patio, string codigo){
	NoLista* atual = patio.inicio;
	while(atual != NULL){
		if(atual->carga.codigo == codigo){
			return atual; //Encontrou a carga
		}
		atual = atual->proximo;
	}
	return NULL; //Carga nao encontrada
}

bool posicaoEstaOcupada(const ListaPatio& patio, string posicao){
	NoLista* atual = patio.inicio;
	while(atual != NULL){
		if(atual->carga.posicao == posicao){
			return true; //Posicao ja ocupada
		}
		atual = atual->proximo;
	}
	return false; //Posicao livre
}

// Blocos refrigerados sao as posicoes que comecam com "R-" (Ex: R-01-01)
bool posicaoRefrigerada(string posicao){
	return posicao.size() >= 2 && posicao[0] == 'R' && posicao[1] == '-';
}

// A area isolada (cargas perigosas) sao as posicoes que comecam com "X-" (Ex: X-01-01)
bool posicaoAreaIsolada(string posicao){
	return posicao.size() >= 2 && posicao[0] == 'X' && posicao[1] == '-';
}

bool alocarNoPatio(ListaPatio& patio, Carga c, string posicaoDesejada){
	//1 - Valida se a posicao ja esta ocupada
	if(posicaoEstaOcupada(patio,posicaoDesejada)){
		cout<<"[ERRO] A posicao "<<posicaoDesejada<<" ja esta ocupada!"<<endl;
		return false;
	}

	//2 - Carga refrigerada so pode ir para um bloco refrigerado
	if(c.necessitaRefrigeracao && !posicaoRefrigerada(posicaoDesejada)){
		cout<<"[ERRO] A carga "<<c.codigo<<" necessita de refrigeracao e a posicao "<<posicaoDesejada<<" nao pertence a um bloco refrigerado (R-)!"<<endl;
		return false;
	}

	//3 - Carga perigosa so pode ir para a area isolada
	if(c.ehPerigosa && !posicaoAreaIsolada(posicaoDesejada)){
		cout<<"[ERRO] A carga "<<c.codigo<<" e perigosa e a posicao "<<posicaoDesejada<<" nao esta na area isolada (X-)!"<<endl;
		return false;
	}

	//4 - Atualiza a posicao da carga
	c.posicao = posicaoDesejada;

	//5 - Aloca dinamicamente o novo no
	NoLista* novo = new NoLista(c);

	//6 - Insercao no fim da Lista Duplamente Encadeada
	if(patio.inicio == NULL){
		//Lista estava vazia
		patio.inicio = novo;
		patio.fim = novo;
	} else {
		//Conecta o no antigo do fim ao novo no e vice-versa
		novo->anterior = patio.fim;
		patio.fim->proximo = novo;
		patio.fim = novo;
	}

	cout<<"[SUCESSO] Carga "<<c.codigo<<" alocada na posicao "<<posicaoDesejada<<" com sucesso"<<endl;
	return true;
}

bool expedirCarga(ListaPatio& patio, string codigo){
	NoLista* atual = buscarPorCodigo(patio, codigo);

	if(atual == NULL){
		cout<<"[ERRO] Carga com o codigo "<<codigo<<" nao encontrada no patio!"<<endl;
		return false;
	}

	//Reajuste do ponteiro 'proximo' do no anterior
	if(atual->anterior != NULL){
		atual->anterior->proximo = atual->proximo;
	} else {
		patio.inicio = atual->proximo; //Era o primeiro no da lista
	}

	//Reajuste do ponteiro 'anterior' do no seguinte
	if(atual->proximo != NULL){
		atual->proximo->anterior = atual->anterior;
	} else {
		patio.fim = atual->anterior; //Era o ultimo no da lista
	}

	string posLiberada = atual->carga.posicao;

	//Libera a memoria do no removido
	delete atual;

	cout<<"[EXPEDICAO] Carga "<<codigo<<" expedida com sucesso! A posicao "<<posLiberada<<" agora esta livre"<<endl;
	return true;
}

bool atualizarLocalizacao(ListaPatio& patio, string codigo, string novaPosicao){
	NoLista* noCarga = buscarPorCodigo(patio, codigo);

	if(noCarga == NULL){
		cout<<"[ERRO] Carga "<<codigo<<" nao encontrada para movimentacao!"<<endl;
		return false;
	}

	if(posicaoEstaOcupada(patio, novaPosicao)){
		cout<<"[ERRO] A nova posicao "<<novaPosicao<<" ja esta ocupada!"<<endl;
		return false;
	}

	string posAntiga = noCarga->carga.posicao;
	noCarga->carga.posicao = novaPosicao;

	cout<<"[MOVIMENTACAO] Carga "<<codigo<<" movida de "<<posAntiga<<" para "<<novaPosicao<<endl;
	return true;
}

void exibirPatio(const ListaPatio& patio){
	if(patio.inicio == NULL){
		cout<<"\n---Patio de armazenamento vazio---"<<endl;
		return;
	}

	cout<<"\n==============CARGAS NO PATIO==========="<<endl;
	NoLista* atual = patio.inicio;
	while(atual != NULL){
		cout<<"Codigo: "<<left<<setw(10)<<atual->carga.codigo<<
		" | Posicao: "<<setw(10)<<atual->carga.posicao<<
		" | Peso: "<<setw(5)<<atual->carga.peso<<" t "<<
		" | Cliente: "<<atual->carga.cliente<<endl;
		atual = atual->proximo;
	}
	cout<<"==========================================="<<endl;
}

// ============================================================================
// MODULO 3 - HISTORICO DE OPERACOES E MECANISMO DE DESFAZER (PILHA LIFO)
// ============================================================================

// Busca uma carga no patio pelo codigo e retorna o ponteiro do no (ou NULL)
NoLista* buscarNoPatio(ListaPatio& patio, const string& codigo) {
    NoLista* atual = patio.inicio;
    while (atual != NULL) {
        if (atual->carga.codigo == codigo) return atual;
        atual = atual->proximo;
    }
    return NULL;
}

// Empilha (push) o registro da operacao no topo do historico
void registrarAcao(Pilha& historico, Operacao op) {
    NoPilha* novo = new NoPilha(op);
    novo->proximo = historico.topo;
    historico.topo = novo;
}

// Desfaz a ultima operacao registrada: le o topo, executa a acao inversa,
// remove o no do topo e desaloca a memoria
void desfazerUltimaAcao(Pilha& historico, ListaPatio& patio, Fila& fila) {
    if (historico.topo == NULL) {
        cout << "Historico vazio. Nada a desfazer." << endl;
        return;
    }

    NoPilha* noTopo = historico.topo;       // top
    Operacao op = noTopo->op;

    switch (op.tipoOp) {
        case MOVIMENTACAO: {
            // Inversa: devolve a carga a posicao de origem
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
            // Inversa: remove do patio e devolve ao inicio da fila
            NoLista* no = buscarNoPatio(patio, op.codigoCarga);
            if (no == NULL) {
                cout << "Carga " << op.codigoCarga << " nao encontrada no patio." << endl;
                return;
            }

            // Desencadeia o no da lista duplamente encadeada
            if (no->anterior != NULL) no->anterior->proximo = no->proximo;
            else patio.inicio = no->proximo;
            if (no->proximo != NULL) no->proximo->anterior = no->anterior;
            else patio.fim = no->anterior;

            Carga c = no->carga;
            c.posicao = "Fila";
            delete no;

            // Reinsere no inicio da fila (era a proxima a sair)
            NoFila* nf = new NoFila(c);
            nf->proximo = fila.inicio;
            fila.inicio = nf;
            if (fila.fim == NULL) fila.fim = nf;

            cout << "Alocacao de " << op.codigoCarga << " desfeita. Carga voltou para a fila." << endl;
            break;
        }

        case EXPEDICAO: {
            // Inversa: reinsere a carga no fim do patio, na posicao de origem
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
    delete noTopo;                          // desaloca o no
}

// ============================================================================
// MODULO 4 - RELATORIOS ESTATISTICOS E CONSULTAS GERENCIAIS
// (somente leitura: nada aqui altera cargas ou ponteiros)
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

// Relatorio de Conteineres Armazenados
// Percorre a lista do patio a partir do no inicial, conta os nos com
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

// Relatorio de Peso de Cargas de Granel
// Percorre a lista do patio filtrando tipo == GRANEL, acumula o peso em
// uma variavel float e apresenta o total em toneladas.
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

// Relatorio de Cargas Pendentes na Fila
// Percorre a fila do inicio ao fim SEM remover nenhum elemento e imprime
// em tabela: codigo, categoria, cliente e requisitos especiais.
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
// INTEGRACAO - liga os modulos e registra cada acao no historico (Modulo 3)
// ============================================================================

string tipoOperacaoParaTexto(TipoOperacao tipo) {
    switch (tipo) {
        case ALOCACAO:     return "Alocacao";
        case MOVIMENTACAO: return "Movimentacao";
        case EXPEDICAO:    return "Expedicao";
    }
    return "Desconhecida";
}

// Tenta alocar a primeira carga da fila no patio. A carga so sai da fila
// se a alocacao for aprovada; se for bloqueada, continua aguardando.
bool alocarProximaDaFila(Fila& fila, ListaPatio& patio, Pilha& historico, string posicao) {
    if (filaVazia(fila)) {
        cout << "[ERRO] Nao ha cargas na fila de espera!" << endl;
        return false;
    }

    Carga c = fila.inicio->carga;

    if (!alocarNoPatio(patio, c, posicao)) {
        cout << "A carga " << c.codigo << " continua aguardando na fila." << endl;
        return false;
    }

    desenfileirar(fila);

    Operacao op;
    op.tipoOp = ALOCACAO;
    op.codigoCarga = c.codigo;
    op.posicaoOrigem = "Fila";
    op.posicaoDestino = posicao;
    op.carga = c;
    registrarAcao(historico, op);
    return true;
}

// Move a carga de posicao e registra a movimentacao no historico
bool movimentarCarga(ListaPatio& patio, Pilha& historico, string codigo, string novaPosicao) {
    NoLista* no = buscarPorCodigo(patio, codigo);
    string posicaoAntiga = "";
    if (no != NULL) {
        posicaoAntiga = no->carga.posicao;
    }

    if (!atualizarLocalizacao(patio, codigo, novaPosicao)) {
        return false;
    }

    Operacao op;
    op.tipoOp = MOVIMENTACAO;
    op.codigoCarga = codigo;
    op.posicaoOrigem = posicaoAntiga;
    op.posicaoDestino = novaPosicao;
    op.carga = no->carga;
    registrarAcao(historico, op);
    return true;
}

// Expede a carga e registra a expedicao (com a copia da carga) no historico
bool expedirComHistorico(ListaPatio& patio, Pilha& historico, string codigo) {
    NoLista* no = buscarPorCodigo(patio, codigo);
    if (no == NULL) {
        return expedirCarga(patio, codigo);     // exibe a mensagem de erro
    }

    Carga copia = no->carga;                    // guarda antes do delete

    if (!expedirCarga(patio, codigo)) {
        return false;
    }

    Operacao op;
    op.tipoOp = EXPEDICAO;
    op.codigoCarga = codigo;
    op.posicaoOrigem = copia.posicao;
    op.posicaoDestino = "Caminhao";
    op.carga = copia;
    registrarAcao(historico, op);
    return true;
}

// Mostra o historico do topo (mais recente) para a base
void exibirHistorico(const Pilha& historico) {
    if (historico.topo == NULL) {
        cout << "Historico vazio." << endl;
        return;
    }

    cout << "\n========== HISTORICO (topo primeiro) ==========" << endl;
    NoPilha* atual = historico.topo;
    while (atual != NULL) {
        cout << left << setw(14) << tipoOperacaoParaTexto(atual->op.tipoOp)
             << "Carga: " << setw(10) << atual->op.codigoCarga
             << "De: " << setw(10) << atual->op.posicaoOrigem
             << "Para: " << atual->op.posicaoDestino << endl;
        atual = atual->proximo;
    }
    cout << "===============================================" << endl;
}

// Exibe os dados completos de uma carga do patio
void consultarCarga(const ListaPatio& patio, string codigo) {
    NoLista* no = buscarPorCodigo(patio, codigo);

    if (no == NULL) {
        cout << "Carga " << codigo << " nao esta no patio." << endl;
        return;
    }

    cout << "\nCodigo:    " << no->carga.codigo;
    cout << "\nCategoria: " << tipoParaTexto(no->carga.tipo);
    cout << "\nPeso:      " << no->carga.peso << " t";
    cout << "\nCliente:   " << no->carga.cliente;
    cout << "\nDestino:   " << no->carga.destino;
    cout << "\nPosicao:   " << no->carga.posicao;
    cout << "\nRequisitos especiais: " << requisitosEspeciais(no->carga) << endl;
}

void emitirRelatorios(const ListaPatio& patio, const Fila& fila) {
    cout << "\n========== RELATORIOS GERENCIAIS ==========" << endl;
    relatorioConteineres(patio);
    relatorioPesoGranel(patio);
    cout << endl;
    relatorioFilaPendentes(fila);
}

// Libera toda a memoria alocada nas tres estruturas
void liberarMemoria(ListaPatio& patio, Fila& fila, Pilha& historico) {
    while (patio.inicio != NULL) {
        NoLista* aux = patio.inicio;
        patio.inicio = patio.inicio->proximo;
        delete aux;
    }
    patio.fim = NULL;

    while (!filaVazia(fila)) {
        desenfileirar(fila);
    }

    while (historico.topo != NULL) {
        NoPilha* aux = historico.topo;
        historico.topo = historico.topo->proximo;
        delete aux;
    }
}

// Le os dados de uma nova carga pelo teclado
Carga lerCarga() {
    Carga c;
    int tipo;
    char resposta;

    cout << "\nCodigo da carga: ";
    cin >> c.codigo;

    do {
        cout << "Tipo (1-Conteiner 2-Granel 3-Refrigerada 4-Perigosa): ";
        cin >> tipo;
    } while (tipo < 1 || tipo > 4);
    c.tipo = (TipoCarga)(tipo - 1);

    cout << "Peso (toneladas): ";
    cin >> c.peso;

    cin.ignore();
    cout << "Cliente: ";
    getline(cin, c.cliente);

    cout << "Destino: ";
    getline(cin, c.destino);

    cout << "Necessita refrigeracao? (s/n): ";
    cin >> resposta;
    c.necessitaRefrigeracao = (resposta == 's' || resposta == 'S');

    cout << "E perigosa? (s/n): ";
    cin >> resposta;
    c.ehPerigosa = (resposta == 's' || resposta == 'S');

    c.posicao = "Fila";
    return c;
}

// ============================================================================
// ROTEIRO INTEGRADO DE TESTES (secao 3 do modulos.md)
// Usa estruturas proprias, sem mexer nos dados do menu.
// ============================================================================

void roteiroDeTestes() {
    ListaPatio patio;
    Fila fila;
    Pilha historico;

    inicializarFila(fila);

    cout << "\n##### ETAPA 1: TRIAGEM - 5 cargas na fila de espera #####" << endl;
    Carga c1 = {"CRG0001", CONTEINER,   22.0f,   "Agro Sul",     "Santos",    "", true,  false};
    Carga c2 = {"CRG0002", GRANEL,      1500.0f, "Graos Brasil", "Paranagua", "", false, false};
    Carga c3 = {"CRG0003", CONTEINER,   18.5f,   "Textil Norte", "Recife",    "", false, false};
    Carga c4 = {"CRG0004", GRANEL,      850.5f,  "Minerios SA",  "Vitoria",   "", false, false};
    Carga c5 = {"CRG0005", REFRIGERADA, 9.0f,    "Frigo Centro", "Itajai",    "", true,  false};
    enfileirar(fila, c1);
    enfileirar(fila, c2);
    enfileirar(fila, c3);
    enfileirar(fila, c4);
    enfileirar(fila, c5);
    relatorioFilaPendentes(fila);

    cout << "\n##### ETAPA 2: ALOCACAO no patio #####" << endl;
    cout << "-> CRG0001 (refrigerada) em posicao comum (esperado: bloqueio)" << endl;
    alocarProximaDaFila(fila, patio, historico, "B-01-01");
    cout << "-> CRG0001 em bloco refrigerado (esperado: sucesso)" << endl;
    alocarProximaDaFila(fila, patio, historico, "R-01-01");
    alocarProximaDaFila(fila, patio, historico, "G-03-02");
    alocarProximaDaFila(fila, patio, historico, "B-02-01");
    cout << "-> CRG0004 em posicao ja ocupada (esperado: bloqueio)" << endl;
    alocarProximaDaFila(fila, patio, historico, "B-02-01");
    alocarProximaDaFila(fila, patio, historico, "G-01-04");
    cout << "-> CRG0005 (refrigerada) em posicao comum (esperado: bloqueio, continua na fila)" << endl;
    alocarProximaDaFila(fila, patio, historico, "B-05-05");
    exibirPatio(patio);

    cout << "\n##### ETAPA 3: MOVIMENTACAO e HISTORICO #####" << endl;
    movimentarCarga(patio, historico, "CRG0003", "B-07-03");
    exibirHistorico(historico);
    cout << "(esperado: Movimentacao de CRG0003 no topo da pilha)" << endl;

    cout << "\n##### ETAPA 4: REVERSAO (UNDO) #####" << endl;
    desfazerUltimaAcao(historico, patio, fila);
    consultarCarga(patio, "CRG0003");
    cout << "(esperado: CRG0003 de volta em B-02-01)" << endl;

    cout << "\n##### ETAPA 5: EXPEDICAO #####" << endl;
    expedirComHistorico(patio, historico, "CRG0002");
    cout << "Posicao G-03-02 ocupada? " << (posicaoEstaOcupada(patio, "G-03-02") ? "sim" : "nao")
         << " (esperado: nao)" << endl;
    exibirPatio(patio);

    cout << "\n##### ETAPA 6: RELATORIOS #####" << endl;
    emitirRelatorios(patio, fila);
    cout << "(esperado: 2 conteineres, 850.50 t de granel, 1 carga pendente - CRG0005)" << endl;

    liberarMemoria(patio, fila, historico);
}

// ============================================================================
// PROGRAMA PRINCIPAL - MENU
// ============================================================================

int main() {
    ListaPatio patio;
    Fila fila;
    Pilha historico;

    int opcao;
    string codigo, posicao;

    inicializarFila(fila);

    do {
        cout << "\n\n================================";
        cout << "\n       TERMINAL PORTO SUL";
        cout << "\n================================";
        cout << "\n1 - Cadastrar carga na fila de espera";
        cout << "\n2 - Alocar proxima carga da fila no patio";
        cout << "\n3 - Movimentar carga no patio";
        cout << "\n4 - Expedir carga (saida para caminhao)";
        cout << "\n5 - Desfazer ultima acao";
        cout << "\n6 - Consultar carga por codigo";
        cout << "\n7 - Exibir patio";
        cout << "\n8 - Exibir historico de operacoes";
        cout << "\n9 - Relatorios gerenciais";
        cout << "\n10 - Executar roteiro integrado de testes";
        cout << "\n0 - Encerrar programa";
        cout << "\n\nEscolha uma opcao: ";
        cin >> opcao;

        switch (opcao) {
            case 1: {
                Carga nova = lerCarga();
                enfileirar(fila, nova);
                cout << "[SUCESSO] Carga " << nova.codigo << " adicionada a fila de espera." << endl;
                break;
            }

            case 2:
                if (filaVazia(fila)) {
                    cout << "[ERRO] Nao ha cargas na fila de espera!" << endl;
                    break;
                }
                cout << "\nProxima carga da fila: " << fila.inicio->carga.codigo
                     << " (" << tipoParaTexto(fila.inicio->carga.tipo)
                     << ", requisitos: " << requisitosEspeciais(fila.inicio->carga) << ")" << endl;
                cout << "Posicao de destino no patio: ";
                cin >> posicao;
                alocarProximaDaFila(fila, patio, historico, posicao);
                break;

            case 3:
                cout << "\nCodigo da carga: ";
                cin >> codigo;
                cout << "Nova posicao: ";
                cin >> posicao;
                movimentarCarga(patio, historico, codigo, posicao);
                break;

            case 4:
                cout << "\nCodigo da carga: ";
                cin >> codigo;
                expedirComHistorico(patio, historico, codigo);
                break;

            case 5:
                desfazerUltimaAcao(historico, patio, fila);
                break;

            case 6:
                cout << "\nCodigo da carga: ";
                cin >> codigo;
                consultarCarga(patio, codigo);
                break;

            case 7:
                exibirPatio(patio);
                break;

            case 8:
                exibirHistorico(historico);
                break;

            case 9:
                emitirRelatorios(patio, fila);
                break;

            case 10:
                roteiroDeTestes();
                break;

            case 0:
                cout << "\nEncerrando o sistema..." << endl;
                break;

            default:
                cout << "\nOpcao invalida!" << endl;
        }
    } while (opcao != 0);

    liberarMemoria(patio, fila, historico);
    return 0;
}
