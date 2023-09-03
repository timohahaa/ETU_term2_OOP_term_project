
#include "field.h"

field::field(int size)
{
    this->_size = size;
    this->_cells = new cell[size*size];
}

cell& field::getCell(int row, int col)
{
    int idx = this->index(row, col);
    return this->_cells[idx];
}

open_result field::openCell(int row, int col)
{
    cell c = this->getCell(row, col);
    c.open();
    open_result res;
    res.hasNumber = c.hasNumber();
    res.number = c.number();
    return res;
}

void field::setNumberToCell(int row, int col, int number)
{
    cell c = this->getCell(row, col);
    c.setNumber(number);
}

int field::getSize()
{
    return this->_size;
}

int field::getOpenNumberCellCount()
{
    return this->_countOpenNumberCells;
}

void field::incrementOpenNumberCellCount()
{
    this->_countOpenNumberCells++;
}

int field::index(int row, int col)
{
    return row * this->_size + col;
}
