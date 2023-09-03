
#ifndef FIELD_H
#define FIELD_H

#include "cell.h"

struct open_result {
    bool hasNumber;
    int number;
};

class field
{
public:
    field() {}
    field(int size);
    open_result openCell(int row, int col);
    void setNumberToCell(int row, int col, int number);
    int getSize();

private:
    int _size = 4;
    cell* _cells;

    cell& getCell(int row, int col);
    int index(int row, int col);
};

#endif // FIELD_H
