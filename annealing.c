#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define ESCALA 20 
#define LINHAS 91
#define ITERACOES 200000

//FUNCAO OBJETIVO -> minimizar clausulas falsas!!
//Funcao de qualidade -> quantidade de clausulas falsas / total clausulas (linhas)
//Delta = qualidade_vizinho - qualidade_atual

void inicia_valores(int *valores){
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

    return (double)qtd_falsas/qtd_clausulas;
}

void atribui_vetor(int* vetor_receber, int* vetor_passar, int tam){
    for(int i = 0; i < tam; i++){
        vetor_receber[i] = vetor_passar[i];
    }
}

void gerar_vizinho(int* valores_vizinho){
    int n = ESCALA;
    int mudancas = ESCALA * (5.0/100.0);
    
    if (mudancas < 1) {
        mudancas = 1;
    }

    for(int i = 0; i < mudancas ; i++){
        int indice = rand() % n;
        valores_vizinho[indice] = 1 - valores_vizinho[indice];
    }
}

void annealing(int *valores, int dados[][3], int iteracoes){
    double delta = 0;
    int valores_atual[ESCALA] = {0};
    double t_corrente = 1.0;
    double it_max = (double)iteracoes;
    
    atribui_vetor(valores_atual, valores, ESCALA);
    int valores_vizinho[ESCALA] = {0};
    
    double qualidade_atual = calcula_qualidade(valores_atual, dados);
    double qualidade_melhor = qualidade_atual;
    double qualidade_vizinho = 0.0;
    
    // Abertura do ficheiro para registar os dados do gráfico
    FILE *arquivo_grafico = fopen("convergencia.csv", "w");
    if (arquivo_grafico == NULL) {
        printf("Erro ao criar o ficheiro de convergência.\n");
        exit(1);
    }
    // Cabeçalho do CSV
    fprintf(arquivo_grafico, "iteracao,qualidade_atual,qualidade_melhor\n");
    
    for(int i = 0; i < iteracoes; i++){
        t_corrente = pow((1.0 - ((double)i / it_max)), 1.0);
        
        if (t_corrente < 1e-10) {
            t_corrente = 1e-10;
        }

        atribui_vetor(valores_vizinho, valores_atual, ESCALA);
        gerar_vizinho(valores_vizinho);
        qualidade_vizinho = calcula_qualidade(valores_vizinho, dados);
        delta = qualidade_vizinho - qualidade_atual;

        if(delta < 0){
            atribui_vetor(valores_atual, valores_vizinho, ESCALA);
            qualidade_atual = qualidade_vizinho;

            if(qualidade_atual < qualidade_melhor){
                atribui_vetor(valores, valores_atual, ESCALA);
                qualidade_melhor = qualidade_atual;
            }
        } else {
            double x = (double)rand() / RAND_MAX;
            if(x < exp(-delta / t_corrente)){
                atribui_vetor(valores_atual, valores_vizinho, ESCALA);
                qualidade_atual = qualidade_vizinho; 
            }
        }
        
        // Regista os dados a cada 100 iterações para não sobrecarregar o I/O
        if (i % 100 == 0) {
            fprintf(arquivo_grafico, "%d,%lf,%lf\n", i, qualidade_atual, qualidade_melhor);
        }
    }
    
    // Regista a última iteração e fecha o ficheiro
    fprintf(arquivo_grafico, "%d,%lf,%lf\n", iteracoes, qualidade_atual, qualidade_melhor);
    fclose(arquivo_grafico);
    printf("Dados de convergência guardados em 'convergencia.csv'.\n");
}

int main() { 
    srand(time(NULL));

    int valores[ESCALA] = {0};
    int dados[LINHAS][3];

    inicia_valores(valores);
    pega_clausulas("instances/20.cnf", dados);

    double funcao_qualidade = calcula_qualidade(valores, dados);
    printf("Qualidade inicial: %lf\n", funcao_qualidade);

    annealing(valores, dados, ITERACOES);

    printf("Resultado final:\n");
    for(int i = 0; i < ESCALA; i++) {
        printf(" %d ", valores[i]);
    }
    
    funcao_qualidade = calcula_qualidade(valores, dados);
    printf("\nQualidade final = %lf\n", funcao_qualidade);

    return 0;
}