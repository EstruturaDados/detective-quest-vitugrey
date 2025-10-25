// =============================================================================
// PROJETO: Detective Quest - Coleta de Pistas com BST
// DESCRIÇÃO: Expansão do mapa da mansão. O jogador explora a árvore de
//            salas e coleta pistas, que são armazenadas em uma Árvore
//            Binária de Busca (BST) para exibição ordenada.
// CONCEITOS: Árvore Binária, Árvore Binária de Busca (BST), Recursão,
//            Structs, Alocação Dinâmica (malloc), strcmp.
// =============================================================================

// 1. Bibliotecas Necessárias
#include <stdio.h>
#include <stdlib.h> // Para malloc, free
#include <string.h> // Para strcpy, strcmp, strlen

// 2. Definição das Estruturas
// 2.1. Estrutura da Sala (Mapa da Mansão)
struct Sala {
    char nome[50];
    char pista[100]; // Nova adição: campo para a pista
    struct Sala* esquerda;
    struct Sala* direita;
};

// 2.2. Estrutura da Pista (BST de Pistas)
// Nó para a Árvore Binária de Busca (BST)
struct PistaNode {
    char pista[100];
    struct PistaNode* esquerda;
    struct PistaNode* direita;
};

// --- PROTÓTIPOS DAS FUNÇÕES ---
// Funções do Mapa (Árvore de Salas)
struct Sala* criarSala(const char* nome, const char* pista);
void explorarSalasComPistas(struct Sala* salaAtual, struct PistaNode** raizPistas);
void liberarMapa(struct Sala* salaAtual);

// Funções das Pistas (Árvore BST)
struct PistaNode* criarNodePista(const char* pista);
void inserirPista(struct PistaNode** raiz, const char* pista);
void exibirPistas(struct PistaNode* raiz);
void liberarPistasBST(struct PistaNode* raiz);


/**
 * @brief Função principal: monta o mapa e gerencia o jogo.
 *
 * A main() é responsável por:
 * 1. Inicializar a BST de pistas (raiz = NULL).
 * 2. Criar as salas da mansão, agora com pistas.
 * 3. Ligar o mapa (definir caminhos).
 * 4. Iniciar a exploração.
 * 5. Exibir as pistas coletadas no final.
 * 6. Liberar toda a memória.
 */
int main() {
    // --- Inicialização das Árvores ---
    struct Sala* hallDeEntrada = NULL; // Raiz do Mapa
    struct PistaNode* raizPistas = NULL; // Raiz da BST de Pistas

    // --- Montagem do Mapa da Mansão com Pistas ---
    hallDeEntrada = criarSala("Hall de entrada", "Uma bota suja de lama foi deixada perto da porta.");
    struct Sala* salaDeEstar = criarSala("Sala de Estar", "Ha uma xicara de cha ainda morna sobre a mesa.");
    struct Sala* biblioteca = criarSala("Biblioteca", "Um livro sobre venenos esta fora do lugar.");
    struct Sala* cozinha = criarSala("Cozinha", "Facas foram limpas recentemente.");
    struct Sala* jardimDeInverno = criarSala("Jardim de Inverno", NULL); // Sala sem pista
    struct Sala* escritorio = criarSala("Escritorio", "Uma carta amassada esta na lixeira.");
    struct Sala* salaDeMusica = criarSala("Sala de Musica", "Uma corda de piano esta arrebentada.");

    // --- Ligando os Caminhos ---
    hallDeEntrada->esquerda = salaDeEstar;
    hallDeEntrada->direita = biblioteca;
    salaDeEstar->esquerda = cozinha;
    salaDeEstar->direita = jardimDeInverno;
    biblioteca->direita = escritorio;
    jardimDeInverno->esquerda = salaDeMusica;

    // --- Início da Exploração ---
    printf("--- Bem-vindo ao Detective Quest! ---\n");
    printf("Explore a mansao para encontrar as pistas.\n\n");
    
    // Inicia a exploração, passando a raiz do mapa e o ENDEREÇO da raiz da BST
    explorarSalasComPistas(hallDeEntrada, &raizPistas);

    // --- Exibição das Pistas Coletadas ---
    printf("\n\n========================================");
    printf("\n--- Caderno de Pistas (Ordem Alfabética) ---\n");
    if (raizPistas == NULL) {
        printf("Nenhuma pista foi coletada.\n");
    } else {
        exibirPistas(raizPistas); // Chama a travessia em-ordem
    }
    printf("========================================\n");

    // --- Limpeza da Memória ---
    liberarMapa(hallDeEntrada);
    liberarPistasBST(raizPistas);
    printf("\nObrigado por jogar!\n");

    return 0;
}


// =============================================================================
// --- FUNÇÕES DO MAPA (ÁRVORE DE SALAS) ---
// =============================================================================

/**
 * @brief Cria (aloca) uma nova sala com nome e pista.
 *
 * @param nome O nome do cômodo.
 * @param pista A pista encontrada no cômodo (pode ser NULL).
 * @return Um ponteiro para a nova Sala criada.
 */
struct Sala* criarSala(const char* nome, const char* pista) {
    struct Sala* novaSala = (struct Sala*) malloc(sizeof(struct Sala));
    if (novaSala == NULL) {
        printf("Erro fatal: Falha ao alocar memoria!\n");
        exit(1);
    }
    
