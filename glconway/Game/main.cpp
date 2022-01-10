#include <iostream>
#include <stdlib.h>
#include <Windows.h>
#include "conway.h"

void printTable(char** table, int n)
{
    for (int i = 0; i < n; i++)
    {
        printf("%s\n", table[i]);
    }

    printf("\n\n");
}

int main()
{
    std::cout << "Hello, world!\n" << std::endl;

#define X 30
#define Y 100

    conway c;
    conway_init(&c, 1, X, Y);

    // GLIDER GUN (X = 30, Y = 100)
    const char* seed[X] = {
        "                                                                                                    ",
        "                                                                                                    ",
        "                                                                                                    ",
        "                                                                                                    ",
        "                                                                                                    ",
        "                                                                                                    ",
        "                                                                                                    ",
        "                                                                                                    ",
        "                                                                                                    ",
        "                                1                                                                   ",
        "                              1 1                                                                   ",
        "                    11      11            11                                                        ",
        "                   1   1    11            11                                                        ",
        "        11        1     1   11                                                                      ",
        "        11        1   1 11    1 1                                                                   ",
        "                  1     1       1                                                                   ",
        "                   1   1                                                                            ",
        "                    11                                                                              ",
        "                                                                                                    ",
        "                                                                                                    ",
        "                                                                                                    ",
        "                                                                                                    ",
        "                                                                                                    ",
        "                                                                                                    ",
        "                                                                                                    ",
        "                                                                                                    ",
        "                                                                                                    ",
        "                                                                                                    ",
        "                                                                                                    ",
        "                                                                                                    " };
    conway_seedTable(&c, (char**)seed, ' ');

    int nr = 800;
    char** output = conway_print(&c, '0', ' ', NULL);
    printTable(output, c.x);
    //system("cls");

    while (nr--)
    {
        conway_simulate(&c);
        output = conway_print(&c, '0', ' ', output);
        //system("cls");
        printTable(output, c.x);
        Sleep(100);
    }

    for (int x = 0; x < c.x; x++)
    {
        free(output[x]);
    }
    free(output);

    conway_destroy(&c);

    return 0;
}