#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#define ESCALA 20
#define LINHAS 91
#define ITERACOES 200000
#define N_RUNS 30   // quantas execucoes independentes por valor de t

// FUNCAO OBJETIVO -> minimizar clausulas falsas!!
// Funcao de qualidade -> quantidade de clausulas falsas / total clausulas (linhas)
// T = (1 - it/itMax)^t   (t = fator de resfriamento, admissivel entre 1 e 5)

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

/*
 * Roda o SA uma unica vez.
 *
 * fator_resfriamento: o "t" da formula T = (1 - it/itMax)^t
 * qualidade_final_saida: se != NULL, recebe a qualidade_melhor ao final da run
 * nome_csv_detalhado: se != NULL, grava o csv detalhado (iteracao a iteracao,
 *                      a cada 100 passos) nesse arquivo. Usar so para runs
 *                      "ilustrativas" (ex: a primeira de cada t), senao vira
 *                      muito I/O para um lote grande.
 *
 * retorna: a iteracao em que qualidade_melhor chegou a 0.0 pela primeira vez,
 *          ou -1 se a run terminou sem convergir.
 */
int annealing(int *valores, int dados[][3], int iteracoes, double fator_resfriamento,
              double *qualidade_final_saida, const char *nome_csv_detalhado){
    double delta = 0;
    int valores_atual[ESCALA] = {0};
    double t_corrente = 1.0;
    double it_max = (double)iteracoes;
    int iteracao_convergencia = -1;

    atribui_vetor(valores_atual, valores, ESCALA);
    int valores_vizinho[ESCALA] = {0};

    double qualidade_atual = calcula_qualidade(valores_atual, dados);
    double qualidade_melhor = qualidade_atual;
    double qualidade_vizinho = 0.0;

    FILE *arquivo_grafico = NULL;
    if (nome_csv_detalhado != NULL) {
        arquivo_grafico = fopen(nome_csv_detalhado, "w");
        if (arquivo_grafico == NULL) {
            printf("Erro ao criar o ficheiro de convergencia.\n");
            exit(1);
        }
        fprintf(arquivo_grafico, "iteracao,qualidade_atual,qualidade_melhor\n");
    }

    // caso o estado inicial ja nasca satisfazendo tudo (raro, mas possivel)
    if (qualidade_melhor == 0.0) {
        iteracao_convergencia = 0;
    }

    for(int i = 0; i < iteracoes; i++){
        t_corrente = pow((1.0 - ((double)i / it_max)), fator_resfriamento);

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

                if (iteracao_convergencia == -1 && qualidade_melhor == 0.0) {
                    iteracao_convergencia = i + 1;
                }
            }
        } else {
            double x = (double)rand() / RAND_MAX;
            if(x < exp(-delta / t_corrente)){
                atribui_vetor(valores_atual, valores_vizinho, ESCALA);
                qualidade_atual = qualidade_vizinho;
            }
        }

        if (arquivo_grafico != NULL && i % 100 == 0) {
            fprintf(arquivo_grafico, "%d,%lf,%lf\n", i, qualidade_atual, qualidade_melhor);
        }
    }

    if (arquivo_grafico != NULL) {
        fprintf(arquivo_grafico, "%d,%lf,%lf\n", iteracoes, qualidade_atual, qualidade_melhor);
        fclose(arquivo_grafico);
    }

    if (qualidade_final_saida != NULL) {
        *qualidade_final_saida = qualidade_melhor;
    }

    return iteracao_convergencia;
}

int main() {
    srand(time(NULL));  // uma unica vez, fora dos loops de run

    int dados[LINHAS][3];
    pega_clausulas("instances/20.cnf", dados);

    FILE *resultados = fopen("resultados.csv", "w");
    if (resultados == NULL) {
        printf("Erro ao criar resultados.csv\n");
        exit(1);
    }
    fprintf(resultados, "t,run,iteracao_convergencia,sucesso,qualidade_final\n");

    double valores_t[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    int n_valores_t = sizeof(valores_t) / sizeof(valores_t[0]);

    for (int ti = 0; ti < n_valores_t; ti++) {
        double t = valores_t[ti];

        for (int run = 0; run < N_RUNS; run++) {
            int valores[ESCALA] = {0};
            inicia_valores(valores);

            // so grava o csv detalhado (para o grafico) na primeira run de cada t,
            // senao geraria N_RUNS * 5 arquivos
            char nome_csv[64];
            const char *csv_a_usar = NULL;
            if (run == 0) {
                snprintf(nome_csv, sizeof(nome_csv), "convergencia_t%d.csv", (int)t);
                csv_a_usar = nome_csv;
            }

            double qualidade_final;
            int iteracao_convergencia = annealing(valores, dados, ITERACOES, t,
                                                    &qualidade_final, csv_a_usar);

            int sucesso = (iteracao_convergencia != -1) ? 1 : 0;

            fprintf(resultados, "%.1f,%d,%d,%d,%lf\n",
                    t, run, iteracao_convergencia, sucesso, qualidade_final);

            printf("t=%.1f run=%d -> convergiu em %d (sucesso=%d), qualidade final=%lf\n",
                   t, run, iteracao_convergencia, sucesso, qualidade_final);
        }
    }

    fclose(resultados);
    printf("\nResultados agregados salvos em 'resultados.csv'.\n");
    printf("Graficos de uma run ilustrativa por t salvos em 'convergencia_t1.csv' .. 'convergencia_t5.csv'.\n");

    return 0;
}