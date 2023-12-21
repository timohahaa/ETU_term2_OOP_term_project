
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

int field::index(int row, int col)
{
    return row * this->_size + col;
}

cell &field::operator()(int i, int j)
{
    return getCell(i, j);
}

bool field::fully_opened()
{
    for (int i = 0; i<getSize(); i++){
        for (int j = 0; j<getSize(); j++){
            if((*this)(i,j).hasNumber() and !(*this)(i,j).isOpen()){
                return 0;
            }
        }
    }
    return 1;
}

bool field::empty()
{
    return this->_size==0;
}
