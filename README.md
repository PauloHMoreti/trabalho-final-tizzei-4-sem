# 🚀 Projeto de Estruturas de Dados e Algoritmos em C++

Repositório dedicado ao estudo prático, implementação modular e consolidação dos conceitos fundamentais de algoritmos e estruturas de dados em C++.

---

## 👥 Equipe e Responsabilidades

| Integrante | Módulo |
|------------|--------|
| Pedro      | Módulo 1 |
| Paulo      | Módulo 2 |
| Angelo     | Módulo 3 |
| Luis       | Módulo 4 |

---

## 📌 Estratégia de Desenvolvimento

O projeto adota uma abordagem em duas fases para garantir domínio conceitual e facilidade de depuração:

1. **Desenvolvimento Modular (Em Partes):** Cada tópico (funções, recursão, vetores, matrizes, pilhas e filas) é desenvolvido, testado e validado em arquivos e funções isoladas dentro do diretório `modules/`.
2. **Unificação (Sistema Integrado):** Após a validação individual de cada estrutura, todos os módulos serão integrados em um executável único (`main.cpp`), gerenciado por um menu interativo de seleção no terminal.

```
                ┌───────────────────────────┐
                │  Desenvolvimento Modular  │
                │   (Módulos Isolados)      │
                └─────────────┬─────────────┘
                              │
     ┌────────────┬───────────┼───────────┬────────────┐
     ▼            ▼           ▼           ▼            ▼
 [Funções]   [Recursão]   [Matrizes]   [Pilhas]     [Filas]
     └────────────┴───────────┼───────────┴────────────┘
                              │
                              ▼
                ┌───────────────────────────┐
                │  Integração e Unificação  │
                │   (Menu Principal/CLI)    │
                └───────────────────────────┘
```

---

## 🛠️ Tecnologias e Padrões Didáticos

O projeto segue estritamente o padrão clássico e acadêmico em C++:

- **Linguagem:** C++ (padrão estruturado e procedural com structs)
- **Bibliotecas Padrão Permitidas:** `<iostream>`, `<cstdlib>`, `<ctime>`, `<cmath>`, `<iomanip>`, `<string>`, `<stack>`, `<queue>`
- **Restrições de Implementação:** Foco em fundamentos de memória e ponteiros, evitando abstrações modernas complexas (sem ponteiros inteligentes, sem lambdas/templates avançados, implementações manuais via ponteiros e `new`/`delete`).

---

## 📂 Estrutura do Repositório

```text
.
├── README.md
├── src/
│   ├── main.cpp                  # Ponto de entrada unificado (menu principal)
│   ├── modules/                  # Módulos em desenvolvimento individual
│   │   ├── 01_funcoes/           # Passagem de parâmetros e modularização
│   │   ├── 02_recursao/          # Algoritmos iterativos vs. recursivos
│   │   ├── 03_vetores/           # Manipulação e formatação de arrays unidimensionais
│   │   ├── 04_matrizes/          # Operações e propriedades de matrizes
│   │   ├── 05_pilhas_stl/        # Pilha estática / STL (<stack>)
│   │   ├── 06_pilhas_dinamicas/  # Pilhas encadeadas manuais com struct e nós
│   │   ├── 07_filas/             # Filas sequenciais e circulares / STL (<queue>)
│   │   └── 08_listas/            # Listas simplesmente e duplamente encadeadas
│   └── include/                  # Cabeçalhos (.h / .hpp) para a etapa de unificação
└── build/                        # Arquivos binários compilados
```

---

## 📑 Módulos de Estudo

### 1. Funções e Modularização

- Funções sem retorno e sem parâmetro.
- Funções com parâmetros (por valor e por referência).
- Funções com retorno e integração matemática (`<cmath>`).

### 2. Recursão vs. Iteração

- Identificação clara de Caso Base e Caso Recursivo.
- Cálculo de fatorial, potências e busca de maior elemento em vetores.

### 3. Vetores e Matrizes

- Vetores dinâmicos e estáticos, preenchimento e busca.
- Propriedades de matrizes quadradas: diagonal principal, secundária, transposta e ordenação.

### 4. Pilhas (LIFO - Last In, First Out)

- Operações com biblioteca STL (`push`, `pop`, `top`, `empty`, `size`).
- Implementação manual dinâmica usando nós e ponteiros (`struct No`, alocação com `new` e liberação com `delete`).
- Aplicações práticas: conversão de bases numéricas e balanceamento de parênteses.

### 5. Filas (FIFO - First In, First Out) e Listas

- Inserção no fim (`push`), remoção no início (`pop`) e inspeção de extremidades (`front`, `back`).
- Simulação de filas de atendimento e separação de pares/ímpares.
- Listas simplesmente e duplamente encadeadas.

---

## ⚙️ Como Compilar e Executar

### Testando um módulo isolado

```bash
# Exemplo: compilar o módulo de pilhas encadeadas
g++ -Wall -O2 src/modules/06_pilhas_dinamicas/pilha_struct.cpp -o build/pilha_teste
./build/pilha_teste
```

### Compilando o projeto unificado

```bash
g++ -Wall -O2 src/main.cpp -o build/programa_unificado
./build/programa_unificado
```

---

## 👥 Autoria e Contribuição

Desenvolvido para fins de estudo e consolidação prática da disciplina de Estrutura de Dados em C++. Sinta-se à vontade para abrir *Issues* ou enviar *Pull Requests* com melhorias e exercícios resolvidos.
