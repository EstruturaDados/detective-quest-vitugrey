// =============================================================================
// PROJETO: Detective Quest - O Desafio Mestre
// DESCRIÇÃO: Versão final que integra 3 estruturas de dados:
//            1. Árvore Binária (Mapa da Mansão)
//            2. Árvore Binária de Busca (BST - Pistas Coletadas)
//            3. Tabela Hash (Banco de Dados Pista -> Suspeito)
// CONCEITOS: Todas as estruturas de dados, recursão, alocação dinâmica,
//            tabelas hash (função de hash, tratamento de colisão).
// =============================================================================

// 1. Bibliotecas Necessárias
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- DEFINIÇÕES DAS ESTRUTURAS ---
#define TAMANHO_HASH 10

// 2.1. Estrutura da Sala (Árvore Binária do Mapa)
struct Sala {
    char nome[50];
    char pista[100];
    struct Sala* esquerda;
    struct Sala* direita;
};

// 2.2. Estrutura da Pista (Árvore Binária de Busca - BST)
struct PistaNode {
    char pista[100];
    struct PistaNode* esquerda;
    struct PistaNode* direita;
};

// 2.3. Estrutura da Tabela Hash (Banco de Dados Pista -> Suspeito)
struct HashEntry {
    char pista[100];
    char suspeito[50];
    struct HashEntry* proximo; // Para tratamento de colisão (encadeamento)
};

// Define a Tabela Hash como um vetor de ponteiros para HashEntry
typedef struct HashEntry* TabelaHash[TAMANHO_HASH];

// --- PROTÓTIPOS DAS FUNÇÕES ---

// Funções do Mapa (Árvore de Salas)
struct Sala* criarSala(const char* nome, const char* pista);
void explorarSalas(struct Sala* salaAtual, struct PistaNode** raizPistas);
void liberarMapa(struct Sala* salaAtual);

// Funções das Pistas (BST)
void inserirPista(struct PistaNode** raiz, const char* pista);
void exibirPistas(struct PistaNode* raiz);
void liberarPistasBST(struct PistaNode* raiz);

// Funções da Tabela Hash
unsigned int hash(const char* pista);
void inicializarTabelaHash(TabelaHash tabela);
void inserirNaHash(TabelaHash tabela, const char* pista, const char* suspeito);
const char* encontrarSuspeito(TabelaHash tabela, const char* pista);
void popularTabelaHash(TabelaHash tabela);
void liberarTabelaHash(TabelaHash tabela);

// Funções de Lógica do Jogo
void verificarSuspeitoFinal(struct PistaNode* raizPistas, TabelaHash tabela);
int contarPistasParaSuspeito(struct PistaNode* raizPista, TabelaHash tabela, const char* suspeito);


/**
 * @brief Função principal: Orquestra todo o jogo.
 */
int main() {
    // 1. Inicializar todas as estruturas
    struct Sala* hallDeEntrada = NULL;
    struct PistaNode* raizPistas = NULL;
    TabelaHash tabelaSuspeitos;
    
    inicializarTabelaHash(tabelaSuspeitos);

    // 2. Popular o "Banco de Dados" (Tabela Hash) com a verdade
    popularTabelaHash(tabelaSuspeitos);

    // 3. Montar o Mapa (Árvore Binária)
    hallDeEntrada = criarSala("Hall de entrada", "Uma bota suja de lama foi deixada perto da porta.");
    struct Sala* salaDeEstar = criarSala("Sala de Estar", "Ha uma xicara de cha ainda morna sobre a mesa.");
    struct Sala* biblioteca = criarSala("Biblioteca", "Um livro sobre venenos esta fora do lugar.");
    struct Sala* cozinha = criarSala("Cozinha", "Facas foram limpas recentemente.");
    struct Sala* jardimDeInverno = criarSala("Jardim de Inverno", NULL); // Sem pista
    struct Sala* escritorio = criarSala("Escritorio", "Uma carta amassada esta na lixeira.");
    struct Sala* salaDeMusica = criarSala("Sala de Musica", "Uma corda de piano esta arrebentada.");

    // 4. Ligar os Caminhos
    hallDeEntrada->esquerda = salaDeEstar;
    hallDeEntrada->direita = biblioteca;
    salaDeEstar->esquerda = cozinha;
    salaDeEstar->direita = jardimDeInverno;
    biblioteca->direita = escritorio;
    jardimDeInverno->esquerda = salaDeMusica;

    // 5. Iniciar a Exploração
    printf("--- Bem-vindo ao Detective Quest! ---\n");
    printf("Explore a mansao e colete as pistas.\n\n");
    explorarSalas(hallDeEntrada, &raizPistas);

    // 6. Iniciar a Fase de Julgamento
    verificarSuspeitoFinal(raizPistas, tabelaSuspeitos);

    // 7. Liberar toda a memória alocada
    liberarMapa(hallDeEntrada);
    liberarPistasBST(raizPistas);
    liberarTabelaHash(tabelaSuspeitos);
    
    printf("\nObrigado por jogar!\n");
    return 0;
}

