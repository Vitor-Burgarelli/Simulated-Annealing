#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define ESCALA 250 
#define LINHAS 1065
#define ITERACOES 5000000
#define NUM_VIZINHOS_T0 50 // Número de vizinhos a testar para calcular a T0
#define RODAGENS 5

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
    //int mudancas = ESCALA * (5.0/100.0);
    int mudancas = 1;

    if (mudancas < 1) {
        mudancas = 1;
    }

    for(int i = 0; i < mudancas ; i++){
        int indice = rand() % n;
        valores_vizinho[indice] = 1 - valores_vizinho[indice];
    }
}

double calcula_temperatura_inicial(int *valores_iniciais, int dados[][3], int num_vizinhos) {
    int valores_vizinho[ESCALA] = {0};
    
    // Avalia o custo da solução inicial como referência base do pior caso
    double pior_custo = calcula_qualidade(valores_iniciais, dados);
    double custo_vizinho = 0.0;

    for (int i = 0; i < num_vizinhos; i++) {
        atribui_vetor(valores_vizinho, valores_iniciais, ESCALA);
        gerar_vizinho(valores_vizinho);
        
        custo_vizinho = calcula_qualidade(valores_vizinho, dados);
        
        // Em problemas de minimização, o pior resultado é o maior valor de custo/erro
        if (custo_vizinho > pior_custo) {
            pior_custo = custo_vizinho;
        }
    }

    // Salvaguarda para evitar divisões por zero ou T0 nulo
    if (pior_custo <= 0.0001) {
        pior_custo = 1.0; 
    }

    return pior_custo;
}

void annealing(int *valores, int dados[][3], int iteracoes, int rodagem){
    double delta = 0;
    int valores_atual[ESCALA] = {0};
    double t_corrente = 1.0;
    double it_max = (double)iteracoes;
    
    atribui_vetor(valores_atual, valores, ESCALA);
    int valores_vizinho[ESCALA] = {0};
    
    double qualidade_atual = calcula_qualidade(valores_atual, dados);
    double qualidade_melhor = qualidade_atual;
    double qualidade_vizinho = 0.0;
    
    // Calcula a T0 usando a heurística de amostragem de vizinhança
    double T0 = calcula_temperatura_inicial(valores, dados, NUM_VIZINHOS_T0);
    double TN = 0.0001;    // Temperatura Final (Tn) requerida pelas fórmulas
    double N = it_max;
    
    printf("\nTemperatura inicial (T0) definida heuristicamente: %lf\n", T0);

    FILE *arquivo_grafico = fopen("convergencia_com_t0.csv", "w");
    if (arquivo_grafico == NULL) {
        printf("Erro ao criar o ficheiro de convergência.\n");
        exit(1);
    }
    fprintf(arquivo_grafico, "iteracao,qualidade_atual,qualidade_melhor\n");
    
    int i;
    for(i = 0; i < iteracoes; i++){
        
        double iter = (double)i;
        double A, B;

        /* ESCOLHA APENAS UMA FORMULA DESCOMENTANDO-A E COMENTANDO AS DEMAIS: */

        // Cooling Schedule 0
        //t_corrente = T0 - iter * ((T0 - TN) / N);

        // Cooling Schedule 1
        //t_corrente = T0 * pow((TN / T0), (iter / N));

        // Cooling Schedule 2
        //A = ((T0 - TN) * (N + 1.0)) / N;
        //B = T0 - A;
        //t_corrente = (A / (iter + 1.0)) + B;

        // Cooling Schedule 3
        //A = log(T0 - TN) / log(N); 
        //t_corrente = T0 - pow(iter, A);

        // Cooling Schedule 4 (Sigmoid)
        // t_corrente = ((T0 - TN) / (1.0 + exp(3.0 * (iter - N / 2.0)))) + TN;

        // Cooling Schedule 5
        //t_corrente = 0.5 * (T0 - TN) * (1.0 + cos((iter * 3.14159265358979323846) / N)) + TN;

        // Cooling Schedule 6
        // t_corrente = 0.5 * (T0 - TN) * (1.0 - tanh((10.0 * iter) / N - 5.0)) + TN;

        // Cooling Schedule 7
         t_corrente = ((T0 - TN) / cosh((10.0 * iter) / N)) + TN;

        // Cooling Schedule 8
        // A = (1.0 / N) * log(T0 / TN);
        // t_corrente = T0 * exp(-A * iter);

        // Cooling Schedule 9
        // A = (1.0 / (N * N)) * log(T0 / TN);
        // t_corrente = T0 * exp(-A * iter * iter);

        // Formula Original
        //t_corrente = T0 * pow((1.0 - (iter / N)), 5.0);
        
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
                if(qualidade_melhor == 0.0) {
                    break; // Solução perfeita encontrada
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

    printf("Vetor Final: [");
    for(int j = 0; j < ESCALA; j++){
        printf(" %d ", valores[j]);
    }
    printf("]\n");
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
    double funcao_qualidade;
    int deu_certo = 0;
    
    // Vetor para guardar a qualidade final de cada execução
    double resultados[RODAGENS]; 

    for(int i = 0; i < RODAGENS; i++){
        // 1. Gera um novo estado inicial aleatório para esta rodagem
        printf("\n--------------RODAGEM %d--------------\n", i+1);

        inicia_valores(valores); 
        
        // 2. Executa a otimização
        annealing(valores, dados, ITERACOES, (i+1));
        
        // 3. Avalia e guarda o resultado
        temp = calcula_qualidade(valores, dados);
        resultados[i] = temp;
        somatorio_media += temp; 
        
        if(temp == 0.0){
            deu_certo++;
        }

        printf("\n--------------FIM RODAGEM %d--------------\n", i+1);
    }

    // Calcula a média final
    media = somatorio_media / (double)RODAGENS;

    // Calcula o somatório do quadrado das diferenças (Variância)
    for(int i = 0; i < RODAGENS; i++){
        somatorio_desvios += pow(resultados[i] - media, 2.0);
    }

    // Calcula o Desvio Padrão Amostral (divide por N-1)
    desvio = sqrt(somatorio_desvios / (RODAGENS - 1.0));

    printf("Media da funcao qualidade nas %d rodagens: %lf\n", RODAGENS, media);
    printf("Desvio padrao nas rodagens: %lf\n", desvio);
    printf("Vezes que encontrou solucao perfeita: %d\n", deu_certo);

    //printf("\nResultado final:\n");
    /*for(int i = 0; i < ESCALA; i++) {
        printf(" %d ", valores[i]);
    }*/

    return 0;
}