#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define ESCALA 20 

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

void pega_clausulas(){
    
}

int main() { 

    int vetor[ESCALA] = {0};

    inicia_vetor(vetor);
    pega_clausulas

    return 0;
}