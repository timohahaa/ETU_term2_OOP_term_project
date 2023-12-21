
#include "gamecontroler.h"
#include "field.h"

gameControler::gameControler(int N, int M, int K)
{
    this->N_fieldSize = N;
    this->M_numberCount = M;
    this->K_numberOfTurns = K;
    this->field1 = nullptr;
    this->field2 = nullptr;
}

TurnResult gameControler::openCell(int row, int col)
{
    this->turnsMade++;
    open_result cell_result;

    if (whos_turn==pl1){
        cell_result = this->field2->openCell(row, col);
        this->scores1+=cell_result.number;

        this->whos_turn = pl2;
    }
    else{
        cell_result = this->field1->openCell(row, col);
        this->scores2+=cell_result.number;
        this->whos_turn = pl1;
    }
    TurnResult result;
    result.number = cell_result.number;
    result.gameEnded = this->gameHasEnded();
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

    field* curfield;
    if (result.ok()){
        int i = 0;
        int j = 0;
        curfield = new field(N_fieldSize);
        foreach (auto row, field_ind) {
            i=0;
            foreach (auto num, row){
                curfield->setNumberToCell(i,j,num);
                i++;
            }
            j++;
        }
        if (player==pl2){
            this->field2 = curfield;
        }
        else{
            this->field1 = curfield;
        }

    }

    return result;
}

bool gameControler::field_empty(Players player)
{
    if (player){

        return this->field2==nullptr;
    }
    else{
        return this->field1==nullptr;
    }
}

int gameControler::getTurnsMade() const
{
    return turnsMade;
}

Players gameControler::getWhos_turn() const
{
    return whos_turn;
}

void gameControler::setWhos_turn(Players newWhos_turn)
{
    whos_turn = newWhos_turn;
}

bool gameControler::gameHasEnded()
{
    return (
        (this->turnsMade >= this->K_numberOfTurns) or
        this->field1->fully_opened() or this->field2->fully_opened()
    );
}
