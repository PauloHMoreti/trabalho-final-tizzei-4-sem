# Especificação de Módulos e Guia de Início Rápido: Sistema de Gerenciamento de Cargas (Terminal Porto Sul)

O sucesso no desenvolvimento de sistemas modulares em C++ depende do alinhamento rigoroso sobre a arquitetura de dados e sobre as responsabilidades de cada componente. No Terminal Porto Sul, a gestão logístico-portuária é dividida em quatro módulos funcionais interdependentes, que compartilham as mesmas estruturas base de dados.

Este documento fornece as instruções de inicialização do projeto e o detalhamento funcional de cada módulo, sem a inclusão de trechos de código de implementação, permitindo que cada integrante ou dupla foque na construção das funções sob sua responsabilidade.

---

## 1. Guia de Início Rápido e Contrato de Integração

Para iniciar o desenvolvimento individual ou em equipe, o projeto adota um arquivo de cabeçalho e tipos comuns denominado `codigo-base.cpp`. Esse arquivo contém a definição das entidades da aplicação, dos enumerados e dos nós de memória das três estruturas de dados.

### Procedimento de Inicialização

1. **Obtenção do Código-Base**: Copie a estrutura de dados definida no Tópico 2 do Guia de Desenvolvimento do sistema (armazenada no repositório do projeto como `codigo-base.cpp`).
2. **Criação do Arquivo de Trabalho**: Crie um novo arquivo fonte para o módulo sob sua responsabilidade ou inclua as `structs` do `codigo-base.cpp` no topo do seu arquivo de desenvolvimento.
3. **Preservação das Assinaturas**: Não altere os nomes dos campos das estruturas `Carga`, `NoLista`, `NoFila`, `NoPilha` ou `Operacao`. A padronização dos nomes garante que os módulos se conectem perfeitamente na etapa de integração final.
4. **Respeito às Restrições da Disciplina**: Mantenha o gerenciamento manual de memória via ponteiros e os operadores `new` e `delete`, abstendo-se de utilizar bibliotecas avançadas da STL como `std::vector` ou `std::list`.

### Tabela de Contrato das Estruturas de Dados

| Estrutura Base | Tipo de Encapsulamento | Campos Integrantes | Responsabilidade no Sistema |
| :--- | :--- | :--- | :--- |
| **`Carga`** | Registro (`struct`) | `codigo`, `tipo`, `peso`, `cliente`, `destino`, `posicao`, `necessitaRefrigeracao`, `ehPerigosa` | Dados cadastrais e operacionais da mercadoria. |
| **`NoFila`** | Nó Simplesmente Encadeado | `carga` (dados), `proximo` (ponteiro) | Elemento da zona de triagem e espera. |
| **`NoLista`** | Nó Duplamente Encadeado | `carga` (dados), `anterior` e `proximo` (ponteiros) | Elemento do pátio de estocagem por bloco. |
| **`Operacao`** | Registro (`struct`) | `tipoOp`, `codigoCarga`, `posicaoOrigem`, `posicaoDestino` | Metadados para auditoria e reversão. |
| **`NoPilha`** | Nó Simplesmente Encadeado | `op` (dados de operação), `proximo` (ponteiro) | Elemento do histórico recente de ações. |

---

## 2. Detalhamento Funcional dos Módulos

### Módulo 1: Fila de Espera de Cargas Pendentes (Recepção e Triagem)

O Módulo 1 é responsável por gerenciar a chegada das cargas descarregadas das embarcações (como o navio Atlântico Norte) antes de sua alocação física nas posições do pátio. Ele opera sob a política de atendimento estrito por ordem de chegada (FIFO).

#### Especificação das Funções do Módulo 1

*   **Função de Inicialização da Fila**:
    *   *Objetivo*: Configurar os ponteiros de controle de início e fim da fila como nulos.
    *   *Parâmetros*: Recebe a estrutura da fila por referência (`Fila&`).
    *   *Comportamento*: Garante que a estrutura nasça em um estado válido e seguro.

