# Guia de Estudo: OBDDs (Baseado no artigo de R. Bryant, 1992)

> Foco: Preparação para Implementação em C++ no CADathlon

O artigo de Bryant demonstra que Funções Booleanas podem ser representadas como Grafos Acíclicos Direcionados (DAGs) de forma canônica. Isso significa que, para uma mesma função booleana e uma mesma ordem de variáveis, o grafo gerado será sempre exatamente o mesmo.

Para garantir essa canonicidade e alta performance na sua implementação em C++, você precisa focar nestes pilares fundamentais abordados no artigo:

## 1. A Estrutura do Nó (The Node)

No artigo, uma função é reduzida usando a expansão de Shannon: $f = x \cdot f_{x=1} + \bar{x} \cdot f_{x=0}$.
Isso significa que cada nó em um BDD representa uma variável $x$ e tem duas arestas de saída: low (quando $x=0$) e high (quando $x=1$).

**Visão para C++:**

Não use ponteiros crus `(BDDNode*)`. O segredo de pacotes BDD ultra-rápidos é armazenar os nós em um grande array contíguo na memória (como um `std::vector<BDDNode>`) e usar índices inteiros (IDs) para referenciar os nós filhos.

**Nó 0**: Constante Terminal 0 (Falso).

**Nó 1**: Constante Terminal 1 (Verdadeiro).

**Nós 2 em diante**: Variáveis reais.

```
// Esboço mental da estrutura
struct BDDNode {
    int var_id;  // Índice da variável (ex: x1, x2...). Deve respeitar a ordem!
    int low;     // ID do nó filho (caminho tracejado no artigo, x=0)
    int high;    // ID do nó filho (caminho contínuo no artigo, x=1)
};
```


## 2. As Duas Regras de Redução:

O que torna o OBDD rápido e de tamanho tratável são as regras de redução. No artigo, Bryant fala sobre o algoritmo Reduce, mas na prática (e para a implementação), aplicamos a redução *On-The-Fly* (durante a criação).

**Regra de Eliminação (Redundant Nodes)**: Se o filho `low` de um nó apontar para o mesmo lugar que o filho `high`, esse nó é inútil. A decisão da variável não muda nada.

**Em C++**: Antes de criar um nó, você checa `if (low == high) return low;`

**Regra do Isomorfismo (Equivalent Subgraphs)**: Não podem existir dois nós no sistema com o mesmo `var_id`, mesmo `low` e mesmo `high`.

## 3. A Tabela Única (Unique Table)

Como garantimos a Regra do Isomorfismo de forma rápida? O artigo menciona isso indiretamente, mas é a técnica fundamental de implementação.
Você precisa de uma `Unique Table`. Toda vez que seu programa quiser "criar" um novo nó (`var`, `low`, `high`), ele primeiro checa na Tabela Única se esse nó já existe.

**Visão para C++**:
A Tabela Única é basicamente um `std::unordered_map` (ou uma Hash Table customizada, que é mais rápida).

**Chave (Key)**: Uma tupla contendo (`var_id`, `low`, `high`).

**Valor (Value)**: O ID inteiro do nó.

**Fluxo de Criação (`Find_or_Add_Node`**):

Checa redundância: se `low == high`, retorne `low`.

Procura na UniqueTable a chave (var, low, high).

Se existir, retorne o ID encontrado (você acabou de reaproveitar um subgrafo inteiro!).

Se não existir, adicione no final do seu `std::vector`, adicione na UniqueTable e retorne o novo ID.

## 4. A Tabela Computada (Computed Table / Cache) - O Segredo do Tempo

Quando você for implementar o algoritmo Apply (que faz AND, OR, XOR entre dois BDDs), ele será uma função recursiva. Sem cache, essa recursão cresce exponencialmente **`$O(2^N)$`**. Com cache, a complexidade cai para $O(|G_1| \cdot |G_2|)$ (tamanho dos grafos).

**Visão para C++**:
Você precisará de outra Hash Table, chamada de Computed Table (ou Cache de Operações).

**Chave (Key)**: A operação e os IDs dos operandos: (`Operacao_AND`, `node_id_1`, `node_id_2`).

**Valor (Value)**: O ID do nó resultante.

Antes de expandir a recursão no algoritmo Apply, você sempre checa a Computed Table. Se já fez essa exata operação antes, apenas retorne o resultado armazenado.

## 5. O Algoritmo "Apply" (Onde a Mágica Acontece)

Leia com muita atenção a seção do artigo que fala sobre a operação Apply. A ideia principal é processar os grafos de cima para baixo.
Dada uma operação (ex: AND) entre os nós $f$ e $g$:

Passo Base: Se $f$ e $g$ são nós terminais (0 ou 1), você simplesmente avalia 0 AND 1, 1 AND 1, etc., e retorna o ID do nó constante correspondente.

Passo Recursivo: 

1. Descubra quem é a variável do topo (a variável com o menor índice na ordem, entre $f$ e $g$).
2. Calcule recursivamente para o ramo low: Apply(AND, f_low, g_low)
3. Calcule recursivamente para o ramo high: Apply(AND, f_high, g_high)
4. Crie/Encontre o novo nó usando a Tabela Única com a variável do topo e os dois resultados recursivos.

## 6. Melhorias: Garbage Collection e Ordenação

Ordem das Variáveis: O artigo menciona que a ordem das variáveis afeta drasticamente o tamanho do BDD. Para a função $(a_1 \cdot b_1) + (a_2 \cdot b_2) + \dots$, uma ordem ruim gera crescimento exponencial, enquanto uma ordem boa gera tamanho linear. Em desafios reais, você talvez precise implementar algoritmos de ordenação heurística (como Sifting).

Garbage Collection: Em C++, se você ficar criando e manipulando muitos BDDs no seu std::vector, logo ele ficará cheio de nós "mortos" (subgrafos que não fazem mais parte do BDD final). Você precisará usar um sistema de "Reference Counting" (contagem de referências) em cada nó para saber quando um nó não está mais sendo apontado por ninguém e pode ter sua memória reaproveitada.

## Resumo do que Fazer:

Releia o artigo focando nos diagramas de grafos (Figuras 1 a 6) para entender a canonicidade visualmente.

Entenda perfeitamente a matemática recursiva do algoritmo Apply.

Quando for pro C++, comece escrevendo a estrutura do Nó e a função Find_or_Add_Node com a Tabela Única. Esse é o seu porto seguro!