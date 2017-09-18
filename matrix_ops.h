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

// Function to help perform mean image filtering on matrix
void calc_matrix_filter(unsigned int*** matrix, int lower_bounds, int upper_bounds, int row, int col)
{
    double square_sum = 0;
    int col_chunks = col / 3;
    int low_col = 0;
    int high_col = 3;
    int row_chunks = (upper_bounds - lower_bounds) / 3;
    int new_upper_bounds = 3 + lower_bounds;
    int new_lower_bounds = lower_bounds;

    while (new_lower_bounds < upper_bounds-1)
    {
        while (low_col < col) 
        {
            for (int i = new_lower_bounds; i < new_upper_bounds; i++)
            {
                for (int j = low_col; j < high_col; j++)
                {
                    if ((*matrix)[i][j] > max) 
                    {
                        max = (*matrix)[i][j];
                    }

                    square_sum += (*matrix)[i][j] * (*matrix)[i][j];
                }
            }

            low_col = high_col;
            high_col += 3;

            (*matrix)[new_upper_bounds/2][high_col/2] = floor(sqrt(square_sum / max));
        }

        low_col = 0;
        high_col = 3;

        new_lower_bounds = new_upper_bounds;
        new_upper_bounds += 3;
    }
}
