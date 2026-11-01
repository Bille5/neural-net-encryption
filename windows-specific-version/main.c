#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "trng.h"
#include <time.h>

#define SIZE 16

#define LEARNING_RATE 0.3

double input[SIZE];

double hidden[SIZE];

double output[SIZE];

double input_hidden_weights[SIZE][SIZE];

double hidden_output_weights[SIZE][SIZE];

void random_weights()
{
	for (int x = 0; x < SIZE; x++)
	{
		for (int y = 0; y < SIZE; y++)
		{
			input_hidden_weights[x][y] = ((((double)(trng_u32())) / 4294967296) * 4) - 2;
			hidden_output_weights[x][y] = ((((double)(trng_u32())) / 4294967296) * 4) - 2;
		}
	}
}


void matrix_multiply(double weights[SIZE][SIZE], double vector[SIZE], double result[SIZE])
{
	for (int i = 0; i < SIZE; i++)
	{
		result[i] = 0;
		for (int j = 0; j < SIZE; j++)
		{
			result[i] += weights[i][j] * vector[j];
		}
	}
}

void atan_layer(double vector[SIZE])
{
	for (int x = 0; x < SIZE; x++)
	{
		vector[x] = atan(vector[x]);
	}
}

void tan_layer(double vector[SIZE])
{
	for (int x = 0; x < SIZE; x++)
	{
		vector[x] = tan(vector[x]);
	}
}

void forward_pass()
{
	atan_layer(input);
	matrix_multiply(input_hidden_weights, input, hidden);
	atan_layer(hidden);
	matrix_multiply(hidden_output_weights, hidden, output);
	atan_layer(output);
}

void invert_matrix(double A[SIZE][SIZE], double A_inv[SIZE][SIZE])
{
	double aug[SIZE][2 * SIZE];

	// Create augmented matrix [A | I]
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			aug[i][j] = A[i][j];
			aug[i][j + SIZE] = (i == j) ? 1.0 : 0.0;
		}
	}

	// Gauss-Jordan elimination
	for (int i = 0; i < SIZE; i++) {
		double pivot = aug[i][i];

		// Normalize the pivot row
		for (int j = 0; j < 2 * SIZE; j++)
			aug[i][j] /= pivot;

		// Eliminate other rows
		for (int k = 0; k < SIZE; k++) {
			if (k == i) continue;
			double factor = aug[k][i];
			for (int j = 0; j < 2 * SIZE; j++)
				aug[k][j] -= factor * aug[i][j];
		}
	}

	// Extract inverse from augmented matrix
	for (int i = 0; i < SIZE; i++)
		for (int j = 0; j < SIZE; j++)
			A_inv[i][j] = aug[i][j + SIZE];

}

void inverse()
{
	double input_hidden_weights_inv[SIZE][SIZE];
	double hidden_output_weights_inv[SIZE][SIZE];
	

	invert_matrix(input_hidden_weights, input_hidden_weights_inv);
	invert_matrix(hidden_output_weights, hidden_output_weights_inv);

	tan_layer(output);
	matrix_multiply(hidden_output_weights_inv, output, hidden);

	tan_layer(hidden);
	matrix_multiply(input_hidden_weights_inv, hidden, input);

	tan_layer(input);
}

void nn_encrypt(char *string, double *result)
{
	for (int x = 0; x < SIZE; x++)
	{
		if (string[x] == NULL)
		{
			input[x] = (double)(trng_u32() % 260);
		}
		else
		{
			input[x] = (double)string[x];
		}
		
        
	}

	forward_pass();

	for (int x = 0; x < SIZE; x++)
	{
		result[x] = output[x];
	}
}

void nn_decrypt(double* encrypted_string, char* decrpyted_string)
{
	for (int x = 0; x < SIZE; x++)
	{
		output[x] = encrypted_string[x];
	}
	inverse();
	for (int x = 0; x < SIZE; x++)
	{
		decrpyted_string[x] = (char)round(input[x]);
	}
}

void initialize_encrypt()
{
	random_weights();
}

void char_string_print(char* my_string)
{
	for (int x = 0; x < SIZE; x++)
	{
		printf("%c", my_string[x]);
	}
	printf("\n\n\n");
}

void double_string_print(double* my_string)
{
	for (int x = 0; x < SIZE; x++)
	{
		printf("%.5lf ", my_string[x]);
	}
	printf("\n\n\n");
}

void backprop_random()
{
	int x;
	int y;
	double correct[SIZE];
	double error[SIZE];
	double output_deltas[SIZE];
	double hidden_delta[SIZE];

	for (x = 0; x < SIZE; x++)
	{
		correct[x] = ((((double)(trng_u32())) / 4294967296) * 4) - 2;
		error[x] = correct[x] - output[x];
		output_deltas[x] = error[x] * (1 / (1 + output[x] * output[x]));
	}
	for (x = 0; x < SIZE; x++)
	{
		for (y = 0; y < SIZE; y++)
		{
			hidden_output_weights[x][y] += output_deltas[x] * hidden[y] * LEARNING_RATE;

			hidden_output_weights[x][y] -= hidden_output_weights[x][y] * 0.01;

			if (fabs(hidden_output_weights[x][y]) > 1.2)
			{
				hidden_output_weights[x][y] = 0.2 * cbrt(hidden_output_weights[x][y]);
				//hidden_output_weights[x][y] -= hidden_output_weights[x][y] * 0.01;
			}
		}
	}
	for (x = 0; x < SIZE; x++)
	{
		hidden_delta[x] = 0;
		for (y = 0; y < SIZE; y++)
		{
			hidden_delta[x] += output_deltas[y] * hidden_output_weights[y][x];
		}
		hidden_delta[x] *= (1 / (1 + hidden[x] * hidden[x]));
	}
	for (x = 0; x < SIZE; x++)
	{
		for (y = 0; y < SIZE; y++)
		{
			input_hidden_weights[x][y] += hidden_delta[x] * input[y] * LEARNING_RATE;
			input_hidden_weights[x][y] -= input_hidden_weights[x][y] * 0.01;
			if (fabs(input_hidden_weights[x][y]) > 1.2)
			{
				input_hidden_weights[x][y] = 0.2 * cbrt(input_hidden_weights[x][y]);
				//input_hidden_weights[x][y] -= input_hidden_weights[x][y] * 0.01;
			}
		}
	}

	

}

int main()
{
	char my_string[SIZE] = "MynamsBill";
	char other_string[SIZE] = "My name is bill";
	double result1[SIZE];
	double result2[SIZE];
	char decrypted_string1[SIZE];
	char decrypted_string2[SIZE];
	clock_t start;
	clock_t end;
	double duration;
	int x;

	initialize_encrypt();

	start = clock();

	for (x = 0; x < 100000; x++)
	{
		

		

		printf("Original:");
		char_string_print(my_string);

		printf("\n\nEncrypted:");
		nn_encrypt(my_string, result1);
		double_string_print(result1);


		nn_decrypt(result1, decrypted_string1);
		printf("\nDecrypted:");
		char_string_print(decrypted_string1);


		backprop_random();



		/*printf("\n\nEncrypted 2:");
		nn_encrypt(other_string, result2);
		double_string_print(result2);
		nn_decrypt(result2, decrypted_string2);

		printf("Decrypted:");
		
		
		char_string_print(decrypted_string2);

		backprop_random();*/
	}

	end = clock();

	duration = ((double)(end - start) / CLOCKS_PER_SEC) / x;

	//printf("%.3lf   ", (duration * 1000));

}