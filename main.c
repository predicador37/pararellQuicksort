#include<mpi.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#define N 10

int main(int argc, char *argv[]) {
    int i, j, k, rank, size, pivote;
    int *lista, *sublista1, *sublista2;
    char worker_program[100];
    strcpy(worker_program, "./Debug/worker");
    lista = (int *) malloc(N * sizeof(int *));
    sublista1 = (int *) malloc(N * sizeof(int *));
    sublista2 = (int *) malloc(N * sizeof(int *));

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Status stat;
    MPI_Comm split;

    printf("Lista inicial:\n");
    for (i = 0; i < N; i++) {
        lista[i] = (rand() / (float) RAND_MAX) * 10;
        printf("%d\t", lista[i]);

    }
    printf("\n");
    pivote = lista[0];
    j = 0;
    k = 0;
    for (i = 1; i < N; i++) {
        if (lista[i] <= pivote) {
            sublista1[j] = lista[i];
            j++;
        }
        else {
            sublista2[k] = lista[i];
            k++;
        }
    }

    printf("[master] spawning child processes\n");
    MPI_Comm_spawn(worker_program, MPI_ARGV_NULL, 2, MPI_INFO_NULL, 0, MPI_COMM_SELF, &split,
                   MPI_ERRCODES_IGNORE);

    MPI_Send(&j, 1, MPI_INT, 0, 0, split);
    MPI_Send(sublista1, j, MPI_INT, 0, 0, split);

    MPI_Send(&k, 1, MPI_INT, 1, 0, split);
    MPI_Send(sublista2, k, MPI_INT, 1, 0, split);

    //Se recibe lista ordenada
    int *listaOrdenada, *listaOrdenada1, *listaOrdenada2;
    int sizeListaOrdenada, sizeListaOrdenada1, sizeListaOrdenada2;

    //tamaño de la lista ordenada a recibir
    MPI_Recv(&sizeListaOrdenada1, 1, MPI_INT, 0, 0, split, &stat);
    MPI_Recv(&sizeListaOrdenada2, 1, MPI_INT, 1, 0, split, &stat);

    sizeListaOrdenada = sizeListaOrdenada1 + 1 + sizeListaOrdenada2;

    listaOrdenada1 = (int *) malloc((sizeListaOrdenada1) * sizeof(int *));
    listaOrdenada2 = (int *) malloc((sizeListaOrdenada2) * sizeof(int *));
    listaOrdenada = (int *) malloc((sizeListaOrdenada) * sizeof(int *));

    MPI_Recv(listaOrdenada1, sizeListaOrdenada1, MPI_INT, 0, 0, split, &stat);
    MPI_Recv(listaOrdenada2, sizeListaOrdenada2, MPI_INT, 1, 0, split, &stat);

    // se confecciona nueva lista
    k = 0;
    i = 0;

    if (sizeListaOrdenada1 != 0 ) {
        for (i = 0; i < sizeListaOrdenada1; i++) {
            listaOrdenada[k] = listaOrdenada1[i];
            k++;
        }
    }
    listaOrdenada[i] = pivote;
    k++;
    if (sizeListaOrdenada2 != 0) {
        for (j = 0; j < sizeListaOrdenada2; j++) {
            listaOrdenada[k] = listaOrdenada2[j];
            k++;
        }
    }

    printf("Lista ordenada:\n");
    for (i = 0; i < sizeListaOrdenada; i++) {
        printf("%d\t", listaOrdenada[i]);

    }

    MPI_Finalize();
    return(0);
}