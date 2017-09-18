/* Author: David Akre
 * Date: 9/3/17
 * Description: Helper matrix math functions for hw1
 *
 */

#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <mpi.h>

// Global declaration max
double max = 0;

// Function to create a matrix
void create_matrix(unsigned int*** matrix, int row, int col, bool is_zero)
{
    // Fwd declaration
    unsigned int count = 0;

    // Create space for 2D array
    *matrix = (unsigned int **)malloc(row * sizeof(unsigned int *));

    for (int i = 0; i < row; i++)
    {
        (*matrix)[i] = (unsigned int *)malloc(col * sizeof(unsigned int));
    }

    // Forward declaration of count used in allocation
    // of 2D matrix
    if (is_zero != true) 
    {
        count = 1;
    }

    // Initialze 2D array with values
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            (*matrix)[i][j] = count++;
        }
    }
}

void free_matrix(unsigned int*** matrix, int row)
{
    // Free 2D array from heap
    for (int i = 0; i < row; i++)
    {
        free((*matrix)[i]);
    }

    free(matrix);
}

// Function to help multiple two matrices
void multiply_matrix(unsigned int*** matrix_one, unsigned int*** matrix_two, 
        unsigned int*** matrix_product, int lower_bounds, int upper_bounds, 
        int row, int col)
{
    for (int i = lower_bounds; i < upper_bounds; i++)
    {
        for (int j = 0; j < col; j++)
        {
            for (int k = 0; k < row; k++)
            {
                (*matrix_product)[i][j] += (*matrix_one)[i][k] * (*matrix_two)[k][j];
            }
        }
    }
}

// Function helper to perform square sum
double calc_square_sum(unsigned int*** matrix, int lower_bounds, int upper_bounds, 
        int row, int col)
{
    double square_sum = 0;
    int col_chunks = col / 3;
    int row_chunks = (upper_bounds - lower_bounds) / 3;

    for (int i = lower_bounds; i < upper_bounds; i++)
    {
        for (int j = 0; j < col; j++)
        {
            if ((*matrix)[i][j] > max) 
            {
                max = (*matrix)[i][j];
            }

            square_sum += (*matrix)[i][j] * (*matrix)[i][j];
        }
    }

    return square_sum;
}

// Function to help perform image filtering on matrix
void calc_matrix_filter(unsigned int*** matrix, int lower_bounds, int upper_bounds, int row, int col)
{
    // Get num chunks for 3x3 filter
    unsigned int square_sum = floor(sqrt(calc_square_sum(matrix, lower_bounds, upper_bounds, row, col) / max));
}
