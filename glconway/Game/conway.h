#ifndef CONWAY_H
#define CONWAY_H

int mod(int n, int d);

typedef struct
{
    int x;
    int y;

    char wrap;

    char *board;
} conway;

int conway_cell(conway *c, int x, int y);

void conway_init(conway *c, char wrap, int x, int y);
void conway_seed(conway *c, char *seed, char empty);
void conway_seedTable(conway *c, char **seed, char empty);

void conway_simulate(conway *c);
void conway_simulateN(conway *c, int n);

void conway_destroy(conway *c);

char **conway_print(conway *c, char live, char dead, char **ret);

#endif // CONWAY_H