*   **Função de Enfileiramento (`enfileirar`)**:
    *   *Objetivo*: Inserir uma nova carga cadastrada no final da fila de espera.
    *   *Parâmetros*: Recebe a estrutura da fila por referência (`Fila&`) e o registro da carga (`Carga`).
    *   *Comportamento*: Aloca dinamicamente um novo nó de fila (`NoFila`). Se a fila estiver vazia, define tanto o ponteiro inicial quanto o final para o novo nó. Caso contrário, conecta o antigo nó final ao novo e atualiza o ponteiro de fim.

*   **Função de Desenfileiramento (`desenfileirar`)**:
    *   *Objetivo*: Remover a carga que está há mais tempo aguardando no topo da fila para processar sua alocação no pátio.
    *   *Parâmetros*: Recebe a estrutura da fila por referência (`Fila&`) e retorna a carga removida.
    *   *Comportamento*: Salva a carga do nó inicial em uma variável temporária, avança o ponteiro de início para o próximo nó, desaloca a memória do nó removido com `delete` e atualiza o ponteiro de fim para nulo caso a fila fique totalmente vazia.

*   **Função de Verificação de Fila Vazia**:
    *   *Objetivo*: Informar se existem cargas aguardando triagem.
    *   *Parâmetros*: Recebe a fila por valor ou referência constante.
    *   *Retorno*: Verdadeiro se o ponteiro de início for nulo; falso caso contrário.

---

### Módulo 2: Pátio de Armazenamento (Lista Duplamente Encadeada)

O Módulo 2 representa o pátio de estocagem do terminal. Como as cargas podem ser consultadas, retiradas ou movimentadas de forma dinâmica em qualquer posição sem uma ordem pré-definida, a lista duplamente encadeada garante acesso e manipulação flexível através dos ponteiros anterior e próximo.

#### Especificação das Funções do Módulo 2

*   **Função de Alocação no Pátio**:
    *   *Objetivo*: Inserir uma carga liberada da fila em uma posição específica do pátio (ex: B-02-01).
    *   *Parâmetros*: Recebe a lista do pátio por referência (`Lista&`), a carga a ser alocada e a posição de destino (`string`).
    *   *Regras de Negócio e Validações*:
        1.  Verificar se a posição desejada já se encontra ocupada por outra carga na lista.
        2.  Se a carga exigir refrigeração (`necessitaRefrigeracao == true`), validar se a posição pertence a um bloco refrigerado.
        3.  Se a carga for classificada como perigosa (`ehPerigosa == true`), validar se a posição está na área isolada.
    *   *Comportamento*: Se aprovada na validação, aloca o nó duplamente encadeado (`NoLista`), atualiza o campo de posição da carga e o insere na lista duplamente encadeada ajustando os ponteiros bidirecionais (`anterior` e `proximo`).

*   **Função de Consulta por Código**:
    *   *Objetivo*: Localizar e retornar as informações detalhadas de uma carga armazenada.
    *   *Parâmetros*: Recebe a lista do pátio por referência constante (`const Lista&`) e a string do código de busca (ex: "CRG0005").
    *   *Comportamento*: Percorre a lista do início ao fim comparando os códigos. Retorna o ponteiro para o nó encontrado ou nulo se a carga não estiver no pátio.

*   **Função de Expedição de Cargas (Saída para Caminhão)**:
    *   *Objetivo*: Remover a carga do pátio quando um veículo terrestre realiza a retirada.
    *   *Parâmetros*: Recebe a lista do pátio por referência (`Lista&`) e o código da carga a ser expedida.
    *   *Comportamento*: Busca a carga na lista. Ao encontrá-la, ajusta o ponteiro `proximo` do nó anterior e o ponteiro `anterior` do nó seguinte para se conectarem diretamente. Atualiza os ponteiros de início ou fim da lista se a carga estiver nas pontas e desaloca a memória do nó com `delete`.

*   **Função de Atualização de Localização**:
    *   *Objetivo*: Mover uma carga de uma posição no pátio para outra.
    *   *Parâmetros*: Recebe a lista por referência (`Lista&`), o código da carga e a nova posição.
    *   *Comportamento*: Localiza o nó da carga, armazena a posição antiga, altera o atributo de posição para o novo valor e solicita ao Módulo 3 o registro da movimentação no histórico.

---

### Módulo 3: Histórico de Operações e Mecanismo de Desfazer (Pilha LIFO)

