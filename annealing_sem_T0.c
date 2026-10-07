#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define ESCALA 250 
#define LINHAS 1065 
#define ITERACOES 5000000
#define RODAGENS 10
#define FATOR_RESFRIAMENTO 5.0 // Fator 't' (admissível entre 1 e 5)

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
        perror("Erro ao abrir o arquivo");
        exit(1);
    }

    char linha[256];

    // Lê garantindo que não ultrapasse a capacidade de 'LINHAS'
    while(i < LINHAS && fgets(linha, sizeof(linha), arquivo) != NULL) {
        // Ignora comentários ('c') e linha de cabeçalho do formato DIMACS ('p')
        if (linha[0] == 'c' || linha[0] == 'p' || linha[0] == '%' || linha[0] == '0' || linha[0] == '\n') {
            continue;
        }

        int v1, v2, v3;
        // Lê os 3 números da cláusula
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
    int mudancas = 1;

    if (mudancas < 1) {
        mudancas = 1;
    }

    for(int i = 0; i < mudancas ; i++){
        int indice = rand() % n;
        valores_vizinho[indice] = 1 - valores_vizinho[indice];
    }
}

// samax inserido como parâmetro na função
void annealing(int *valores, int dados[][3], int iteracoes, int rodagem, int samax){
    double delta = 0;
    int valores_atual[ESCALA] = {0};
    double t_corrente = 1.0;
    
    // Calcula-se os estágios máximos de temperatura com base em samax
    double it_max = (double)(iteracoes / samax);
    if (it_max < 1.0) it_max = 1.0;
    
    atribui_vetor(valores_atual, valores, ESCALA);
    int valores_vizinho[ESCALA] = {0};
    
    double qualidade_atual = calcula_qualidade(valores_atual, dados);
    double qualidade_melhor = qualidade_atual;
    double qualidade_vizinho = 0.0;
    
    FILE *arquivo_grafico = fopen("convergencia_sem_t0.csv", "w");
    if (arquivo_grafico == NULL) {
        printf("Erro ao criar o ficheiro de convergência.\n");
        exit(1);
    }
    fprintf(arquivo_grafico, "iteracao,qualidade_atual,qualidade_melhor\n");
    
    int i;
    for(i = 0; i < iteracoes; i++){
        
        // A temperatura decai em blocos para permitir equilíbrio térmico
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
        
        if (i % 100 == 0) {
            fprintf(arquivo_grafico, "%d,%lf,%lf\n", i, qualidade_atual, qualidade_melhor);
        }
    }
    
    fprintf(arquivo_grafico, "%d,%lf,%lf\n", i, qualidade_atual, qualidade_melhor);
    fclose(arquivo_grafico);

    printf("Iterações da rodagem %d = %d\n", rodagem, i);
    printf("Quantidade de erradas da rodagem %d = %d\n", rodagem, (int)(qualidade_melhor * LINHAS));
    printf("Resultado da função objetivo (falsas/total): %lf\n", qualidade_melhor);
}

int main() { 
    srand(time(NULL));

    int valores[ESCALA] = {0};
    int dados[LINHAS][3];

    pega_clausulas("instances/250.cnf", dados);

    double media;
    double desvio; 
    double somatorio_media = 0.0;
    double somatorio_desvios = 0.0;
    double temp; 
    int deu_certo = 0;
    
    double resultados[RODAGENS]; 

    for(int i = 0; i < RODAGENS; i++){
        printf("\n--------------RODAGEM %d--------------\n", i+1);

        inicia_valores(valores); 
        
        annealing(valores, dados, ITERACOES, (i+1));
        
        temp = calcula_qualidade(valores, dados);
        resultados[i] = temp;
        somatorio_media += temp; 
        
        if(temp == 0.0){
            deu_certo++;
        }

        printf("\n--------------FIM RODAGEM %d--------------\n", i+1);
    }

    media = somatorio_media / (double)RODAGENS;

    for(int i = 0; i < RODAGENS; i++){
        somatorio_desvios += pow(resultados[i] - media, 2.0);
    }

    desvio = sqrt(somatorio_desvios / (RODAGENS - 1.0));

    printf("Media da funcao qualidade nas %d rodagens: %lf\n", RODAGENS, media);
    printf("Desvio padrao nas rodagens: %lf\n", desvio);
    printf("Vezes que encontrou solucao perfeita: %d\n", deu_certo);

    return 0;
}