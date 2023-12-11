
#ifndef QFIELDWIDGET_H
#define QFIELDWIDGET_H


#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>

class QFieldWidget : public QWidget {
    Q_OBJECT

public:
    explicit QFieldWidget(QWidget *parent = nullptr, bool isOpponent = false, int _gridSize=9);

    // Добавленные функции
    void setPlacingState();
    void setOpponentState();
    void setLockedState();
    void setCellColorFail(int row, int col);

    void setVisible(bool isVisible);

private slots:
    void handleCellClick();
    void updateGridSize(int size);

private:
    QLineEdit* createLineEdit(int row, int col);
    QPushButton* createPushButton(int row, int col);


    QGridLayout *gridLayout;
    QWidget ***cells = nullptr; // Динамический массив для хранения виджетов
    int gridSize; // Размер поля
    enum State {
        Placing, Locked, Opponent
    };
    State currentState; // Текущее состояние

    void createGrid();
    void clearGrid();
    void updateCellState(int row, int col);

};

#endif // QFIELDWIDGET_H
