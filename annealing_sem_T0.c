#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define ESCALA 20 
#define LINHAS 91 
#define ITERACOES 5000
#define RODAGENS 10            // Ajustado para 10 rodagens (ideal para o boxplot)
#define FATOR_RESFRIAMENTO 5.0 // Fator 't' (admissível entre 1 e 5)
#define SAMAX 5

// Define o passo de gravação: a cada 1 iteração se ESCALA == 20, senão a cada 100
#if ESCALA == 20
    #define PASSO_GRAVACAO 1
#else
    #define PASSO_GRAVACAO 100
#endif

// FUNCAO OBJETIVO -> minimizar clausulas falsas!!
// Funcao de qualidade -> quantidade de clausulas falsas / total clausulas (linhas)
// Delta = qualidade_vizinho - qualidade_atual

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
        perror("Erro ao abrir o arquivo");
        exit(1);
    }

    char linha[256];

    while(i < LINHAS && fgets(linha, sizeof(linha), arquivo) != NULL) {
        if (linha[0] == 'c' || linha[0] == 'p' || linha[0] == '%' || linha[0] == '0' || linha[0] == '\n') {
            continue;
        }

        int v1, v2, v3;
        if(sscanf(linha, "%d %d %d", &v1, &v2, &v3) == 3) {
            dados[i][0] = v1;
            dados[i][1] = v2;
            dados[i][2] = v3;
            i++;
        }
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
        if(contador == 3){ 
            qtd_falsas++;
        }
    }

    return (double)qtd_falsas / (double)qtd_clausulas;
}

void atribui_vetor(int* vetor_receber, int* vetor_passar, int tam){
    for(int i = 0; i < tam; i++){
        vetor_receber[i] = vetor_passar[i];
    }
}

void gerar_vizinho(int* valores_vizinho){
    int n = ESCALA;
    int mudancas = 1;

    for(int i = 0; i < mudancas ; i++){
        int indice = rand() % n;
        valores_vizinho[indice] = 1 - valores_vizinho[indice];
    }
}

double annealing(int *valores, int dados[][3], int iteracoes, int rodagem, int samax){
    double delta = 0;
    int valores_atual[ESCALA] = {0};
    double t_corrente = 1.0;
    
    double it_max = (double)(iteracoes / samax);
    if (it_max < 1.0) it_max = 1.0;
    
    atribui_vetor(valores_atual, valores, ESCALA);
    int valores_vizinho[ESCALA] = {0};
    
    double qualidade_atual = calcula_qualidade(valores_atual, dados);
    double qualidade_melhor = qualidade_atual;
    double qualidade_vizinho = 0.0;
    
    FILE *arquivo_grafico = fopen("convergencia.csv", "a");
    if (arquivo_grafico == NULL) {
        printf("Erro ao abrir o ficheiro de convergência.\n");
        exit(1);
    }

    int i;
    for(i = 0; i < iteracoes; i++){
        
        if (i % samax == 0) {
            double iter = (double)(i / samax);

            t_corrente = pow((1.0 - (iter / it_max)), FATOR_RESFRIAMENTO);
            
            if (t_corrente < 1e-10) {
                t_corrente = 1e-10;
            }
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
                if(qualidade_melhor == 0.0) {
                    break;
                }
            }
        } else {
            double x = (double)rand() / RAND_MAX;
            if(x < exp(-delta / t_corrente)){
                atribui_vetor(valores_atual, valores_vizinho, ESCALA);
                qualidade_atual = qualidade_vizinho; 
            }
        }
        
        if (i % PASSO_GRAVACAO == 0) {
            fprintf(arquivo_grafico, "%d,%d,%lf,%lf\n", rodagem, i, qualidade_atual, qualidade_melhor);
        }
    }

    fprintf(arquivo_grafico, "%d,%d,%lf,%lf\n", rodagem, i, qualidade_atual, qualidade_melhor);
    fclose(arquivo_grafico);

    printf("Iterações da rodagem %d = %d\n", rodagem, i);
    printf("Quantidade de erradas da rodagem %d = %d\n", rodagem, (int)(qualidade_melhor * LINHAS));
    printf("Resultado da função objetivo (falsas/total): %lf\n", qualidade_melhor);

    return qualidade_melhor;
}

int main() { 
    srand(time(NULL));

    int valores[ESCALA] = {0};
    int dados[LINHAS][3];

    pega_clausulas("instances/20.cnf", dados);

    // Inicializa o arquivo de convergência limpo com o cabeçalho correto
    FILE *f_init = fopen("convergencia.csv", "w");
    if (f_init != NULL) {
        fprintf(f_init, "rodagem,iteracao,qualidade_atual,qualidade_melhor\n");
        fclose(f_init);
    }

    double media, desvio; 
    double somatorio_media = 0.0, somatorio_desvios = 0.0;
    int deu_certo = 0;
    double resultados[RODAGENS]; 

    for(int i = 0; i < RODAGENS; i++){
        printf("\n--------------RODAGEM %d--------------\n", i+1);
        inicia_valores(valores); 
        
        double melhor_qualidade = annealing(valores, dados, ITERACOES, (i+1), SAMAX);
        
        resultados[i] = melhor_qualidade;
        somatorio_media += melhor_qualidade; 
        
        if(melhor_qualidade == 0.0){
            deu_certo++;
        }
        printf("\n--------------FIM RODAGEM %d--------------\n", i+1);
    }

    // Salva o resumo das qualidades finais de cada rodagem para o Boxplot
    FILE *f_res = fopen("resultados_finais.csv", "w");
    if (f_res != NULL) {
        fprintf(f_res, "rodagem,qualidade_final\n");
        for(int i = 0; i < RODAGENS; i++){
            fprintf(f_res, "%d,%lf\n", i + 1, resultados[i]);
        }
        fclose(f_res);
    }

    media = somatorio_media / (double)RODAGENS;
    for(int i = 0; i < RODAGENS; i++){
        somatorio_desvios += pow(resultados[i] - media, 2.0);
    }
    desvio = sqrt(somatorio_desvios / (RODAGENS > 1 ? (RODAGENS - 1.0) : 1.0));

    printf("\nMedia da funcao qualidade nas %d rodagens: %lf\n", RODAGENS, media);
    printf("Desvio padrao nas rodagens: %lf\n", desvio);
    printf("Vezes que encontrou solucao perfeita: %d\n", deu_certo);

    return 0;
}