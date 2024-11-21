#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>  // Biblioteca OpenMP

#define N 100000000  // Tamanho do vetor

// Função para medir o tempo decorrido
double get_elapsed_time(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
}

int main() {
    int *arr;
    long long soma_serial = 0, soma_parallel = 0;
    struct timeval start, end;

    // Alocar memória para o vetor
    arr = (int *)malloc(N * sizeof(int));
    if (arr == NULL) {
        fprintf(stderr, "Erro ao alocar memória!\n");
        return EXIT_FAILURE;
    }

    // Inicializar o vetor com números aleatórios entre 0 e 99
    srand(42);
    for (int i = 0; i < N; i++) {
        arr[i] = rand() % 100;
    }

    // Versão serializada (não paralela)
    gettimeofday(&start, NULL);
    for (int i = 0; i < N; i++) {
        soma_serial += arr[i];
    }
    gettimeofday(&end, NULL);
    double time_serial = get_elapsed_time(start, end);
    printf("Versão serial:\n");
    printf("Soma dos elementos do vetor: %lld\n", soma_serial);
    printf("Tempo de execução: %f segundos\n\n", time_serial);

    // Versão paralelizada com OpenMP
    for (int num_threads = 1; num_threads <= 8; num_threads *= 2) {  // Testar 1, 2, 4 e 8 threads
        soma_parallel = 0;  // Resetar a soma

        // Configurar o número de threads
        omp_set_num_threads(num_threads);

        gettimeofday(&start, NULL);
        #pragma omp parallel for reduction(+:soma_parallel)  // Paralelização do loop com redução na soma
        for (int i = 0; i < N; i++) {
            soma_parallel += arr[i];
        }
        gettimeofday(&end, NULL);
        double time_parallel = get_elapsed_time(start, end);

        // Imprimir resultados para a execução paralela
        printf("Versão paralela com %d threads:\n", num_threads);
        printf("Soma dos elementos do vetor: %lld\n", soma_parallel);
        printf("Tempo de execução: %f segundos\n\n", time_parallel);

        // Comparação com a versão serial
        printf("Speedup (Serial / Paralelo): %.2fx\n\n", time_serial / time_parallel);
    }

    // Liberar a memória alocada
    free(arr);

    return 0;
}
