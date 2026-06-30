// TRABALHO A - LISTA SIMPLES (vetor de struct)
// Tema: Lista de Músicas

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100
#define ARQUIVO "musicas.csv"

// ---------- Struct do item ----------
typedef struct {
    int codigo;
    char titulo[60];
    char artista[40];
    int anoLancamento;
    float duracaoMinutos;
} Musica;

// ---------- "Banco de dados" em memória ----------
Musica lista[MAX];
int quantidade = 0; // controla quantos itens estão realmente preenchidos

// FUNÇÕES DE APOIO

// Limpa o buffer do teclado depois de ler números (evita bug
// clássico de "enter sobrando" quando depois você lê string)
void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Procura uma música pelo código. Retorna o ÍNDICE no vetor,
// ou -1 se não encontrar. Esse padrão de "buscar e retornar
// índice" é a base de quase tudo (remover, alterar, etc).
int buscarPorCodigo(int codigo) {
    for (int i = 0; i < quantidade; i++) {
        if (lista[i].codigo == codigo) {
            return i;
        }
    }
    return -1;
}

// OPERAÇÕES PRINCIPAIS DA LISTA

void inserirMusica() {
    if (quantidade >= MAX) {
        printf("\n[ERRO] Lista cheia! Não é possível inserir.\n");
        return;
    }

    Musica nova;
    nova.codigo = quantidade + 1; // código automático e simples

    printf("\n--- Cadastrar Música ---\n");
    printf("Título: ");
    limparBuffer();
    fgets(nova.titulo, sizeof(nova.titulo), stdin);
    nova.titulo[strcspn(nova.titulo, "\n")] = '\0'; // remove o \n do fgets

    printf("Artista: ");
    fgets(nova.artista, sizeof(nova.artista), stdin);
    nova.artista[strcspn(nova.artista, "\n")] = '\0';

    printf("Ano de lançamento: ");
    scanf("%d", &nova.anoLancamento);

    printf("Duração (minutos, ex: 3.45): ");
    scanf("%f", &nova.duracaoMinutos);

    // insere SEMPRE no final do vetor (mais simples e comum)
    lista[quantidade] = nova;
    quantidade++;

    printf("\n[OK] Música cadastrada com código %d!\n", nova.codigo);
}

void removerMusica() {
    if (quantidade == 0) {
        printf("\n[ERRO] Lista vazia!\n");
        return;
    }

    int codigo;
    printf("\nDigite o código da música a remover: ");
    scanf("%d", &codigo);

    int pos = buscarPorCodigo(codigo);
    if (pos == -1) {
        printf("\n[ERRO] Música não encontrada.\n");
        return;
    }

    // AQUI é o ponto-chave de remover de uma LISTA:
// desloca todo mundo depois da posição removida uma
// casa pra trás, "tampando o buraco".
    for (int i = pos; i < quantidade - 1; i++) {
        lista[i] = lista[i + 1];
    }
    quantidade--;

    printf("\n[OK] Música removida com sucesso!\n");
}

void listarMusicas() {
    if (quantidade == 0) {
        printf("\nLista vazia.\n");
        return;
    }

    printf("\n%-5s %-25s %-20s %-6s %-8s\n",
           "Cod", "Título", "Artista", "Ano", "Duração");
    printf("--------------------------------------------------------------\n");

    for (int i = 0; i < quantidade; i++) {
        printf("%-5d %-25s %-20s %-6d %-8.2f\n",
               lista[i].codigo,
               lista[i].titulo,
               lista[i].artista,
               lista[i].anoLancamento,
               lista[i].duracaoMinutos);
    }
}

void buscarMusica() {
    int codigo;
    printf("\nDigite o código a buscar: ");
    scanf("%d", &codigo);

    int pos = buscarPorCodigo(codigo);
    if (pos == -1) {
        printf("\n[ERRO] Não encontrada.\n");
        return;
    }

    printf("\nEncontrada: %s - %s (%d) [%.2f min]\n",
           lista[pos].titulo, lista[pos].artista,
           lista[pos].anoLancamento, lista[pos].duracaoMinutos);
}

