
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
    cell& getCell(int row, int col);
    open_result openCell(int row, int col);
    void setNumberToCell(int row, int col, int number);
    int getSize();
    int getOpenNumberCellCount();
    void incrementOpenNumberCellCount();

private:
    int _size = 4;
    cell* _cells;
    int _countOpenNumberCells = 0;

    int index(int row, int col);
};

#endif // FIELD_H
