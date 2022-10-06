/*
 Andrew Yang
 ay0123
 rec02.c
 */
#include<stdio.h>
#include<stdlib.h>
int main()
{
    int *int_ptr = (int *)malloc(sizeof(int)*2);
    
    printf("Enter first integer: ");
    scanf("%d",int_ptr);
    printf("Enter second integer: ");
    scanf("%d",int_ptr+1);
    printf("Original values: 1st =  %d 2nd =  %d\n",int_ptr[0],int_ptr[1]);
    
    int_ptr[0] = int_ptr[0]^int_ptr[1];
    int_ptr[1] = int_ptr[0]^int_ptr[1];
    int_ptr[0] = int_ptr[0]^int_ptr[1];
    
    printf("Swapped values: 1st =  %d 2nd =  %d\n",int_ptr[0],int_ptr[1]);
    
    free(int_ptr);
   
    return 0;
}
