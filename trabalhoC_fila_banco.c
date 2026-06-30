// TRABALHO C - FILA NORMAL E FILA CIRCULAR (vetor de struct)
// Tema: Atendimento de Banco com Preferencial
// Estratégia: duas filas CIRCULARES separadas.
// - filaPreferencial: idosos, gestantes, PCD
// - filaNormal: demais clientes
// Regra de atendimento: SE houver alguém na preferencial,
// atende ela primeiro. Só atende a normal se a preferencial
// estiver vazia.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 20 // tamanho de CADA fila (capacidade de guichê)
#define ARQUIVO "atendimento.csv"

// ---------- Struct do item (a senha/cliente) ----------
typedef struct {
    int senha;
    char nome[40];
    char tipoAtendimento[20]; // ex: "Saque", "Empréstimo"
} Cliente;

// FILA CIRCULAR - implementação genérica via struct
// Em vez de duplicar o código pra cada fila, criamos um
// "tipo Fila" e instanciamos DUAS variáveis desse tipo.
// Isso é a essência de reaproveitar lógica em C sem POO.
typedef struct {
    Cliente itens[MAX];
    int inicio;     // índice do primeiro elemento da fila
    int fim;        // índice da PRÓXIMA posição livre
    int quantidade; // quantos elementos tem agora (controla cheio/vazio)
} FilaCircular;

FilaCircular filaPreferencial;
FilaCircular filaNormal;
int proximaSenha = 1;

// FUNÇÕES DE APOIO

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void inicializarFila(FilaCircular *f) {
    f->inicio = 0;
    f->fim = 0;
    f->quantidade = 0;
}

int filaVazia(FilaCircular *f) {
    return f->quantidade == 0;
}

int filaCheia(FilaCircular *f) {
    return f->quantidade == MAX;
}

// OPERAÇÕES DA FILA CIRCULAR (genéricas - servem pra qualquer
// uma das duas filas, por isso recebem "FilaCircular *f")

// ENFILEIRAR (entrar na fila): entra sempre no "fim".
// O pulo do gato da fila CIRCULAR está no "% MAX": quando fim
// chega no final do vetor, ele volta pro índice 0 em vez de
// ficar preso, reaproveitando espaços liberados por quem saiu.
int enfileirar(FilaCircular *f, Cliente c) {
    if (filaCheia(f)) {
        return 0; // falhou
    }

    f->itens[f->fim] = c;
    f->fim = (f->fim + 1) % MAX; // <-- AQUI é a circularidade
    f->quantidade++;

    return 1; // sucesso
}

// DESENFILEIRAR (atender/sair da fila): sai sempre do "início".
// Mesmo truque: inicio "anda" e volta pro 0 ao chegar no fim.
int desenfileirar(FilaCircular *f, Cliente *clienteAtendido) {
    if (filaVazia(f)) {
        return 0;
    }

    *clienteAtendido = f->itens[f->inicio];
    f->inicio = (f->inicio + 1) % MAX; // <-- circularidade aqui também
    f->quantidade--;

    return 1;
}

// OPERAÇÕES DO SISTEMA DE ATENDIMENTO

void gerarSenha() {
    Cliente novo;
    novo.senha = proximaSenha++;

    printf("\n--- Gerar Senha ---\n");
    printf("Nome: ");
    limparBuffer();
    fgets(novo.nome, sizeof(novo.nome), stdin);
    novo.nome[strcspn(novo.nome, "\n")] = '\0';

    printf("Tipo de atendimento (ex: Saque, Empréstimo): ");
    fgets(novo.tipoAtendimento, sizeof(novo.tipoAtendimento), stdin);
    novo.tipoAtendimento[strcspn(novo.tipoAtendimento, "\n")] = '\0';

    int opcao;
    printf("É preferencial? (1-Sim / 0-Não): ");
    scanf("%d", &opcao);

    FilaCircular *destino = (opcao == 1) ? &filaPreferencial : &filaNormal;
    const char *nomeFila = (opcao == 1) ? "PREFERENCIAL" : "NORMAL";

    if (enfileirar(destino, novo)) {
        printf("\n[OK] Senha %d gerada (fila %s). Posição na fila: %d\n",
               novo.senha, nomeFila, destino->quantidade);
    } else {
        printf("\n[ERRO] Fila %s está cheia! Tente novamente em instantes.\n", nomeFila);
    }
}

// CHAMAR PRÓXIMO: aqui mora a REGRA DE NEGÓCIO do banco:
// preferencial sempre tem prioridade sobre a normal.
void chamarProximo() {
    Cliente atendido;

    if (!filaVazia(&filaPreferencial)) {
        desenfileirar(&filaPreferencial, &atendido);
        printf("\n[CHAMANDO - PREFERENCIAL] Senha %d - %s (%s)\n",
               atendido.senha, atendido.nome, atendido.tipoAtendimento);
        return;
    }

    if (!filaVazia(&filaNormal)) {
        desenfileirar(&filaNormal, &atendido);
        printf("\n[CHAMANDO - NORMAL] Senha %d - %s (%s)\n",
               atendido.senha, atendido.nome, atendido.tipoAtendimento);
        return;
    }

    printf("\n[INFO] Não há ninguém esperando. Todas as filas vazias.\n");
}

