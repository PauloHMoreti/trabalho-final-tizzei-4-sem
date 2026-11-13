# 🚢 Sistema de Gerenciamento de Cargas — Terminal Porto Sul

Trabalho final da disciplina de Estrutura de Dados em C++: um sistema de terminal portuário que gerencia a recepção, o armazenamento, o histórico de operações e os relatórios das cargas, usando **fila**, **lista duplamente encadeada** e **pilha** implementadas manualmente com ponteiros.

---

## 👥 Equipe e Responsabilidades

| Integrante | Módulo | Estrutura de Dados |
|------------|--------|--------------------|
| Pedro      | Módulo 1 — Fila de Espera (Recepção e Triagem) | Fila encadeada (FIFO) |
| Paulo      | Módulo 2 — Pátio de Armazenamento | Lista duplamente encadeada |
| Angelo     | Módulo 3 — Histórico e Desfazer | Pilha encadeada (LIFO) |
| Luis       | Módulo 4 — Relatórios Gerenciais | Leitura das estruturas |

---

## 📌 Estratégia de Desenvolvimento

O projeto foi feito em duas fases:

1. **Desenvolvimento Modular:** cada integrante implementou e testou o seu módulo em um arquivo isolado, partindo das estruturas definidas em `codigo-base.cpp`.
2. **Unificação:** depois de validados, os módulos foram integrados em um único executável (`trabalho_final.cpp`), controlado por um menu interativo no terminal.

```
                ┌───────────────────────────┐
                │      codigo-base.cpp      │
                │  (structs compartilhadas) │
                └─────────────┬─────────────┘
                              │
       ┌──────────────┬───────┴───────┬──────────────┐
       ▼              ▼               ▼              ▼
  [Módulo 1]     [Módulo 2]      [Módulo 3]     [Módulo 4]
    Fila           Pátio          Histórico     Relatórios
   (FIFO)      (Lista Dupla)    (Pilha LIFO)   (só leitura)
       └──────────────┴───────┬───────┴──────────────┘
                              │
                              ▼
                ┌───────────────────────────┐
                │    trabalho_final.cpp     │
                │  (Menu Principal / CLI)   │
                └───────────────────────────┘
```

---

## 🛠️ Tecnologias e Padrões Didáticos

O projeto segue o padrão clássico e acadêmico em C++ ensinado na disciplina:

- **Linguagem:** C++ estruturado e procedural, com `struct`s e funções.
- **Bibliotecas usadas:** `<iostream>`, `<string>`, `<cstdlib>`, `<ctime>`, `<iomanip>`.
- **Restrições:** gerenciamento manual de memória com ponteiros, `new` e `delete`; sem `std::vector`, `std::list`, ponteiros inteligentes ou recursos modernos.
- **Compilador de referência:** g++ 4.9.2 (TDM-GCC do Dev-C++).

---

## 📂 Estrutura do Repositório

```text
.
├── README.md                 # Este arquivo
├── modulos.md                # Especificação dos módulos e roteiro de testes
├── codigo-base.cpp           # Estruturas base fornecidas pelo professor
│
├── trabalho_final.cpp        # ✅ SISTEMA UNIFICADO (arquivo de entrega)
│
├── modulo 1.cpp              # Desenvolvimento individual — Módulo 1
├── modulo2.cpp               # Desenvolvimento individual — Módulo 2
├── modulo 3.cpp              # Desenvolvimento individual — Módulo 3
├── modulos 1 e 3.cpp         # Versão alternativa dos módulos 1 e 3 (não usada na unificação)
└── modulo4_relatorios.cpp    # Desenvolvimento individual — Módulo 4
```

> Os arquivos de módulo individuais foram mantidos como registro do desenvolvimento de cada integrante. O arquivo de entrega é o **`trabalho_final.cpp`**.

---

## 📑 Módulos do Sistema

### Estruturas compartilhadas (`codigo-base.cpp`)

| Estrutura | Tipo | Campos |
|-----------|------|--------|
| `Carga` | Registro | `codigo`, `tipo`, `peso`, `cliente`, `destino`, `posicao`, `necessitaRefrigeracao`, `ehPerigosa` |
| `NoFila` / `Fila` | Nó simplesmente encadeado | `carga`, `proximo` / `inicio`, `fim` |
| `NoLista` / `ListaPatio` | Nó duplamente encadeado | `carga`, `anterior`, `proximo` / `inicio`, `fim` |
| `Operacao` | Registro | `tipoOp`, `codigoCarga`, `posicaoOrigem`, `posicaoDestino`, `carga` |
| `NoPilha` / `Pilha` | Nó simplesmente encadeado | `op`, `proximo` / `topo` |

> O campo `carga` da `Operacao` foi acrescentado no Módulo 3: ele guarda uma cópia da carga para que seja possível desfazer uma expedição.

### 1. Fila de Espera — Recepção e Triagem (FIFO)

- `inicializarFila`, `enfileirar`, `desenfileirar`, `filaVazia`.
- As cargas descarregadas aguardam na fila por ordem de chegada até serem alocadas no pátio.

### 2. Pátio de Armazenamento (Lista Duplamente Encadeada)

- `alocarNoPatio`, `buscarPorCodigo`, `expedirCarga`, `atualizarLocalizacao`, `exibirPatio`.
- Validações na alocação:
  - a posição não pode estar ocupada;
  - carga que **necessita refrigeração** só pode ir para um **bloco refrigerado** — posições que começam com `R-` (ex.: `R-01-01`);
  - carga **perigosa** só pode ir para a **área isolada** — posições que começam com `X-` (ex.: `X-01-01`).

### 3. Histórico de Operações e Desfazer (Pilha LIFO)

- `registrarAcao` (push) e `desfazerUltimaAcao` (undo).
- Toda alocação, movimentação e expedição é registrada automaticamente. Ao desfazer:
  - **Movimentação** → a carga volta para a posição de origem;
  - **Alocação** → a carga sai do pátio e volta para o início da fila;
  - **Expedição** → a carga é reinserida no pátio na posição em que estava.

### 4. Relatórios Gerenciais (somente leitura)

- `relatorioConteineres` — total de contêineres no pátio.
- `relatorioPesoGranel` — peso total (t) das cargas de granel no pátio.
- `relatorioFilaPendentes` — tabela das cargas que aguardam vaga na fila.

---

## 🖥️ Menu do Sistema

```
1  - Cadastrar carga na fila de espera
2  - Alocar proxima carga da fila no patio
3  - Movimentar carga no patio
4  - Expedir carga (saida para caminhao)
5  - Desfazer ultima acao
6  - Consultar carga por codigo
7  - Exibir patio
8  - Exibir historico de operacoes
9  - Relatorios gerenciais
10 - Executar roteiro integrado de testes
0  - Encerrar programa
```

A opção **10** executa automaticamente o roteiro integrado de testes da seção 3 do `modulos.md` (triagem, alocação com bloqueio de refrigerada, movimentação, desfazer, expedição e relatórios), mostrando o resultado esperado de cada etapa.

---

## ⚙️ Como Compilar e Executar

### No Dev-C++

Abra o `trabalho_final.cpp` e use **Executar → Compilar & Executar** (F11).

### Pelo terminal

```bash
g++ -Wall trabalho_final.cpp -o trabalho_final
./trabalho_final
```

### Testando um módulo isolado

```bash
g++ -Wall modulo4_relatorios.cpp -o modulo4
./modulo4
```

---

## 👥 Autoria

Desenvolvido por Pedro, Paulo, Angelo e Luis para a disciplina de Estrutura de Dados em C++.
