// TRABALHO B - PILHA SIMPLES (vetor de struct) - LIFO
// Tema: Histórico de Navegação Web

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100
#define ARQUIVO "historico.csv"

// ---------- Struct do item ----------
typedef struct {
    int codigo;
    char url[100];
    char titulo[60];
    char dataHora[20]; // ex: "30/06/2026 14:30"
} Acesso;

// ---------- Pilha ----------
Acesso pilha[MAX];
int topo = -1; // -1 significa pilha vazia. Quando empilha, topo sobe.
int proximoCodigo = 1;

// FUNÇÕES DE APOIO

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int pilhaVazia() {
    return topo == -1;
}

int pilhaCheia() {
    return topo == MAX - 1;
}

// OPERAÇÕES PRINCIPAIS DA PILHA

// EMPILHAR (push) = visitar uma página nova.
// Diferente da lista: aqui a regra é fixa, sempre no topo.
void empilhar() {
    if (pilhaCheia()) {
        printf("\n[ERRO] Histórico cheio! (limite de %d acessos)\n", MAX);
        return;
    }

    Acesso novo;
    novo.codigo = proximoCodigo++;

    printf("\n--- Visitar nova página ---\n");
    printf("URL: ");
    limparBuffer();
    fgets(novo.url, sizeof(novo.url), stdin);
    novo.url[strcspn(novo.url, "\n")] = '\0';

    printf("Título da página: ");
    fgets(novo.titulo, sizeof(novo.titulo), stdin);
    novo.titulo[strcspn(novo.titulo, "\n")] = '\0';

    printf("Data/Hora (ex: 30/06/2026 14:30): ");
    fgets(novo.dataHora, sizeof(novo.dataHora), stdin);
    novo.dataHora[strcspn(novo.dataHora, "\n")] = '\0';

    topo++;            // sobe o topo PRIMEIRO
    pilha[topo] = novo; // depois coloca o item lá

    printf("\n[OK] Acessado: %s\n", novo.url);
}

// DESEMPILHAR (pop) = botão "voltar" do navegador.
// Sempre remove do TOPO - nunca do meio ou do início.
void desempilhar() {
    if (pilhaVazia()) {
        printf("\n[ERRO] Histórico vazio, não há para onde voltar.\n");
        return;
    }

    printf("\n[VOLTAR] Saindo de: %s (%s)\n",
           pilha[topo].titulo, pilha[topo].url);

    topo--; // "remover" numa pilha de vetor é só isso: decrementar o topo.
// O dado antigo continua fisicamente no vetor, mas como
// topo não aponta mais pra lá, ele é "esquecido" e será
// sobrescrito na próxima vez que empilhar.
}

// CONSULTAR O TOPO sem remover (muito comum em pilhas)
void verPaginaAtual() {
    if (pilhaVazia()) {
        printf("\n[INFO] Nenhuma página acessada ainda.\n");
        return;
    }
    printf("\n[PÁGINA ATUAL] %s - %s\n", pilha[topo].titulo, pilha[topo].url);
}

// Lista do topo até a base (ordem de navegação mais recente primeiro)
void listarHistorico() {
    if (pilhaVazia()) {
        printf("\nHistórico vazio.\n");
        return;
    }

    printf("\n%-5s %-30s %-30s %-18s\n", "Cod", "Título", "URL", "Data/Hora");
    printf("----------------------------------------------------------------------------\n");

    // percorre do topo (mais recente) até a base (mais antigo)
    for (int i = topo; i >= 0; i--) {
        printf("%-5d %-30s %-30s %-18s\n",
               pilha[i].codigo, pilha[i].titulo, pilha[i].url, pilha[i].dataHora);
    }
}

// PERSISTÊNCIA EM CSV

void salvarCSV() {
    FILE *f = fopen(ARQUIVO, "w");
    if (f == NULL) {
        printf("\n[ERRO] Não foi possível salvar.\n");
        return;
    }

    fprintf(f, "codigo,url,titulo,dataHora\n");

    // salva da base pro topo - assim, ao recarregar, basta empilhar
// na ordem que está no arquivo e a pilha fica idêntica
    for (int i = 0; i <= topo; i++) {
        fprintf(f, "%d,%s,%s,%s\n",
                pilha[i].codigo, pilha[i].url, pilha[i].titulo, pilha[i].dataHora);
    }

    fclose(f);
    printf("\n[OK] Histórico salvo em %s\n", ARQUIVO);
}

// Função de DEBUG: imprime o conteúdo bruto do arquivo CSV na tela.
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
        printf("%d: %s", numeroLinha, linha);
        numeroLinha++;
    }

    printf("=========================================\n");
    fclose(f);
}

void carregarCSV() {
    FILE *f = fopen(ARQUIVO, "r");
    if (f == NULL) {
        printf("\n[AVISO] Nenhum histórico salvo ainda.\n");
        return;
    }

    char linha[256];
    topo = -1;

    fgets(linha, sizeof(linha), f); // descarta cabeçalho

    while (fgets(linha, sizeof(linha), f) != NULL && topo < MAX - 1) {
        Acesso a;
        sscanf(linha, "%d,%[^,],%[^,],%[^\n]",
               &a.codigo, a.url, a.titulo, a.dataHora);

        topo++;
        pilha[topo] = a;

        if (a.codigo >= proximoCodigo) {
            proximoCodigo = a.codigo + 1;
        }
    }

    fclose(f);
    printf("\n[OK] %d acesso(s) carregado(s).\n", topo + 1);
}

// MENU PRINCIPAL

int main() {
    int opcao;

    carregarCSV();

    do {
        printf("\n========= HISTÓRICO DE NAVEGAÇÃO (PILHA) =========\n");
        printf("1 - Visitar nova página (empilhar)\n");
        printf("2 - Voltar (desempilhar)\n");
        printf("3 - Ver página atual (topo)\n");
        printf("4 - Listar histórico completo\n");
        printf("5 - Salvar em CSV\n");
        printf("6 - Ver conteúdo bruto do CSV (debug)\n");
        printf("0 - Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1: empilhar(); break;
            case 2: desempilhar(); break;
            case 3: verPaginaAtual(); break;
            case 4: listarHistorico(); break;
            case 5: salvarCSV(); break;
            case 6: verConteudoCSV(); break;
            case 0:
                salvarCSV();
                printf("\nSaindo... histórico salvo. Até mais!\n");
                break;
            default:
                printf("\n[ERRO] Opção inválida.\n");
        }

    } while (opcao != 0);

    return 0;
}
