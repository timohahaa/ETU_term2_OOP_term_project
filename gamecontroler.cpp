
#include "gamecontroler.h"
#include "field.h"

gameControler::gameControler(int N, int M, int K)
{
    this->N_fieldSize = N;
    this->M_numberCount = M;
    this->K_numberOfTurns = K;
    this->_field = field(N);
}

int gameControler::getSumOfOpenedCells()
{
    return this->_sumOfOpenedCells;
}

bool gameControler::gameHasEnded()
{
    return this->_turnsMade >= this->K_numberOfTurns || this->M_numberCount == this->_field.getOpenNumberCellCount();
}
