#include "dialog.h"
#include "ui_dialog.h"
#include <QThread>
#include <QString>
#include "myThread.h"
#include "delegate.h"
#include "myThread.h"
Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    model = new QStandardItemModel(105, 105, this);
    setUpModel();
    ui->tableView->setModel(model);
    myDelegate = new Delegate(this);
    ui->tableView->setItemDelegate(myDelegate);
    ui->tableView->setMouseTracking(true);
    setUpVisibility(100);
    mThread = new myThread(this);
    connect(mThread, SIGNAL(makeStep()), this, SLOT(onMakeStep()));
}

Dialog::~Dialog()
{
    mThread->stop();
    delete mThread;
    delete ui;
}


//set up grid size
void Dialog::on_SizePushButton_clicked()
{
    QString labelText = "Width: ";
    int width = ui->XspinBox->value();
    int height = ui->YspinBox->value();
    int margin;
    labelText.append(QString::number(width));
    labelText.append(", Height: ");
    labelText.append(QString::number(height));
    ui->sizeLabel->setText(labelText);

    //making grid bigger than requested and hiding those additional cells
    margin = 100;
    model->setColumnCount(width + margin);
    model->setRowCount(height + margin);
    void setUpModel();
    setUpVisibility(margin);

    this->setFocus();

}

//sets up a model so that newly added cells are marked as dead, but without killing living cells
void Dialog::setUpModel()
{
    for (int col = 0; col < model->columnCount(); col++)
    {
        for (int row = 0; row < model->rowCount(); row++)
        {
            QModelIndex index = model->index(row, col, QModelIndex());
            if (index.data() != 1)
            {
                model->setData(index, 0);
            }
        }
    }
}

void Dialog::setUpCellSize(int cellSize)
{
    ui->tableView->horizontalHeader()->setDefaultSectionSize(cellSize);
    ui->tableView->verticalHeader()->setDefaultSectionSize(cellSize);
}

void Dialog::makeStep()
{

    QList<QModelIndex> toKill;
    QList<QModelIndex> toResurrect;

    if (aliveCells.count() == 0)
    {
        aliveCells = findLiveCells();
    }

        int aliveInNeighborhood;

        //go through alive cells
    foreach(QModelIndex index, aliveCells)
    {
        aliveInNeighborhood = 0;
        {
            // go through live cells' neighbors
            for (int col = index.column() - 1; col <= index.column() + 1; col++)
                for(int row = index.row() - 1; row <= index.row() + 1; row++)
                {
                    QModelIndex tempIndex = model->index(row, col, QModelIndex());
                    aliveInNeighborhood = countAliveNeighbors(tempIndex);

                    if ((tempIndex.data() != 1 && aliveInNeighborhood == 3) && !toResurrect.contains(tempIndex))
                    {
                        toResurrect.append(tempIndex);
                        aliveCells.append(tempIndex);
                    }
                    if (tempIndex.data() == 1 && (aliveInNeighborhood < 3 || aliveInNeighborhood > 4) && !toKill.contains(tempIndex))
                    {
                        toKill.append(tempIndex);
                        aliveCells.removeOne(tempIndex);
                    }
                }
        }
    }
    if (toKill.count() == 0 && toResurrect.count() == 0)
    {
        mThread->stop();
        return;
    }
    killCells(toKill);
    resurrectCells(toResurrect);

    QString text = "Alive cells: ";
    text.append(QString::number(aliveCells.count()));
    ui->aliveCells->setText(text);
}

void Dialog::killCells(QList<QModelIndex> indexes)
{
    for (int i = 0; i < indexes.size(); i++)
    model->setData(indexes.value(i), 0);
    ui->tableView->setModel(model);
}

void Dialog::resurrectCells(QList<QModelIndex> indexes)
{
    for (int i = 0; i < indexes.size(); i++)
    model->setData(indexes.value(i), 1);
    ui->tableView->setModel(model);
}

