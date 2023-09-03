
#include "gamecontroler.h"
#include "field.h"

gameControler::gameControler(int N, int M, int K)
{
    this->N_fieldSize = N;
    this->M_numberCount = M;
    this->K_numberOfTurns = K;
    this->_field = field(N);
}

void gameControler::openCell(int row, int col)
{
    this->_turnsMade++;
    open_result res = this->_field.openCell(row, col);
    if (res.hasNumber)
    {
        this->_sumOfOpenedCells += res.number;
        this->_countOpenNumberCells++;
    }
}

void gameControler::setNumberToCell(int row, int col, int number)
{
    this->_field.setNumberToCell(row, col, number);
}

int gameControler::getSumOfOpenedCells()
{
    return this->_sumOfOpenedCells;
}

int gameControler::getOpenNumberCellsCount()
{
    return this->_countOpenNumberCells;
}

bool gameControler::gameHasEnded()
{
    return this->_turnsMade >= this->K_numberOfTurns || this->M_numberCount == this->_countOpenNumberCells;
}