// Lista o conteúdo de UMA fila circular, do início ao fim.
// Como é circular, percorremos "quantidade" vezes a partir do
// índice "inicio", e usamos % MAX para não "vazar" do vetor.
void listarFila(FilaCircular *f, const char *nomeFila) {
    printf("\n--- Fila %s (%d pessoa(s)) ---\n", nomeFila, f->quantidade);

    if (filaVazia(f)) {
        printf("(vazia)\n");
        return;
    }

    for (int i = 0; i < f->quantidade; i++) {
        int indiceReal = (f->inicio + i) % MAX; // <-- circularidade na leitura também
        printf("%dº - Senha %d - %s (%s)\n",
               i + 1,
               f->itens[indiceReal].senha,
               f->itens[indiceReal].nome,
               f->itens[indiceReal].tipoAtendimento);
    }
}

void listarTudo() {
    listarFila(&filaPreferencial, "PREFERENCIAL");
    listarFila(&filaNormal, "NORMAL");
}

// PERSISTÊNCIA EM CSV
// Salvamos as duas filas no mesmo arquivo, com uma coluna a
// mais indicando de qual fila o cliente é.

void salvarFilaNoArquivo(FILE *f, FilaCircular *fila, const char *tipo) {
    for (int i = 0; i < fila->quantidade; i++) {
        int indiceReal = (fila->inicio + i) % MAX;
        fprintf(f, "%d,%s,%s,%s\n",
                fila->itens[indiceReal].senha,
                fila->itens[indiceReal].nome,
                fila->itens[indiceReal].tipoAtendimento,
                tipo);
    }
}

void salvarCSV() {
    FILE *f = fopen(ARQUIVO, "w");
    if (f == NULL) {
        printf("\n[ERRO] Não foi possível salvar.\n");
        return;
    }

    fprintf(f, "senha,nome,tipoAtendimento,filaOrigem\n");
    salvarFilaNoArquivo(f, &filaPreferencial, "PREFERENCIAL");
    salvarFilaNoArquivo(f, &filaNormal, "NORMAL");

    fclose(f);
    printf("\n[OK] Atendimento salvo em %s\n", ARQUIVO);
}

// Função de DEBUG: imprime o conteúdo bruto do arquivo CSV na tela.
void verConteudoCSV() {
    FILE *f = fopen(ARQUIVO, "r");
    if (f == NULL) {
        printf("\n[AVISO] O arquivo %s ainda não existe. Salve algo primeiro (opção 4).\n", ARQUIVO);
        return;
    }

    printf("\n========= CONTEÚDO BRUTO DE %s =========\n", ARQUIVO);

    char linha[256];
    int numeroLinha = 1;
    while (fgets(linha, sizeof(linha), f) != NULL) {
        printf("%d: %s", numeroLinha, linha);
        numeroLinha++;
    }

    printf("=========================================\n");
    fclose(f);
}

void carregarCSV() {
    FILE *f = fopen(ARQUIVO, "r");
    if (f == NULL) {
        printf("\n[AVISO] Nenhum atendimento salvo ainda.\n");
        return;
    }

    inicializarFila(&filaPreferencial);
    inicializarFila(&filaNormal);

    char linha[256];
    fgets(linha, sizeof(linha), f); // descarta cabeçalho

    while (fgets(linha, sizeof(linha), f) != NULL) {
        Cliente c;
        char filaOrigem[20];

        sscanf(linha, "%d,%[^,],%[^,],%[^\n]",
               &c.senha, c.nome, c.tipoAtendimento, filaOrigem);

        if (strcmp(filaOrigem, "PREFERENCIAL") == 0) {
            enfileirar(&filaPreferencial, c);
        } else {
            enfileirar(&filaNormal, c);
        }

        if (c.senha >= proximaSenha) {
            proximaSenha = c.senha + 1;
        }
    }

    fclose(f);
    printf("\n[OK] Filas recarregadas do arquivo.\n");
}

// MENU PRINCIPAL

int main() {
    inicializarFila(&filaPreferencial);
    inicializarFila(&filaNormal);

    carregarCSV();

    int opcao;
    do {
        printf("\n========= ATENDIMENTO BANCÁRIO (FILA CIRCULAR) =========\n");
        printf("1 - Gerar senha (entrar na fila)\n");
        printf("2 - Chamar próximo (preferencial tem prioridade)\n");
        printf("3 - Listar filas\n");
        printf("4 - Salvar em CSV\n");
        printf("5 - Ver conteúdo bruto do CSV (debug)\n");
        printf("0 - Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1: gerarSenha(); break;
            case 2: chamarProximo(); break;
            case 3: listarTudo(); break;
            case 4: salvarCSV(); break;
            case 5: verConteudoCSV(); break;
            case 0:
                salvarCSV();
                printf("\nSaindo... atendimento salvo. Até mais!\n");
                break;
            default:
                printf("\n[ERRO] Opção inválida.\n");
        }

    } while (opcao != 0);

    return 0;
}
