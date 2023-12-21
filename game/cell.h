
#ifndef CELL_H
#define CELL_H


class cell
{
public:
    cell();
    void open();
    bool isOpen();
    bool hasNumber();
    int number();
    void setNumber(int number);

private:
    bool _hasNumber(){return _number!=0;}
    bool _isOpen = false;
    int _number = 0;
};

#endif //CELL_H
