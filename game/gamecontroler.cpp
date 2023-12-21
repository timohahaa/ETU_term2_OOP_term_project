
#include "gamecontroler.h"
#include "field.h"

gameControler::gameControler(int N, int M, int K)
{
    this->N_fieldSize = N;
    this->M_numberCount = M;
    this->K_numberOfTurns = K;
    this->field1 = field(N);
    this->field2 = field(N);
}

TurnResult gameControler::openCell(int row, int col)
{
    this->turnsMade++;
    TurnResult result;
//    //open_result opres = this->_field.openCell(row, col);
//    result res;
//    res.hasNumber = opres.hasNumber;
//    res.number = opres.number;
//    res.gameEnded = this->gameHasEnded();
//    if (res.hasNumber)
//    {
//        this->_sumOfOpenedCells += res.number;
//        this->_countOpenNumberCells++;
//    }
    return result;
}

int gameControler::getScores(Players player)
{
    if (player){
        return scores2;
    }
    else{
        return scores1;
    }
}

int gameControler::getN_fieldSize() const
{
    return N_fieldSize;
}

int gameControler::getM_numberCount() const
{
    return M_numberCount;
}

int gameControler::getK_numberOfTurns() const
{
    return K_numberOfTurns;
}

SetFieldResult gameControler::set_field(QVector<QVector<int>> field_ind, Players player)
{


    auto result = SetFieldResult();
    if (field_ind.size()!=this->N_fieldSize){
        result.field_size_err = true;
        return result;
    }
    foreach (auto row, field_ind) {
        if (row.size()!=this->N_fieldSize){
            result.field_size_err = true;
            return result;
        }
    }

    field* curfield = nullptr;
    if (player){
        curfield = &this->field2;
    }
    else{
        curfield = &this->field1;
    }

    QVector<int> various_digits;
    for (int i=1; i<=M_numberCount; i++){
        various_digits.append(i);
    }

    for (int i=0; i<N_fieldSize; i++){
        for (int j=0; j<N_fieldSize; j++){
            if (field_ind[i][j]!=0){
                if (
                    (
                        (i>0 and field_ind[i-1][j]!=0) or //слева
                        (i<N_fieldSize-1 and field_ind[i+1][j]!=0) or //справа
                        (j>0 and field_ind[i][j-1]!=0) or //сверху
                        (j<N_fieldSize-1 and field_ind[i][j+1]!=0) //снизу
                    ) or
                    (
                        !various_digits.contains(field_ind[i][j])
                    )

                ){
                    if (!various_digits.contains(field_ind[i][j])){
                        result.extra_digits = 1;
                    }
                    else{
                        result.neighbours = 1;
                    }
                    result.errors.append(SetFieldResult::coord(i, j));
                }
                if (various_digits.contains(field_ind[i][j])){
                    various_digits.removeOne(field_ind[i][j]);
                }
            }
        }




    }

    if (!various_digits.empty()){
        result.not_enough_digits = 1;
    }

    if (result.ok()){
        int i = 0;
        int j = 0;
        (*curfield) = field(N_fieldSize);
        foreach (auto row, field_ind) {
            foreach (auto num, row){
                (*curfield).setNumberToCell(i,j,num);
                i++;
            }
            j++;
        }

    }

    return result;
}

bool gameControler::field_empty(Players player)
{
    if (player){
        return this->field2.empty();
    }
    else{
        return this->field1.empty();
    }
}

int gameControler::getTurnsMade() const
{
    return turnsMade;
}

bool gameControler::gameHasEnded()
{
    return (
        this->turnsMade >= this->K_numberOfTurns or
        this->field1.fully_opened() or this->field2.fully_opened()
    );
}
