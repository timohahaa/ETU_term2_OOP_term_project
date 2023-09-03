
#ifndef GAMECONTROLER_H
#define GAMECONTROLER_H

#include "field.h"

class gameControler
{
public:
    gameControler(int N, int M, int K);
    void openCell(int row, int col);
    void setNumberToCell(int row, int col, int number);
    int getSumOfOpenedCells();
    int getOpenNumberCellsCount();
    bool gameHasEnded();
private:
    int N_fieldSize;
    int M_numberCount;
    int K_numberOfTurns;
    int _sumOfOpenedCells = 0;
    int _countOpenNumberCells;
    int _turnsMade = 0;
    field _field;
};

#endif // GAMECONTROLER_H