// =============================================================================
// --- FUNÇÕES DA TABELA HASH ---
// =============================================================================

/**
 * @brief Função de Hash (djb2).
 * Converte uma string (pista) em um índice para a tabela.
 */
unsigned int hash(const char* pista) {
    unsigned long hash = 5381;
    int c;
    while ((c = *pista++))
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    return hash % TAMANHO_HASH;
}

/**
 * @brief Inicializa a tabela hash com NULL.
 */
void inicializarTabelaHash(TabelaHash tabela) {
    for (int i = 0; i < TAMANHO_HASH; i++) {
        tabela[i] = NULL;
    }
}

/**
 * @brief Insere uma associação pista/suspeito na tabela hash.
 * Usa encadeamento separado para tratar colisões.
 */
void inserirNaHash(TabelaHash tabela, const char* pista, const char* suspeito) {
    unsigned int indice = hash(pista);
    struct HashEntry* novoEntry = (struct HashEntry*) malloc(sizeof(struct HashEntry));
    if (novoEntry == NULL) {
        printf("Erro fatal de memoria ao criar Hash Entry!\n");
        exit(1);
    }
    strcpy(novoEntry->pista, pista);
    strcpy(novoEntry->suspeito, suspeito);
    
    // Insere no início da lista encadeada (tratamento de colisão)
    novoEntry->proximo = tabela[indice];
    tabela[indice] = novoEntry;
}

/**
 * @brief Consulta o suspeito correspondente a uma pista.
 * @return O nome do suspeito (string) se encontrado, ou NULL se não.
 */
const char* encontrarSuspeito(TabelaHash tabela, const char* pista) {
    unsigned int indice = hash(pista);
    struct HashEntry* atual = tabela[indice];

    // Percorre a lista encadeada naquele índice
    while (atual != NULL) {
        if (strcmp(atual->pista, pista) == 0) {
            return atual->suspeito;
        }
        atual = atual->proximo;
    }
    return NULL; // Pista não encontrada
}

/**
 * @brief Carrega o banco de dados de pistas e suspeitos na tabela hash.
 */
void popularTabelaHash(TabelaHash tabela) {
    // Pistas do Mordomo
    inserirNaHash(tabela, "Uma bota suja de lama foi deixada perto da porta.", "Mordomo");
    inserirNaHash(tabela, "Ha uma xicara de cha ainda morna sobre a mesa.", "Mordomo");
    
    // Pistas da Herdeira
    inserirNaHash(tabela, "Um livro sobre venenos esta fora do lugar.", "Herdeira");
    inserirNaHash(tabela, "Uma carta amassada esta na lixeira.", "Herdeira");

    // Outras pistas
    inserirNaHash(tabela, "Facas foram limpas recentemente.", "Cozinheiro");
    inserirNaHash(tabela, "Uma corda de piano esta arrebentada.", "Musico");
}

/**
 * @brief Libera toda a memória alocada para a tabela hash.
 */
void liberarTabelaHash(TabelaHash tabela) {
    for (int i = 0; i < TAMANHO_HASH; i++) {
        struct HashEntry* atual = tabela[i];
        while (atual != NULL) {
            struct HashEntry* temp = atual;
            atual = atual->proximo;
            free(temp);
        }
    }
}

