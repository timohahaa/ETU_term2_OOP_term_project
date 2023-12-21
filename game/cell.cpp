
#include "cell.h"

cell::cell()
{

}

void cell::open()
{
    this->_isOpen = true;
}

bool cell::isOpen()
{
    return this->_isOpen;
}

bool cell::hasNumber()
{
    return this->_hasNumber();
}

int cell::number()
{
    return this->_number;
}

void cell::setNumber(int number)
{
    this->_number = number;
}

