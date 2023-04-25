#include <stdio.h>
#include <stdlib.h>

int main(int argc, char * argv[]){
	printf("hello\n");

	int row_size = 2;
	int col_size = 2;

	int **int_array = malloc( sizeof(int) * row_size);

	for (int x = 0; x < row_size; x++){
		int_array[x] = (int *)malloc(sizeof(int) * col_size);
	}

	int_array[0][0] = 10;
	int_array[0][1] = 20;
	int_array[1][0] = 30;
	int_array[1][1] = 40;

	for (int j = 0; j < row_size; j ++){
		for( int n = 0; n < col_size; n ++){
			printf("ele %d\n",int_array[j][n]);
		}
	}

	int temp_col = col_size * 2;

	for (int m = 0; m < row_size; m++){
		int *temp_array = (int * )realloc(sizeof(int) * temp_col);
		for (int k = 0; k < col_size; k++){
			temp_array[k] = int_array[m][k]; 
		}

	}

	for (int i = 0; i < row_size; i++){
		free(int_array[i]);
	}
	free(int_array);

	return 0;
}