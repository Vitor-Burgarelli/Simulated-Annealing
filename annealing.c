#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define ESCALA 20 
#define LINHAS 91
#define ITERACOES 200000

//FUNCAO OBJETIVO -> minimizar clausulas falsas!!
//Funcao de qualidade -> quantidade de clausulas falsas / total clausulas (linhas)
//Delta = qualidade_atual - qualidade_vizinho 

void inicia_valores(int *valores){

    srand(time(NULL));

    int k = rand() % (ESCALA + 1); 

    for (int i = 0; i < k; i++) {
        valores[i] = 1;
    }

    for (int i = ESCALA - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = valores[i];
        valores[i] = valores[j];
        valores[j] = temp;
    }
    

    printf("Quantidade de 1s (k) = %d\n\n", k);
    
    printf("Valores Iniciais:\n");
    for (int i = 0; i < ESCALA; i++) {
        printf("%d ", valores[i]);
    }
    printf("\n");

    return;
}

void pega_clausulas(const char* nome_arquivo, int dados[][3]){
    FILE *arquivo = fopen(nome_arquivo, "r");
    int i = 0;
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo :(\n");
        exit(1);
    }

    char linha[256];

    while(fgets(linha, sizeof(linha), arquivo) != NULL) {
        sscanf(linha, "%d %d %d", &dados[i][0], &dados[i][1], &dados[i][2]);
        i++;
    }

    fclose(arquivo);
}

double calcula_qualidade(int *valores, int dados[][3]){

    int qtd_clausulas = LINHAS;
    int qtd_falsas = 0;

    for(int i = 0; i < qtd_clausulas; i++){

        int contador = 0;
        for(int j = 0; j < 3; j++){

            if(dados[i][j] < 0){
                if ((valores[(dados[i][j] * (-1)) - 1]) == 1){
                    contador++;
                }

            }else if (dados[i][j] > 0){
                if (valores[dados[i][j] - 1] == 0){
                    contador++;
                }
            }
        }
        if(contador == 3){ qtd_falsas++;}
    }


    double funcao_qualidade = (double)qtd_falsas/qtd_clausulas;

    printf("quntidade de clausulas = %d", qtd_clausulas);
    printf("\nqtd_falsas = %d\nfuncao_qualidade = %lf", qtd_falsas, funcao_qualidade);

    return funcao_qualidade;

}

void atriubui_vetor(int* vetor_receber, int* vetor_passar, tam){
    for(int i = 0; i < tam; i++){
        vetor_receber[i] = vetor_passar[i];
    }
}

int* annealing(int *valores, int **dados, iterações, temp_inicial){

    double delta = 0;
    int valores_atual[ESCALA] = {0};

    for(int i = 0; i < ESCALA; i++){
        valores_atual[i] = valores[i];
    }

    int valores_vizinho[ESCALA] = {0};

    double qualidade_atual = calcula_qualidade(valores, dados);   //s(valor otimo inicial)
    double qualidade_vizinho = 0.0;

    for(int i = 0; i < iterações; i++){

        gerar_vizinho(valores_vizinho);
        qualidade_vizinho = calcula_qualidade(valores_vizinho, dados);
        delta = qualidade_vizinho - qualidade_atual;

        if(delta < 0){
            atribui_vetor(valores, valores_vizinho, ESCALA);    
            if(qualidade_vizinho < calcula_qualidade(valores_atual)){
                atribui_vetor(valores_atual, valores_vizinho);
            }
        }else {
            
        }

    }

    return valores;
}

void gerar_vizinho(int* valores_vizinho){

    int indice = -1;
    int n = ESCALA;
    int mudancas = ESCALA * (5/100)
    for(int i = 0; i<mudancas ; i++){
        indice = rand() % n;
        valores_vizinho[indice] = 1 - valores_vizinho[indice];
    }
}

int main() { 

    srand(time(NULL));

    int valores[ESCALA] = {0};
    int dados[LINHAS][3];

    inicia_valores(valores);
    pega_clausulas("instances/20.cnf", dados);
    for(int i = 0; i < LINHAS; i++) {
        for(int j = 0; j < 3; j ++) {
            printf(" %d ", dados[i][j]);
        }
        printf("\n");
    }

    double funcao_qualidade = calcula_qualidade(valores, dados);


    return 0;
}