// counts all alive neighbors that surround the cell (counts the cell itself too)
int Dialog::countAliveNeighbors(QModelIndex cellIndex)
{
    int aliveCells = 0;
    for (int col = cellIndex.column() - 1; col <= cellIndex.column() + 1; col++)
    {
        for (int row = cellIndex.row() - 1; row <= cellIndex.row() + 1; row++)
        {
            QModelIndex tempindex = model->index(row, col, QModelIndex());
            if (tempindex.data() == 1)
            {
                aliveCells ++;
            }
        }
    }
    return aliveCells;
}

//hide columns/rows that shouldn't be seen and show the ones that should be
void Dialog::setUpVisibility(int margin)
{
    for (int col = 0; col < model->columnCount(); col++)
    {
        if ((col < margin / 2) || (col >= model->columnCount() - margin / 2))
            ui->tableView->hideColumn(col);
        else
            ui->tableView->showColumn(col);
    }

    for (int row = 0; row < model->rowCount(); row++)
    {
        if ((row < margin / 2) || (row >= model->rowCount() - margin / 2))
            ui->tableView->hideRow(row);
        else
            ui->tableView->showRow(row);
    }
}

void Dialog::on_stepButton_clicked()
{
    makeStep();
    this->setFocus();
}

void Dialog::on_startButton_clicked()
{
    mThread->start(QThread::HighestPriority);
    this->setFocus();
}

void Dialog::on_pauseButton_clicked()
{
    mThread->stop();
    this->setFocus();
}

void Dialog::onMakeStep()
{
    makeStep();
}

void Dialog::on_speedSlider_valueChanged(int value)
{
    mThread->setLatency(1050-value);
}

void Dialog::on_cellSizeSlider_valueChanged(int value)
{
    setUpCellSize(value);
}

//return a list of indexes to alive cells
QList<QModelIndex> const Dialog::findLiveCells()
{
    QList<QModelIndex> indexes;
    int columns = ui->tableView->model()->columnCount();
    int rows = ui->tableView->model()->rowCount();
    for (int col = 1; col < columns; col++)
        for (int row = 1; row < rows; row++)
        {
            QModelIndex cellIndex = model->index(row, col, QModelIndex());
            if (cellIndex.data() == 1)
                indexes.append(cellIndex);
        }
    return indexes;
}

void Dialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space)
    {
        makeStep();
        return;
    }
    if (event->key() == Qt::Key_Plus)
    {
        ui->cellSizeSlider->setValue(ui->cellSizeSlider->value() + 1);
        return;
    }
    if (event->key() == Qt::Key_Minus)
    {
        ui->cellSizeSlider->setValue(ui->cellSizeSlider->value() - 1);

        return;
    }

}

void Dialog::on_clearButton_clicked()
{
    if (mThread->isStopped())
    {
        for (int col = 0; col < model->columnCount(); col++)
        {
            for (int row = 0; row < model->rowCount(); row++)
            {
                QModelIndex index = model->index(row, col, QModelIndex());
                model->setData(index, 0);
            }
        }
        aliveCells.clear();
        this->setFocus();
    }
}

void Dialog::on_tableView_entered(const QModelIndex &index)
{
//    QApplication::keyboardModifiers().testFlag(Qt::ControlModifier
    if (QApplication::mouseButtons().testFlag(Qt::LeftButton))
    {
        model->setData(index, 1);
        aliveCells.append(index);
    }
    if (QApplication::mouseButtons().testFlag(Qt::RightButton))
    {
        model->setData(index, 0);
        aliveCells.removeOne(index);
    }
    this->setFocus();
}

void Dialog::on_tableView_pressed(const QModelIndex &index)
{
    if(index.data().toInt() == 1)
    {
        model->setData(index, 0);
        aliveCells.removeOne(index);
    }
    else
    {
        model->setData(index, 1);
        aliveCells.append(index);
    }
    ui->tableView->setModel(model);
    this->setFocus();
}