O Módulo 3 é responsável pela auditoria operacional e pela segurança do pátio, permitindo reverter erros cometidos pelos operadores (como alocação em blocos incorretos). Ele segue a política LIFO, onde a última operação realizada é a primeira a ser desfeita.

#### Especificação das Funções do Módulo 3

*   **Função de Registro de Ação (`push`)**:
    *   *Objetivo*: Gravar cada evento de alocação, movimentação ou expedição no topo do histórico.
    *   *Parâmetros*: Recebe a pilha por referência (`Pilha&`) e a estrutura de operação (`Operacao`) contendo o tipo da ação, o código da carga e as posições envolvidas.
    *   *Comportamento*: Aloca um nó de pilha (`NoPilha`), define o seu ponteiro `proximo` para o topo atual e atualiza o topo da pilha para o novo nó.

*   **Função de Reversão de Operação (`desfazerUltimaAcao`)**:
    *   *Objetivo*: Desfazer a última ação registrada no topo do histórico (mecanismo Undo).
    *   *Parâmetros*: Recebe a pilha por referência (`Pilha&`), a lista do pátio por referência (`Lista&`) e a fila de espera por referência (`Fila&`).
    *   *Regras de Reversão*:
        1.  *Se a última ação foi uma Movimentação*: Busca a carga no pátio e altera sua localização de volta para a posição de origem.
        2.  *Se a última ação foi uma Alocação*: Remove a carga do pátio e a devolve para a fila de espera.
        3.  *Se a última ação foi uma Expedição*: Reinsere a carga expedida de volta no pátio na posição em que ela se encontrava.
    *   *Comportamento*: Após executar a ação inversa, remove o nó do topo da pilha, atualiza o ponteiro de topo e desaloca a memória do registro revertido com `delete`.

---

### Módulo 4: Relatórios Estatísticos e Consultas Gerenciais

O Módulo 4 disponibiliza a visão consolidada para a supervisão do terminal. Ele realiza varreduras de leitura sobre as estruturas de dados sem alterar seus elementos ou ponteiros.

#### Especificação dos Algoritmos de Relatório

*   **Relatório de Contêineres Armazenados**:
    *   *Algoritmo*: Percorre a lista duplamente encadeada do pátio a partir do nó inicial. Para cada nó onde a categoria corresponda a contêiner (`tipo == CONTEINER`), incrementa um contador específico. Ao final, exibe o total consolidado.

*   **Relatório de Peso de Cargas de Granel**:
    *   *Algoritmo*: Percorre a lista duplamente encadeada filtrando os nós com categoria igual a granel (`tipo == GRANEL`). Acumula o valor do campo de peso em uma variável do tipo ponto flutuante (`float`) e apresenta o peso total em toneladas armazenado no pátio.

*   **Relatório de Cargas Pendentes na Fila**:
    *   *Algoritmo*: Percorre a fila de espera do início ao fim sem remover nenhum elemento. Imprime em formato de tabela o código, a categoria, o cliente e os requisitos especiais (como refrigeração ou periculosidade) de cada carga que continua aguardando vaga.

---

## 3. Roteiro Integrado de Testes e Validação do Módulo

Após a implementação individual das funções de cada módulo, a equipe deve validar o comportamento integrado executando a sequência operacional abaixo na função principal (`main`):

1.  **Etapa de Triagem**: Inserir 5 cargas variadas na Fila de Espera (incluindo cargas que necessitam de refrigeração e cargas comuns).
2.  **Etapa de Alocação**: Tentar alocar as cargas no pátio através da Lista Duplamente Encadeada, verificando o bloqueio de cargas refrigeradas em posições não compatíveis.
3.  **Etapa de Movimentação e Histórico**: Mover uma carga de posição no pátio e validar o registro automático da operação na Pilha.
4.  **Etapa de Reversão (Undo)**: Acionar a função de desfazer e confirmar se a carga retornou à posição anterior.
5.  **Etapa de Expedição**: Realizar a saída de uma carga e validar a liberação de vaga no pátio.
6.  **Etapa de Emissão de Relatórios**: Executar o Módulo 4 e conferir se os totais de contêineres, peso de granel e itens pendentes na fila correspondem ao estado real das estruturas.
