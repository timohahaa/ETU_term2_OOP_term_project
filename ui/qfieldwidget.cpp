
#include "qfieldwidget.h"
#include "qlineedit.h"
#include <QSpinBox>
#include <QObject>
#include <QRegularExpressionValidator>
#include <QSizePolicy>
#define fieldsize 400

QFieldWidget::QFieldWidget(QWidget *parent, bool isOpponent, int _gridSize) :

QWidget(parent) {
    setMinimumSize(fieldsize, fieldsize);
    setMaximumSize(fieldsize, fieldsize);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    gridLayout = new QGridLayout();
    gridLayout->setSpacing(0);
    gridLayout->setSizeConstraint(gridLayout->SetMaximumSize);
    gridSize = _gridSize;
    if (isOpponent){
        currentState = Opponent;
    }
    else{
        currentState = Placing;
    }

    createGrid();
    //qDebug()<<fieldsize<< " " << fieldsize/_gridSize;

    setLayout(gridLayout);
}

void QFieldWidget::createGrid() {
    clearGrid();

    cells = new QWidget**[gridSize];
    for (int row = 0; row < gridSize; ++row) {
        cells[row] = new QWidget*[gridSize];
        for (int col = 0; col < gridSize; ++col) {
            cells[row][col] = createPushButton(row, col); // По умолчанию используем QPushButton

            gridLayout->addWidget(cells[row][col], row, col);
            updateCellState(row, col);
        }
    }
}

void QFieldWidget::clearGrid() {
    if (cells) {
        for (int row = 0; row < gridSize; ++row) {
            for (int col = 0; col < gridSize; ++col) {
                delete cells[row][col];
            }
            delete[] cells[row];
        }
        delete[] cells;
    }
}

void QFieldWidget::updateGridSize(int size) {
    gridSize = size;
    createGrid();
}

QLineEdit *QFieldWidget::createLineEdit(int row, int col)
{

    auto *filter = new QRegularExpressionValidator(QRegularExpression("[1-9]{1}"));
    QLineEdit *lineEdit = new QLineEdit(this);
    lineEdit->setAlignment(Qt::AlignmentFlag::AlignCenter);
    lineEdit->setValidator(filter);
    lineEdit->setFixedSize(fieldsize/gridSize, fieldsize/gridSize);
    lineEdit->setProperty("row", row);
    lineEdit->setProperty("col", col);
    connect(lineEdit, SIGNAL(editingFinished()), this, SLOT(handleCellClick()));

    return lineEdit;
}

QPushButton *QFieldWidget::createPushButton(int row, int col)
{
    auto button = new QPushButton(this);
    button->setFixedSize(fieldsize/gridSize, fieldsize/gridSize);
    //button->setText(QString::number(col)+QString::number(row)); //test
    button->setProperty("row", row);
    button->setProperty("col", col);
    connect(button, SIGNAL(clicked()), this, SLOT(handleCellClick()));

    return button;
}



void QFieldWidget::handleCellClick() {
    if (currentState == Opponent) {
        QLineEdit *clickedLineEdit = qobject_cast<QLineEdit*>(sender());
        if (clickedLineEdit) {
            int row = clickedLineEdit->property("row").toInt();
            int col = clickedLineEdit->property("col").toInt();

            if (currentState == Placing) {
                // Логика для режима размещения

            }
        }



    } else {
        QPushButton *clickedButton = qobject_cast<QPushButton*>(sender());
        if (clickedButton) {
            int row = clickedButton->property("row").toInt();
            int col = clickedButton->property("col").toInt();
            // Обработка клика в режиме LineEdit
        }
    }
}

void QFieldWidget::setPlacingState() {
    currentState = Placing;
    for (int row = 0; row < gridSize; ++row) {
        for (int col = 0; col < gridSize; ++col) {
            updateCellState(row, col);
        }
    }
}

void QFieldWidget::setLockedState() {
    currentState = Locked;
    for (int row = 0; row < gridSize; ++row) {
        for (int col = 0; col < gridSize; ++col) {
            updateCellState(row, col);
        }
    }
}

void QFieldWidget::setCellColorFail(int row, int col)
{

}

void QFieldWidget::setVisible(bool isVisible)
{
    QWidget::setVisible(isVisible);
    if (cells){
        for (int row = 0; row < gridSize; ++row) {
            for (int col = 0; col < gridSize; ++col) {
                cells[row][col]->setVisible(isVisible);
            }
        }
    }
}

void QFieldWidget::setOpponentState() {
    currentState = Opponent;
    for (int row = 0; row < gridSize; ++row) {
        for (int col = 0; col < gridSize; ++col) {
            updateCellState(row, col);
        }
    }
}

void QFieldWidget::updateCellState(int row, int col) {
    QLayoutItem *Item = gridLayout->itemAtPosition(row, col);
    QWidget *previousWidget = Item->widget();
    previousWidget->setParent(NULL);

    switch (currentState){
    case (Placing):
        {
            QLineEdit *lineEdit = createLineEdit(row, col);

            gridLayout->replaceWidget(previousWidget, lineEdit);
            delete previousWidget;
            cells[row][col] = lineEdit;
            break;
        }
    case (Locked):
        {
            QPushButton *button = createPushButton(row, col);
            button->setEnabled(false);

            gridLayout->replaceWidget(previousWidget, button);
            delete previousWidget;
            cells[row][col] = button;
            break;
        }
    case (Opponent):
        {
            QPushButton *button = createPushButton(row, col);

            gridLayout->replaceWidget(previousWidget, button);
            delete previousWidget;
            cells[row][col] = button;
            break;
        }

    }
}