// =============================================================================
// --- FUNÇÕES DE LÓGICA DO JOGO ---
// =============================================================================

/**
 * @brief Conduz à fase de julgamento final.
 * Pergunta ao jogador, conta as pistas e exibe o desfecho.
 */
void verificarSuspeitoFinal(struct PistaNode* raizPistas, TabelaHash tabela) {
    printf("\n\n========================================");
    printf("\n--- Fase de Julgamento ---");
    printf("\n========================================");

    if (raizPistas == NULL) {
        printf("\nVoce nao coletou nenhuma pista. Nao e possivel acusar ninguem.\n");
        printf("O culpado escapou...\n");
        return;
    }

    // 1. Lista as pistas coletadas (da BST)
    printf("\nSeu caderno de pistas (em ordem alfabetica):\n");
    exibirPistas(raizPistas);

    // 2. Pergunta ao jogador
    char acusado[50];
    printf("\nCom base nas pistas, quem voce acusa?\n");
    printf("Suspeitos possiveis: [Mordomo, Herdeira, Cozinheiro, Musico]\n");
    printf("Acusacao: ");
    fgets(acusado, 50, stdin);
    acusado[strcspn(acusado, "\n")] = 0; // Remove a nova linha

    // 3. Verifica a acusação
    printf("\nAnalisando as evidencias contra '%s'...", acusado);
    
    // 4. Conta as pistas na BST que apontam para o acusado (via Hash Table)
    int numPistas = contarPistasParaSuspeito(raizPistas, tabela, acusado);

    // 5. Exibe o Desfecho
    printf("\n--- O Desfecho ---\n");
    printf("Voce acusou: %s\n", acusado);
    printf("Voce encontrou %d pista(s) que apontam para essa pessoa.\n\n", numPistas);

    if (numPistas >= 2) {
        printf("Evidencias conclusivas! As pistas sao suficientes.\n");
        printf("O culpado foi preso! Voce venceu, Detetive!\n");
    } else {
        printf("Evidencias insuficientes! O verdadeiro culpado escapou.\n");
        printf("Voce perdeu o caso...\n");
    }
}

/**
 * @brief Conta recursivamente quantas pistas na BST coletada
 * apontam para um suspeito específico.
 *
 * @param raizPista Nó atual da BST de pistas coletadas.
 * @param tabela A tabela hash (banco de dados).
 * @param suspeito O nome do suspeito a ser verificado.
 * @return O número de pistas encontradas para esse suspeito.
 */
int contarPistasParaSuspeito(struct PistaNode* raizPista, TabelaHash tabela, const char* suspeito) {
    // Caso Base: nó vazio, 0 pistas
    if (raizPista == NULL) {
        return 0;
    }

    int contagem = 0;
    
    // 1. Verifica a pista neste nó
    // Encontra o suspeito associado a esta pista no banco de dados
    const char* suspeitoDaPista = encontrarSuspeito(tabela, raizPista->pista);
    
    // Se a pista foi encontrada no banco de dados E
    // o suspeito dela é o mesmo que o acusado...
    if (suspeitoDaPista != NULL && strcmp(suspeitoDaPista, suspeito) == 0) {
        contagem = 1; // ...conta +1
    }

    // 2. Soma a contagem deste nó com a dos filhos
    // (Travessia Pré-Ordem)
    return contagem + 
           contarPistasParaSuspeito(raizPista->esquerda, tabela, suspeito) +
           contarPistasParaSuspeito(raizPista->direita, tabela, suspeito);
}


// =============================================================================
// --- FUNÇÕES BÁSICAS (MAPA E BST - DO DESAFIO ANTERIOR) ---
// =============================================================================

/**
 * @brief Cria (aloca) uma nova sala com nome e pista.
 */
struct Sala* criarSala(const char* nome, const char* pista) {
    struct Sala* novaSala = (struct Sala*) malloc(sizeof(struct Sala));
    if (novaSala == NULL) { exit(1); } // Simplificado
    strcpy(novaSala->nome, nome);
    if (pista != NULL) strcpy(novaSala->pista, pista);
    else novaSala->pista[0] = '\0';
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;
    return novaSala;
}