    strcpy(novaSala->nome, nome);
    
    // Verifica se uma pista foi fornecida
    if (pista != NULL) {
        strcpy(novaSala->pista, pista);
    } else {
        // Se pista for NULL, define a string como vazia
        novaSala->pista[0] = '\0';
    }
    
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;
    return novaSala;
}


/**
 * @brief Controla a navegação do jogador e a coleta de pistas.
 *
 * Controla o loop de navegação. A cada sala visitada,
 * verifica se há uma pista. Se houver, a insere na BST de pistas
 * e apaga a pista da sala (para não coletar de novo).
 *
 * @param salaInicial A sala por onde a exploração começa.
 * @param raizPistas Um ponteiro para o ponteiro da raiz da BST de pistas.
 */
void explorarSalasComPistas(struct Sala* salaInicial, struct PistaNode** raizPistas) {
    struct Sala* salaAtual = salaInicial;
    char escolha;

    while (salaAtual != NULL) {
        printf("----------------------------------------\n");
        printf("Voce esta em: %s\n", salaAtual->nome);
        printf("----------------------------------------\n");

        // --- Lógica de Coleta de Pistas ---
        // Verifica se a string da pista não está vazia
        if (strlen(salaAtual->pista) > 0) {
            printf(">>> Pista encontrada: \"%s\"\n", salaAtual->pista);
            // Insere a pista na Árvore Binária de Busca
            inserirPista(raizPistas, salaAtual->pista);
            // Apaga a pista da sala para não ser coletada novamente
            salaAtual->pista[0] = '\0';
        }

        // --- Lógica de Navegação ---
        if (salaAtual->esquerda == NULL && salaAtual->direita == NULL) {
            printf("Fim do caminho! Nao ha mais salas para explorar a partir daqui.\n");
            break;
        }

        printf("\nEscolha seu proximo movimento:\n");
        if (salaAtual->esquerda != NULL) printf(" (e) Ir para a Esquerda (%s)\n", salaAtual->esquerda->nome);
        if (salaAtual->direita != NULL) printf(" (d) Ir para a Direita (%s)\n", salaAtual->direita->nome);
        printf(" (s) Parar exploracao\n");
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
                printf("Voce guarda seu caderno e encerra a exploracao.\n");
                return;
            default:
                printf("Opcao invalida. Tente novamente.\n");
        }
        printf("\n");
    }
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


// =============================================================================
// --- FUNÇÕES DAS PISTAS (ÁRVORE BST) ---
// =============================================================================

/**
 * @brief Cria (aloca) um novo nó para a BST de pistas.
 *
 * @param pista O texto da pista a ser armazenado.
 * @return Um ponteiro para o novo PistaNode criado.
 */
struct PistaNode* criarNodePista(const char* pista) {
    struct PistaNode* novoNode = (struct PistaNode*) malloc(sizeof(struct PistaNode));
    if (novoNode == NULL) {
        printf("Erro fatal: Falha ao alocar memoria para a pista!\n");
        exit(1);
    }
    strcpy(novoNode->pista, pista);
    novoNode->esquerda = NULL;
    novoNode->direita = NULL;
    return novoNode;
}


/**
 * @brief Insere uma nova pista na BST, mantendo a ordem alfabética.
 *
 * Esta função é recursiva. Ela usa 'strcmp' para decidir se a
 * nova pista deve ir para a sub-árvore esquerda (vem antes)
 * ou direita (vem depois).
 *
 * @param raiz Ponteiro para o ponteiro da raiz (ou sub-árvore).
 * Usamos ponteiro duplo (**) para modificar a raiz.
 * @param pista O texto da pista a ser inserida.
 */
void inserirPista(struct PistaNode** raiz, const char* pista) {
    // Caso Base: Se a árvore/sub-árvore está vazia, criamos o nó aqui.
    if (*raiz == NULL) {
        *raiz = criarNodePista(pista);
        return;
    }

    // Compara a nova pista com a pista do nó atual
    int comparacao = strcmp(pista, (*raiz)->pista);

    // Caso Recursivo:
    if (comparacao < 0) {
        // Nova pista vem antes -> vai para a esquerda
        inserirPista(&(*raiz)->esquerda, pista);
    } else if (comparacao > 0) {
        // Nova pista vem depois -> vai para a direita
        inserirPista(&(*raiz)->direita, pista);
    }
    // Se comparacao == 0, a pista já existe. Não fazemos nada.
}


/**
 * @brief Exibe todas as pistas da BST em ordem alfabética.
 *
 * Esta função usa a travessia "em-ordem" (In-Order Traversal):
 * 1. Visita a sub-árvore esquerda (recursivamente).
 * 2. Visita/Imprime o nó atual.
 * 3. Visita a sub-árvore direita (recursivamente).
 * O resultado é uma impressão ordenada.
 *
 * @param raiz O nó raiz da BST a ser exibida.
 */
void exibirPistas(struct PistaNode* raiz) {
    // Caso Base da recursão: nó vazio, não faz nada.
    if (raiz == NULL) {
        return;
    }

    // 1. Esquerda
    exibirPistas(raiz->esquerda);

    // 2. Raiz (Imprime o item)
    printf(" - \"%s\"\n", raiz->pista);

    // 3. Direita
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