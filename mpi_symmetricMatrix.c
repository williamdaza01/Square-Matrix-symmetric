#include <stdio.h>
#include <time.h>
#include <mpi.h>
#include <stdlib.h>

MPI_Status status;

int main(int argc, char const *argv[])
{
    double start, stop;
    int N = atof(argv[1]);
    int i, j, a[N][N], b[N][N], temp, source, numtasks, taskid, numworkers, offset, dest, rows;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    numworkers = numtasks - 1;

    if (taskid == 0)
    {

        printf("\nEnter the dimension of the matrix: \n\n");
        scanf("%d", &N);

        printf("\nEnter the %d elements of the matrix: \n\n", N * N);
        for (i = 0; i < N; i++)     // to iterate the rows
            for (j = 0; j < N; j++) // to iterate the columns
                scanf("%d", &a[i][j]);

        for (i = 0; i < N; i++)     // to iterate the rows
            for (j = 0; j < N; j++) // to iterate the columns
                b[i][j] = a[i][j];

        start = MPI_Wtime();

        rows = N / numworkers;
        offset = 0;

        for (dest = 1; dest <= numworkers; dest++)
        {
            MPI_Send(&offset, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
            MPI_Send(&rows, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
            MPI_Send(&a[offset][0], rows * N, MPI_DOUBLE, dest, 1, MPI_COMM_WORLD);
            MPI_Send(&b, N * N, MPI_DOUBLE, dest, 1, MPI_COMM_WORLD);
            offset = offset + rows;
        }

        for (i = 1; i <= numworkers; i++)
        {
            source = i;
            MPI_Recv(&offset, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
            MPI_Recv(&rows, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
        }

        stop = MPI_Wtime();

        printf("\n\nThe original matrix is: \n\n");
        for (i = 0; i < N; i++) // to iterate the rows
        {
            for (j = 0; j < N; j++) // to iterate the columns
            {
                printf("%d\t", a[i][j]);
            }
            printf("\n");
        }

        printf("TOTAL time: %lf\n", stop - start);
    }

    if (taskid > 0)
    {
        source = 0;
        MPI_Recv(&offset, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&rows, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&a, rows * N, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&b, N * N, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, &status);

        printf("\n\nThe Transpose matrix is: \n\n");
        for (i = 0; i < N; i++) // to iterate the rows
        {
            for (j = 0; j < N; j++) // to iterate the columns
            {
                printf("%d\t", b[i][j]);
            }
            printf("\n");
        }

        for (i = 0; i < N; i++) // to iterate the rows
        {
            for (j = 0; j < N; j++) // to iterate the columns
            {
                if (a[i][j] != b[i][j])
                {
                    printf("\n\nMatrix is not Symmetric\n\n");
                    exit(0); // a system defined method to terminate the program
                }
            }
        }

        printf("\n\nMatrix is Symmetric\n\n");
        MPI_Send(&offset, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        MPI_Send(&rows, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
    }
    MPI_Finalize();
    return 0;
}