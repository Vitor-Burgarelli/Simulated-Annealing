#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define ESCALA 20 
#define LINHAS 91


void inicia_vetor(int *vetor){

    srand(time(NULL));

    int k = rand() % (ESCALA + 1); 

    for (int i = 0; i < k; i++) {
        vetor[i] = 1;
    }

    for (int i = ESCALA - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = vetor[i];
        vetor[i] = vetor[j];
        vetor[j] = temp;
    }
    

    printf("Quantidade de 1s (k) = %d\n\n", k);
    
    printf("Vetor Inicial:\n");
    for (int i = 0; i < ESCALA; i++) {
        printf("%d ", vetor[i]);
    }
    printf("\n");

    return;
}

void pega_clausulas(const char* nome_arquivo, int vetor[][3]){
    FILE *arquivo = fopen(nome_arquivo, "r");
    int i = 0;
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo :(\n");
        exit(1);
    }

    char linha[256];

    while(fgets(linha, sizeof(linha), arquivo) != NULL) {
        sscanf(linha, "%d %d %d", &vetor[i][0], &vetor[i][1], &vetor[i][2]);
        i++;
    }

    fclose(arquivo);
}

int main() { 

    int vetor[ESCALA] = {0};
    int dados[LINHAS][3];

    inicia_vetor(vetor);
    pega_clausulas("instances/20.cnf", dados);
    for(int i = 0; i < LINHAS; i++) {
        for(int j = 0; j < 3; j ++) {
            printf(" %d ", dados[i][j]);
        }
    }

    return 0;
}