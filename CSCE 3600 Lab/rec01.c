/*
Name:Andrew Yang
EUID:ay0123
csce3600 rec01.c
*/

#include <stdio.h>

int main()
{
    unsigned char x;
    int y;

    printf("Enter an ASCII character: ");
    scanf("%c", &x);
    printf("The ASCII value of %c is:\n", x);
    printf("    dec -- %d \n", x);
    printf("    hex -- %x \n", x);
    printf("    bin -- ");

        for (int i = 7; i >= 0; i--)
        {
                y = x >> i;
                if (y & 1)
                {
                printf("1");
                }
                else
                {
                printf("0");
                }
        }
        printf("\n");

    return 0;
}
