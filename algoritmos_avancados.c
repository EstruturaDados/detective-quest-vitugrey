// =============================================================================
// PROJETO: Detective Quest - Mapa da Mansão com Árvore Binária
// DESCRIÇÃO: Este programa simula a exploração de uma mansão usando uma
//            árvore binária. O mapa é criado dinamicamente e o jogador
//            navega interativamente pelos cômodos.
// CONCEITOS: Árvore Binária, Structs, Alocação Dinâmica (malloc), Ponteiros.
// =============================================================================

// 1. Bibliotecas Necessárias
#include <stdio.h>
#include <stdlib.h> // Para malloc, free
#include <string.h> // Para strcpy

// 2. Definição da Estrutura (Struct) da Sala
// Cada nó da nossa árvore é uma "Sala".
// Ela contém o nome da sala e ponteiros para as salas
// à esquerda e à direita.
struct Sala {
    char nome[50]; // Nome do cômodo
    struct Sala* esquerda; // Ponteiro para a sala à esquerda
    struct Sala* direita;  // Ponteiro para a sala à direita
};

// --- PROTÓTIPOS DAS FUNÇÕES ---
struct Sala* criarSala(const char* nome);
void explorarSalas(struct Sala* salaInicial);
void liberarMapa(struct Sala* salaAtual);


/**
 * @brief Função principal que monta o mapa e inicia o jogo.
 *
 * A função main() é responsável por:
 * 1. Criar todas as salas da mansão usando criarSala().
 * 2. Ligar as salas, definindo a estrutura da árvore (mapa).
 * 3. Iniciar a exploração do jogador a partir do "Hall de entrada".
 * 4. Liberar toda a memória alocada ao final do jogo.
 */
int main() {
    // --- Montagem Automática do Mapa da Mansão ---
    // Criamos a raiz da árvore (o início do jogo)
    struct Sala* hallDeEntrada = criarSala("Hall de entrada");

    // Criamos os outros cômodos
    struct Sala* salaDeEstar = criarSala("Sala de Estar");
    struct Sala* biblioteca = criarSala("Biblioteca");
    struct Sala* cozinha = criarSala("Cozinha");
    struct Sala* jardimDeInverno = criarSala("Jardim de Inverno");
    struct Sala* escritorio = criarSala("Escritorio");
    struct Sala* salaDeMusica = criarSala("Sala de Musica");

    // Agora, ligamos os caminhos (ponteiros)
    // Nível 1
    hallDeEntrada->esquerda = salaDeEstar;
    hallDeEntrada->direita = biblioteca;

    // Nível 2
    salaDeEstar->esquerda = cozinha;
    salaDeEstar->direita = jardimDeInverno;
    biblioteca->direita = escritorio; // A biblioteca não tem caminho à esquerda

    // Nível 3
    jardimDeInverno->esquerda = salaDeMusica;

    // --- Início da Exploração ---
    printf("--- Bem-vindo ao Detective Quest! ---\n");
    printf("Explore a mansao para encontrar as pistas.\n\n");
    
    // Chama a função de navegação, começando pela raiz
    explorarSalas(hallDeEntrada);

    // --- Limpeza da Memória ---
    // Após o fim do jogo, liberamos toda a memória alocada.
    liberarMapa(hallDeEntrada);
    printf("Obrigado por jogar!\n");

    return 0;
}


/**
 * @brief Cria (aloca dinamicamente) uma nova sala.
 *
 * Esta função recebe um nome, aloca memória para uma nova struct Sala,
 * copia o nome para dentro da struct e define os caminhos (filhos)
 * como NULL (sem saída, por padrão).
 *
 * @param nome O nome do cômodo a ser criado.
 * @return Um ponteiro para a nova Sala criada.
 */
struct Sala* criarSala(const char* nome) {
    // 1. Aloca memória do tamanho de uma struct Sala
    struct Sala* novaSala = (struct Sala*) malloc(sizeof(struct Sala));

    // 2. Verifica se a alocação de memória foi bem-sucedida
    if (novaSala == NULL) {
        printf("Erro fatal: Falha ao alocar memoria para uma sala!\n");
        exit(1); // Encerra o programa se não houver memória
    }

    // 3. Copia o nome para a struct
    strcpy(novaSala->nome, nome);

    // 4. Inicializa os caminhos como nulos (sem saída)
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;

    // 5. Retorna o ponteiro para a sala criada
    return novaSala;
}


/**
 * @brief Permite a navegação interativa do jogador pela mansão.
 *
 * Esta função usa um loop 'while' para gerenciar a exploração.
 * Ela mostra a sala atual, verifica se é um fim de caminho (folha)
 * e pede ao jogador para escolher o próximo movimento.
 *
 * @param salaInicial A sala por onde a exploração começa (raiz).
 */
void explorarSalas(struct Sala* salaInicial) {
    struct Sala* salaAtual = salaInicial; // Onde o jogador está agora
    char escolha;

    // O loop continua enquanto o jogador estiver em uma sala válida
    while (salaAtual != NULL) {
        printf("----------------------------------------\n");
        printf("Voce esta em: %s\n", salaAtual->nome);
        printf("----------------------------------------\n");

        // 1. Verifica se é um nó-folha (sem saídas)
        if (salaAtual->esquerda == NULL && salaAtual->direita == NULL) {
            printf("Fim do caminho! Nao ha mais salas para explorar a partir daqui.\n");
            break; // Sai do loop 'while'
        }

        // 2. Mostra as opções de caminho disponíveis
        printf("\nEscolha seu proximo movimento:\n");
        if (salaAtual->esquerda != NULL) {
            printf(" (e) Ir para a Esquerda (%s)\n", salaAtual->esquerda->nome);
        }
        if (salaAtual->direita != NULL) {
            printf(" (d) Ir para a Direita (%s)\n", salaAtual->direita->nome);
        }
        printf(" (s) Parar exploracao\n");
        printf("Opcao: ");

        // 3. Lê a escolha do jogador
        scanf(" %c", &escolha);
        while (getchar() != '\n'); // Limpa o buffer de entrada

        // 4. Processa a escolha
        switch (escolha) {
            case 'e':
                if (salaAtual->esquerda != NULL) {
                    salaAtual = salaAtual->esquerda; // Move o jogador para a esquerda
                } else {
                    printf("Nao ha caminho para a esquerda.\n");
                }
                break;
            case 'd':
                if (salaAtual->direita != NULL) {
                    salaAtual = salaAtual->direita; // Move o jogador para a direita
                } else {
                    printf("Nao ha caminho para a direita.\n");
                }
                break;
            case 's':
                printf("Voce decidiu parar a exploracao por enquanto.\n");
                return; // Sai da função (e do loop)
            default:
                printf("Opcao invalida. Tente novamente.\n");
        }
        printf("\n"); // Adiciona um espaço para legibilidade
    }
}


/**
 * @brief Libera a memória alocada para o mapa.
 *
 * Esta função usa recursão em pós-ordem para garantir que
 * todos os filhos sejam liberados antes dos pais, evitando
 * "ponteiros perdidos".
 *
 * @param salaAtual O nó (sala) a ser liberado.
 */
void liberarMapa(struct Sala* salaAtual) {
    if (salaAtual == NULL) {
        return; // Caso base da recursão
    }

    // 1. Libera os filhos primeiro
    liberarMapa(salaAtual->esquerda);
    liberarMapa(salaAtual->direita);

    // 2. Libera o nó atual (pai)
    // printf("Liberando memoria de: %s\n", salaAtual->nome); // (Para depuração)
    free(salaAtual);
}