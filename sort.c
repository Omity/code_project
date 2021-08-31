#include <stdio.h>

void sort(int *arr, size_t size);
int main()
{
    int i, j;
    size_t size = 0;
    int array[] = {1, 5, 8, 7, 0, 9, 7, 10, 3};
    
    size = (int) sizeof(array) / sizeof(array[0]);
	
    sort(array, size);
	
    for(i = 0; i < size; ++i)
    {
	printf("%d ", array[i]);
    }

    printf("\n");
    return 0;
}

void sort(int *arr, size_t size)
{
	int i, j;
	int tmp = 0;
	for(i = 0; i < size - 1; ++i)
	{
		for(j = i + 1;j < size; ++j)
		{
			if(arr[j] > arr[i])
			{
				tmp = arr[i];
				arr[i] = arr[j];
				arr[j] = tmp;
			}
			else
			{
			}
		}
	}
}
