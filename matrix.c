/* Author: David Akre
 * Date: 9/3/17
 * Description: Part 1 of hw1
 * - Apply a simple matrix multiplication and test the two 
 *   matrices by running it serially first followed by
 *   parallelized mechanism. Compare the total execution
 *   times for the processors scaling by 2 from 4 to 64.
 *   Additionally, profile the code to show the computing
 *   to communication ratios. 
 *
 * Description: Part 2 of h1
 * - Apply a filter on an image using the window size 3x3
 *   for a matrix size 256x256.
 *
 * Usage:
 * - Run ./matrix with the following argument options
 *   - arg: 1 run part 1 
 *   - arg: 2 run part 2 
 *   - arg: 3 run part 1 in parallel
 *   - arg: 4 run part 2 in parallel
 */

#include <matrix_ops.h>

// Main entry point for the simple matrix multiplication
// algorithm 
int main (int argc, char *argv[])
{
    // Forward declaration of matrix variables
    int upper_bounds, lower_bounds;
    int row_one = 511;
    int col_one = 511;
    int row_two = 255;
    int col_two = 255;
    unsigned int** matrix_one; 
    unsigned int** matrix_two;
    unsigned int** matrix_product_one;
    unsigned int** matrix_product_two;

    // Forward declaration of mpi variables
    MPI_Status status;
    MPI_Request request;
    double start_total, finish_total, start, finish;
    int size, rank;

    // Forward initialization of all matrices used
    create_matrix(matrix_one, row_one, col_one);
    create_matrix(matrix_two, row_two, col_two);
    create_matrix(row_one, col_one, matrix_product_one);
    create_matrix(row_two, col_two, matrix_product_two);

    // Initialize MPI execution environment
    MPI_Init(&argc, &argv);

    // Determine rank and size of calling process in the communicator
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argv[1] == NULL) 
    {
        printf("ERROR: No input argument provided\n");
        exit(-1);
    }

    if (strcmp(argv[1], "1") == 0 || strcmp(argv[1], "3") == 0) 
    {
        if (strcmp(argv[1], "3") == 0)
        {
            // Initially broadcast matrices to multiply
            MPI_Bcast(*matrix_one, row*col, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
            MPI_Bcast(*matrix_two, row*col, MPI_UNSIGNED, 0, MPI_COMM_WORLD);

            // If master process, initialize matrices and publish them 
            // to worker nodes
            // Else if worker nodes, wait till matrices have been published
            // then begin processing matrices in multiply function
            if (rank == 0)
            {
                for (int i = 0; i < (size - 1); i++)
                {
                    lower_bounds = (row / (size - 1)) * i;

                    if ((i + 1) == (size - 1))
                    {
                        upper_bounds = row;
                    }
                    else
                    {
                        upper_bounds = lower_bounds + (row / (size - 1));
                    }
                    
                    MPI_Isend(&lower_bounds, 1, MPI_INT, i+1, 1, MPI_COMM_WORLD, &request);
                    MPI_Isend(&upper_bounds, 1, MPI_INT, i+1, 1, MPI_COMM_WORLD, &request);
                }

                start_total = MPI_Wtime();

                for (int i = 1; i < size; i++)
                {
                    MPI_Recv(&lower_bounds, 1, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
                    MPI_Recv(&upper_bounds, 1, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
                    MPI_Recv(&matrix_product[lower_bounds][0], (upper_bounds - lower_bounds) * col, 
                            MPI_UNSIGNED, i, 2, MPI_COMM_WORLD, &status);
                    MPI_Recv(&start, 1, MPI_DOUBLE, i, 2, MPI_COMM_WORLD, &status);
                    MPI_Recv(&finish, 1, MPI_DOUBLE, i, 2, MPI_COMM_WORLD, &status);
                }

                finish_total = MPI_Wtime();

                printf("Total computation time %.6f seconds\n", (finish - start));
                printf("Total communication time %.6f seconds\n", 
                        (finish_total - start_total) - (finish - start));
            }
            else
            {
                MPI_Recv(&lower_bounds, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
                MPI_Recv(&upper_bounds, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);

                start = MPI_Wtime();
                matrix_product = multiply_matrix(matrix_one, matrix_two, matrix_product,
                        lower_bounds, upper_bounds);
                finish = MPI_Wtime();

                MPI_Isend(&lower_bounds, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &request);
                MPI_Isend(&upper_bounds, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &request);
                MPI_Isend(&matrix_product[lower_bounds][0], (upper_bounds - lower_bounds) * col, 
                        MPI_UNSIGNED, 0, 2, MPI_COMM_WORLD, &request);
                MPI_Isend(&start, 1, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD, &request);
                MPI_Isend(&finish, 1, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD, &request); 
            }
        }
        else
        {
            start = MPI_Wtime();
            matrix_product = multiply_matrix(matrix_one, matrix_two, matrix_product, 0, row);
            finish = MPI_Wtime();

            printf("Total computation time %.4f on process %d\n", 
                    (finish - start), rank);
        }
    }
    else if (strcmp(argv[1], "2") == 0 || strcmp(argv[1], "4") == 0)
    {
        if (strcmp(argv[1], "4") == 0)
        {
            MPI_Bcast(*matrix_three, row*col, MPI_UNSIGNED, 0, MPI_COMM_WORLD);

            if (rank == 0)
            {
                start_total = MPI_Wtime();

                for (int i = 0; i < (size - 1); i++)
                {
                    lower_bounds = (row / (size - 1)) * i;
                    if ((i + 1) == (size - 1))
                    {
                        upper_bounds = row;
                    }
                    else
                    {
                        upper_bounds = lower_bounds + (row / (size - 1));
                    }
                   
                    MPI_Isend(&lower_bounds, 1, MPI_INT, i+1, 1, MPI_COMM_WORLD, &request);
                    MPI_Isend(&upper_bounds, 1, MPI_INT, i+1, 1, MPI_COMM_WORLD, &request);
                }

                for (int i = 1; i < size; i++)
                {
                    MPI_Recv(&lower_bounds, 1, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
                    MPI_Recv(&upper_bounds, 1, MPI_INT, i, 2, MPI_COMM_WORLD, &status);

                    // TODO: Recv partial image filter from prc here
                    
                    MPI_Recv(&start, 1, MPI_DOUBLE, i, 2, MPI_COMM_WORLD, &status);
                    MPI_Recv(&finish, 1, MPI_DOUBLE, i, 2, MPI_COMM_WORLD, &status);
                }

                finish_total = MPI_Wtime();

                printf("Total computation time %.6f seconds\n", (finish - start));
                printf("Total communication time %.6f seconds\n", 
                        (finish_total - start_total) - (finish - start));
            }
            else
            {
                MPI_Recv(&lower_bounds, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
                MPI_Recv(&upper_bounds, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);

                start = MPI_Wtime();

                // TODO: Perform image filter proc in parallel

                finish = MPI_Wtime();

                MPI_Isend(&lower_bounds, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &request);
                MPI_Isend(&upper_bounds, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &request);
                MPI_Isend(&sum, 1, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD, &request);
                MPI_Isend(&max, 1, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD, &request);
                MPI_Isend(&start, 1, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD, &request);
                MPI_Isend(&finish, 1, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD, &request); 
            }
        }
        else
        {
            start = MPI_Wtime();

            // TODO: Perform image filter proc

            finish = MPI_Wtime();

            printf("Total computation time %.4f on process %d\n", 
                    (finish - start), rank);
        }
    }
    else
    {
        printf("ERROR: Invalid input argument %s\n", argv[1]);
        exit(-1);
    }

    // Terminate MPI execution environment
    MPI_Finalize();

    exit(0);
}