// PERSISTÊNCIA EM CSV

void salvarCSV() {
    FILE *f = fopen(ARQUIVO, "w");
    if (f == NULL) {
        printf("\n[ERRO] Não foi possível abrir o arquivo para escrita.\n");
        return;
    }

    // cabeçalho do CSV - boa prática, facilita debug e leitura humana
    fprintf(f, "codigo,titulo,artista,ano,duracao\n");

    for (int i = 0; i < quantidade; i++) {
        fprintf(f, "%d,%s,%s,%d,%.2f\n",
                lista[i].codigo,
                lista[i].titulo,
                lista[i].artista,
                lista[i].anoLancamento,
                lista[i].duracaoMinutos);
    }

    fclose(f);
    printf("\n[OK] Dados salvos em %s\n", ARQUIVO);
}

// Função de DEBUG: abre o arquivo CSV e imprime o conteúdo BRUTO
// na tela, linha por linha. Útil no OnlineGDB porque lá não dá
// pra navegar nas pastas e ver o arquivo diretamente.
void verConteudoCSV() {
    FILE *f = fopen(ARQUIVO, "r");
    if (f == NULL) {
        printf("\n[AVISO] O arquivo %s ainda não existe. Salve algo primeiro (opção 5).\n", ARQUIVO);
        return;
    }

    printf("\n========= CONTEÚDO BRUTO DE %s =========\n", ARQUIVO);

    char linha[256];
    int numeroLinha = 1;
    while (fgets(linha, sizeof(linha), f) != NULL) {
        // fgets já traz o \n no final, então não precisamos de outro printf("\n")
        printf("%d: %s", numeroLinha, linha);
        numeroLinha++;
    }

    printf("=========================================\n");
    fclose(f);
}

void carregarCSV() {
    FILE *f = fopen(ARQUIVO, "r");
    if (f == NULL) {
        printf("\n[AVISO] Nenhum arquivo encontrado ainda. Começando vazio.\n");
        return;
    }

    char linha[256];
    quantidade = 0;

    fgets(linha, sizeof(linha), f); // descarta o cabeçalho

    while (fgets(linha, sizeof(linha), f) != NULL && quantidade < MAX) {
        Musica m;
        // sscanf com %[^,] lê tudo até encontrar uma vírgula -
// é o jeito clássico de "parsear" CSV simples em C
        sscanf(linha, "%d,%[^,],%[^,],%d,%f",
               &m.codigo, m.titulo, m.artista,
               &m.anoLancamento, &m.duracaoMinutos);

        lista[quantidade] = m;
        quantidade++;
    }

    fclose(f);
    printf("\n[OK] %d música(s) carregada(s) de %s\n", quantidade, ARQUIVO);
}

// MENU PRINCIPAL

int main() {
    int opcao;

    carregarCSV(); // tenta carregar dados salvos anteriormente

    do {
        printf("\n========= LISTA DE MÚSICAS =========\n");
        printf("1 - Cadastrar música\n");
        printf("2 - Remover música\n");
        printf("3 - Listar músicas\n");
        printf("4 - Buscar música por código\n");
        printf("5 - Salvar em CSV\n");
        printf("6 - Ver conteúdo bruto do CSV (debug)\n");
        printf("0 - Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1: inserirMusica(); break;
            case 2: removerMusica(); break;
            case 3: listarMusicas(); break;
            case 4: buscarMusica(); break;
            case 5: salvarCSV(); break;
            case 6: verConteudoCSV(); break;
            case 0:
                salvarCSV(); // salva automaticamente ao sair
                printf("\nSaindo... dados salvos. Até mais!\n");
                break;
            default:
                printf("\n[ERRO] Opção inválida.\n");
        }

    } while (opcao != 0);

    return 0;
}
