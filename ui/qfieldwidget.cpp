
#include "qfieldwidget.h"
#include "qlineedit.h"
#include <QSpinBox>
#include <QObject>
#include <QRegularExpressionValidator>
#include <QSizePolicy>
#define fieldsize 400
#define fontsize 13

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
    setLayout(gridLayout);
}

void QFieldWidget::opponent_lock()
{
    if (currentState != Opponent)
        return;

    for (int row = 0; row < gridSize; ++row) {
        for (int col = 0; col < gridSize; ++col) {
            cells[row][col]->setDisabled(true);
        }
    }



}

void QFieldWidget::opponent_unlock()
{
    if (currentState != Opponent)
        return;
    for (int row = 0; row < gridSize; ++row) {
        for (int col = 0; col < gridSize; ++col) {
            if (cells[row][col]->styleSheet().isEmpty()){
                cells[row][col]->setEnabled(true);
            }
        }
    }
}

void QFieldWidget::set_cell_text(int col, int row, QString text)
{
    if (currentState==Opponent){
        qobject_cast<QPushButton*>(cells[row][col])->setText(text);
        qobject_cast<QPushButton*>(cells[row][col])->setDisabled(1);
        setCellColor(row, col, Error);
    }

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
    auto font = lineEdit->font();
    font.setPixelSize((fieldsize/gridSize)/2);
    this->setFont(font);
    lineEdit->setFont(font);
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

    auto font = button->font();
    font.setPixelSize((fieldsize/gridSize)/2);
    this->setFont(font);
    button->setFont(font);
    //button->setText(QString::number(col)+QString::number(row)); //test
    button->setProperty("row", row);
    button->setProperty("col", col);
    connect(button, SIGNAL(clicked()), this, SLOT(handleCellClick()));

    return button;
}

void QFieldWidget::handleCellClick() {
    if (currentState == Opponent) {
        QPushButton *clickedButton = qobject_cast<QPushButton*>(sender());
        if (clickedButton) {
            int row = clickedButton->property("row").toInt();
            int col = clickedButton->property("col").toInt();
            if (clickedButton->isEnabled()){
                emit cell_clicked(col, row);
                qDebug()<< "Клик на "<<row<<" "<< col;
            }

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

void QFieldWidget::setCellColor(int row, int col, CellColor color)
{
    QString ss;
    if (color==Default){
        ss = "";
    }
    else if (color == Error){
        ss = "background-color: #ff0000;";
    }
    cells[row][col]->setStyleSheet(ss);

}

void QFieldWidget::resetColors()
{
    if (cells){
        for (int row = 0; row < gridSize; ++row) {
            for (int col = 0; col < gridSize; ++col) {
                cells[row][col]->setStyleSheet("");
            }
        }
    }
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

void QFieldWidget::setDisabled(bool isDisabled)
{

    QWidget::setDisabled(isDisabled);
    if (cells){
        for (int row = 0; row < gridSize; ++row) {
            for (int col = 0; col < gridSize; ++col) {
                cells[row][col]->setDisabled(isDisabled);
            }
        }
    }

}

QVector<QVector<int>> QFieldWidget::get_indexes()
{
    QVector<QVector<int>> ret;
    ret.resize(this->gridSize);
    for (auto row : ret) {
        row.resize(this->gridSize);
    }
    for (int row = 0; row < gridSize; ++row) {
        for (int col = 0; col < gridSize; ++col) {
            QVariant a = cells[row][col]->property("text");
            if (a.toString() == "")
                ret[row].append(0);
            else
                ret[row].append(a.toInt());
        }
    }
    return ret;
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
            auto text = previousWidget->property("text").toString();
            button->setText(text);
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
