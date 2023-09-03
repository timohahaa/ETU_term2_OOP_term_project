
#ifndef GAMECONTROLER_H
#define GAMECONTROLER_H

#include "field.h"

class gameControler
{
public:
    gameControler(int N, int M, int K);
    int getSumOfOpenedCells();
    bool gameHasEnded();
private:
    int N_fieldSize;
    int M_numberCount;
    int K_numberOfTurns;
    int _sumOfOpenedCells = 0;
    int _turnsMade = 0;
    field _field;
};

#endif // GAMECONTROLER_H
