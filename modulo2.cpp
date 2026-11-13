#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iomanip>

using namespace std;

// Tipos de Carga no Terminal
enum TipoCarga { CONTEINER, GRANEL, REFRIGERADA, PERIGOSA };

// Registro principal de informaï¿½ï¿½es da carga
struct Carga {
    string codigo;      // Ex: "CRG0012", "CRG0005"
    TipoCarga tipo;     // Categoria da carga
    float peso;         // Peso em toneladas
    string cliente;     // Nome do proprietï¿½rio/cliente
    string destino;     // Cidade ou porto de destino
    string posicao;     // Posiï¿½ï¿½o no pï¿½tio (Ex: "B-02-01" ou "Fila")
    bool necessitaRefrigeracao;
    bool ehPerigosa;
};

// Nï¿½ para a Lista Duplamente Encadeada (Pï¿½tio de Armazenamento)
struct NoLista {
    Carga carga;
    NoLista* anterior;
    NoLista* proximo;

    NoLista(Carga c) : carga(c), anterior(NULL), proximo(NULL) {}
};

// Estrutura que representa o Pï¿½tio (guarda os ponteiros do inï¿½cio e do fim)
struct ListaPatio {
    NoLista* inicio;
    NoLista* fim;

    ListaPatio() : inicio(NULL), fim(NULL) {}
};

// Nï¿½ para a Fila de Espera (Cargas Pendentes)
struct NoFila {
    Carga carga;
    NoFila* proximo;

    NoFila(Carga c) : carga(c), proximo(NULL) {}
};

// Registro de operaï¿½ï¿½o para o Histï¿½rico (Pilha Undo)
enum TipoOperacao { ALOCACAO, MOVIMENTACAO, EXPEDICAO };

struct Operacao {
    TipoOperacao tipoOp;
    string codigoCarga;
    string posicaoOrigem;
    string posicaoDestino;
};

// Nó para a Pilha de Histï¿½rico
struct NoPilha {
    Operacao op;
    NoPilha* proximo;

    NoPilha(Operacao o) : op(o), proximo(NULL) {}
};

// Retorna o ponteiro para o nó de carga encontrada ou NULL se não existir
NoLista* buscarPorCodigo(const ListaPatio& patio, string codigo){
	NoLista* atual = patio.inicio;
	while(atual != NULL){
		if(atual->carga.codigo == codigo){
			return atual; //Encontrou a carga
		}
		atual = atual->proximo;
	}
	return NULL; //Carga não encontrada
}

bool posicaoEstaOcupada(const ListaPatio& patio, string posicao){
	NoLista* atual = patio.inicio;
	while(atual != NULL){
		if(atual->carga.posicao == posicao){
			return true; //Posição já ocupada
		}
		atual = atual->proximo;
	}
	return false; //Posição livre
}

bool alocarNoPatio(ListaPatio& patio, Carga c, string posicaoDesejada){
	//1 - Valida se a posição já está ocupada
	if(posicaoEstaOcupada(patio,posicaoDesejada)){
		cout<<"[ERRO] A posicao "<<posicaoDesejada<<" ja esta ocupada!"<<endl;
		return false;
	}
	
	//2 - Atualiza a posição da carga
	c.posicao = posicaoDesejada;
	
	//3 - Aloca dinamicamente o novo nó
	NoLista* novo = new NoLista(c);
	
	//4 - Inserção no fim da Lista Duplamente Encadeada
	if(patio.inicio == NULL){
		//Lista estava vazia
		patio.inicio = novo;
		patio.fim = novo;
	} else {
		//Conecta o nó antigo do fim ao novo nó e vice-versa
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
	
	//Reajuste do ponteiro 'proximo' do nó anterior
	if(atual->anterior != NULL){
		atual->anterior->proximo = atual->proximo;
	} else {
		patio.inicio = atual->proximo; //Era o primeiro nó da lista
	}
	
	string posLiberada = atual->carga.posicao;
	
	//Libera a memória do nó removido
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

int main() {
    ListaPatio patio;
    
    //Criando algumas cargas de teste
    Carga c1 = {"CRG01", CONTEINER, 12.5, "Empresa A", "Santos", "", false, false};
    Carga c2 = {"CRG02", GRANEL, 45, "Empresa B", "Parana", "", false, false};
    
    cout<<"---Teste do modulo 2: PATIO DE ARMAZENAMENTO---"<<endl;
    
    //1 - Alocar cargas no patio
    alocarNoPatio(patio, c1, "B-02-01");
    alocarNoPatio(patio, c2, "A-01-05");
    
    exibirPatio(patio);
    
    //2 - Movimentacao de carga
    atualizarLocalizacao(patio, "CRG02", "C-09-09");
    exibirPatio(patio);
    
    //3 - Exepedição de carga
    expedirCarga(patio, "CRG01");
    exibirPatio(patio);
    
    //4 - Limpeza da memória ao encerrar
    while(patio.inicio != NULL){
    	NoLista* temp = patio.inicio;
    	patio.inicio = patio.inicio->proximo;
    	delete temp;
	}
	
	return 0;
}
