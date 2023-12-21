
#ifndef GAMECONTROLER_H
#define GAMECONTROLER_H

#include "field.h"

#include <QVector>

enum Players{
    pl1,
    pl2
};

struct TurnResult {
    bool hasNumber(){return number==0;}
    int number;
    bool gameEnded;
};

struct SetFieldResult{
    struct coord {
        int i;
        int j;
        coord(int _i, int _j){i=_i;j=_j;}
    };
    bool field_size_err = 0;
    bool not_enough_digits = 0;
    bool extra_digits = 0;
    bool neighbours = 0;
    bool ok(){return errors.empty() and !field_size_err and !not_enough_digits and !extra_digits;};
    bool err(){return !ok();}
    QVector<coord> errors;
};


class gameControler
{
public:
    gameControler(int N, int M, int K);
    TurnResult openCell(int row, int col);
    void setNumberToCell(int row, int col, int number);
    int getScores(Players player);

    int getOpenNumberCellsCount();
    int getN_fieldSize() const;
    int getM_numberCount() const;
    int getK_numberOfTurns() const;
    SetFieldResult set_field(QVector<QVector<int>> field, Players player);
    bool field_empty(Players player);

    int getTurnsMade() const;

private:
    int N_fieldSize;
    int M_numberCount;
    int K_numberOfTurns;

    int turnsMade = 0;
    int scores1;
    int scores2;

    field field1;
    field field2;

    bool gameHasEnded();
};

#endif // GAMECONTROLER_H
