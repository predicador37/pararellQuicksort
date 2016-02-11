#include<mpi.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>


int main(int argc, char *argv[]) {
    int i, j, k, rank, size, sizeList, pivote;
    int *lista, *sublista1, *sublista2;
    MPI_Init(&argc, &argv);
    char worker_program[100];
    strcpy(worker_program, "./Debug/worker");
    MPI_Comm padre, split;
    MPI_Status stat;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_get_parent(&padre);

    printf("[process %d] starting child \n", rank);
    //Se recibe tamaño del array que se va a recibir
    MPI_Recv(&sizeList, 1, MPI_INT, 0, 0, padre, &stat);
    lista = (int *) malloc(sizeList * sizeof(int *));
    //Se recibe lista a ordenar
    MPI_Recv(lista, sizeList, MPI_INT, 0, 0, padre, &stat);

    if (sizeList == 1) { // nodo hoja
        // se devuelve al padre tal cual
        printf("[process %d] Leaf node\n", rank);
        printf("[process %d] value: %d\n", rank, lista[0]);
        MPI_Send(&sizeList, 1, MPI_INT, 0, 0, padre);
        MPI_Send(lista, sizeList, MPI_INT, 0, 0, padre);
    }
    else if (sizeList == 0) { // proceso muerto
        MPI_Send(&sizeList, 1, MPI_INT, 0, 0, padre);
    }

    else {

        sublista1 = (int *) malloc(sizeList * sizeof(int *));
        sublista2 = (int *) malloc(sizeList * sizeof(int *));

        printf("[process %d] Lista inicial:\n", rank);
        for (i = 0; i < sizeList; i++) {
            printf("%d\t", lista[i]);

        }
        printf("\n");
        pivote = lista[0];
        j = 0;
        k = 0;
        for (i = 1; i < sizeList; i++) {
            if (lista[i] <= pivote) {
                sublista1[j] = lista[i];
                j++;
            }
            else {
                sublista2[k] = lista[i];
                k++;
            }
        }


        MPI_Comm_spawn(worker_program, MPI_ARGV_NULL, 2, MPI_INFO_NULL, 0, MPI_COMM_SELF, &split,
                       MPI_ERRCODES_IGNORE);

        MPI_Send(&j, 1, MPI_INT, 0, 0, split);
        MPI_Send(sublista1, j, MPI_INT, 0, 0, split);

        MPI_Send(&k, 1, MPI_INT, 1, 0, split);
        MPI_Send(sublista2, k, MPI_INT, 1, 0, split);

        //Se recibe lista ordenada
        int *listaOrdenada, *listaOrdenada1, *listaOrdenada2;
        int sizeListaOrdenada, sizeListaOrdenada1, sizeListaOrdenada2;

        printf("[proceso %d] Receiving sorted list...\n", rank);
        //tamaño de la lista ordenada a recibir
        MPI_Recv(&sizeListaOrdenada1, 1, MPI_INT, 0, 0, split, &stat);
        MPI_Recv(&sizeListaOrdenada2, 1, MPI_INT, 1, 0, split, &stat);


        sizeListaOrdenada = sizeListaOrdenada1 + 1 + sizeListaOrdenada2;

        if (sizeListaOrdenada1 != 0 ) {
            listaOrdenada1 = (int *) malloc((sizeListaOrdenada1) * sizeof(int *));
        }
        if (sizeListaOrdenada2 != 0) {
            listaOrdenada2 = (int *) malloc((sizeListaOrdenada2) * sizeof(int *));
        }

        listaOrdenada = (int *) malloc((sizeListaOrdenada) * sizeof(int *));

        if (sizeListaOrdenada1 != 0 ) {
            MPI_Recv(listaOrdenada1, sizeListaOrdenada1, MPI_INT, 0, 0, split, &stat);
        }

        if (sizeListaOrdenada2 != 0) {
            MPI_Recv(listaOrdenada2, sizeListaOrdenada2, MPI_INT, 1, 0, split, &stat);
        }

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
        printf("\n");

        // se envía la lista mezclada al padre
        MPI_Send(&sizeListaOrdenada, 1, MPI_INT, 0, 0, padre);
        MPI_Send(listaOrdenada, sizeListaOrdenada, MPI_INT, 0, 0, padre);

    }
    MPI_Finalize();
    return (0);
}