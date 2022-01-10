#include "conway.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int mod(int n, int d)
{
    while (n < 0)
    {
        n += d;
    }

    return n % d;
}

int conway_cell(conway *c, int x, int y)
{
    if (c->wrap)
    {
        x = mod(x, c->x);
        y = mod(y, c->y);
    }
    else
    {
        // check bounds
        if (x < 0 || x >= c->x ||
            y < 0 || y >= c->y)
        {
            return 0;
        }
    }

    return c->board[x * c->y + y];
}

void conway_init(conway *c, char wrap, int x, int y)
{
    if (!c)
    {
        return;
    }

    if (x < 0 || y < 0)
    {
        return;
    }

    c->wrap = wrap;
    c->x = x;
    c->y = y;
    c->board = (char*)malloc(x * y);
    memset(c->board, 0, x * y);
}

void conway_seed(conway *c, char *seed, char empty)
{
    for (int i = 0, n = c->x * c->y; i < n; i++)
    {
        c->board[i] = seed[i] != empty ? 1 : 0;
    }
}

void conway_seedTable(conway *c, char **seed, char empty)
{
    int i = 0;
    // c->x strings
    for (int x = 0; x < c->x; x++)
    {
        // each have c->y characters
        for (int y = 0; y < c->y; y++)
        {
            c->board[i] = seed[x][y] != empty ? 1 : 0;
            i++;
        }
    }
}

void conway_simulate(conway *c)
{
    char *tmp = (char*)malloc(c->x * c->y);

    int i = 0;
    for (int x = 0; x < c->x; x++)
    {
        for (int y = 0; y < c->y; y++)
        {
            int activeNeigbors = 0;
            for (int xi = -1; xi <= 1; xi++)
            {
                for (int yi = -1; yi <= 1; yi++)
                {
                    if ((xi || yi) && conway_cell(c, x + xi, y + yi))
                    {
                        activeNeigbors++;
                    }
                }
            }

            if (c->board[i] && (activeNeigbors == 2 || activeNeigbors == 3))
            {
                // live cells with 2 or 3 neighbors stay live
                tmp[i] = 1;
            }
            else if (!c->board[i] && activeNeigbors == 3)
            {
                // dead cells with 3 neighbors become live
                tmp[i] = 1;
            }
            else
            {
                // all others remain dead
                tmp[i] = 0;
            }

            i++;
        }
    }

    memcpy(c->board, tmp, c->x * c->y);
    free(tmp);
}

void conway_simulateN(conway *c, int n)
{
    while (n--)
    {
        conway_simulate(c);
    }
}

void conway_destroy(conway *c)
{
    if (c && c->board)
    {
        c->x = 0;
        c->y = 0;
        free(c->board);
    }
}

char **conway_print(conway *c, char live, char dead, char **ret)
{
    char allocate = 0;
    if (!ret)
    {
        ret = (char**)malloc(c->x * sizeof(char *));
        allocate = 1;
    }

    int i = 0;
    for (int x = 0; x < c->x; x++)
    {
        if (allocate)
        {
            ret[x] = (char*)malloc(c->y * sizeof(char) + 1);
        }

        for (int y = 0; y < c->y; y++)
        {
            ret[x][y] = c->board[i] ? live : dead;
            i++;
        }
        ret[x][c->y] = '\0';
    }

    return ret;
}