/**
 * @brief Libera a memória alocada para o mapa (pós-ordem).
 */
void liberarMapa(struct Sala* salaAtual) {
    if (salaAtual == NULL) return;
    liberarMapa(salaAtual->esquerda);
    liberarMapa(salaAtual->direita);
    free(salaAtual);
}

/**
 * @brief Cria (aloca) um novo nó para a BST de pistas.
 */
struct PistaNode* criarNodePista(const char* pista) {
    struct PistaNode* novoNode = (struct PistaNode*) malloc(sizeof(struct PistaNode));
    if (novoNode == NULL) { exit(1); } // Simplificado
    strcpy(novoNode->pista, pista);
    novoNode->esquerda = NULL;
    novoNode->direita = NULL;
    return novoNode;
}

/**
 * @brief Insere uma nova pista na BST, mantendo a ordem alfabética.
 */
void inserirPista(struct PistaNode** raiz, const char* pista) {
    if (*raiz == NULL) {
        *raiz = criarNodePista(pista);
        return;
    }
    int comparacao = strcmp(pista, (*raiz)->pista);
    if (comparacao < 0) {
        inserirPista(&(*raiz)->esquerda, pista);
    } else if (comparacao > 0) {
        inserirPista(&(*raiz)->direita, pista);
    }
    // Se comparacao == 0, a pista já existe (não insere duplicata)
}

/**
 * @brief Exibe todas as pistas da BST em ordem alfabética (em-ordem).
 */
void exibirPistas(struct PistaNode* raiz) {
    if (raiz == NULL) return;
    exibirPistas(raiz->esquerda);
    printf(" - \"%s\"\n", raiz->pista);
    exibirPistas(raiz->direita);
}

/**
 * @brief Libera a memória alocada para a BST de pistas (pós-ordem).
 */
void liberarPistasBST(struct PistaNode* raiz) {
    if (raiz == NULL) return;
    liberarPistasBST(raiz->esquerda);
    liberarPistasBST(raiz->direita);
    free(raiz);
}

/**
 * @brief Controla a navegação do jogador e a coleta de pistas.
 */
void explorarSalas(struct Sala* salaInicial, struct PistaNode** raizPistas) {
    struct Sala* salaAtual = salaInicial;
    char escolha;

    while (salaAtual != NULL) {
        printf("----------------------------------------\n");
        printf("Voce esta em: %s\n", salaAtual->nome);
        printf("----------------------------------------\n");

        if (strlen(salaAtual->pista) > 0) {
            printf(">>> Pista encontrada: \"%s\"\n", salaAtual->pista);
            inserirPista(raizPistas, salaAtual->pista);
            salaAtual->pista[0] = '\0'; // Marca como coletada
        }

        if (salaAtual->esquerda == NULL && salaAtual->direita == NULL) {
            printf("Fim do caminho! Nao ha mais salas para explorar a partir daqui.\n");
            break;
        }

        printf("\nEscolha seu proximo movimento:\n");
        if (salaAtual->esquerda != NULL) printf(" (e) Ir para a Esquerda (%s)\n", salaAtual->esquerda->nome);
        if (salaAtual->direita != NULL) printf(" (d) Ir para a Direita (%s)\n", salaAtual->direita->nome);
        printf(" (s) Parar exploracao (ir para o julgamento)\n");
        printf("Opcao: ");

        scanf(" %c", &escolha);
        while (getchar() != '\n'); 

        switch (escolha) {
            case 'e':
                if (salaAtual->esquerda != NULL) salaAtual = salaAtual->esquerda;
                else printf("Nao ha caminho para a esquerda.\n");
                break;
            case 'd':
                if (salaAtual->direita != NULL) salaAtual = salaAtual->direita;
                else printf("Nao ha caminho para a direita.\n");
                break;
            case 's':
                printf("Voce guarda seu caderno e se prepara para a acusacao...\n");
                return;
            default:
                printf("Opcao invalida. Tente novamente.\n");
        }
        printf("\n");
    }
}