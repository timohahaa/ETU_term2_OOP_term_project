
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
    enum CellColor{
        Default, Error
    };
    enum State {
        Placing, Locked, Opponent
    };

    // Добавленные функции
    void opponent_lock();
    void opponent_unlock();

    void setPlacingState();
    void setOpponentState();
    void setLockedState();
    void setCellColor(int row, int col, CellColor color);
    void resetColors();
    void setVisible(bool isVisible);
    void setDisabled(bool isDisabled);
    QVector<QVector<int>> get_indexes();
    State getState(){return currentState;}

    void set_cell_text(int col, int row, QString text);
private slots:
    void handleCellClick();
    void updateGridSize(int size);

private:
    QLineEdit* createLineEdit(int row, int col);
    QPushButton* createPushButton(int row, int col);

    QGridLayout *gridLayout;
    QWidget ***cells = nullptr; // Динамический массив для хранения виджетов
    int gridSize; // Размер поля
    State currentState; // Текущее состояние

    void createGrid();
    void clearGrid();
    void updateCellState(int row, int col);

signals:
    void cell_clicked(int i, int j);
};

#endif // QFIELDWIDGET_H
