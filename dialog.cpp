#include "dialog.h"
#include "ui_dialog.h"
#include <QThread>
#include "myThread.h"
#include "delegate.h"
#include "myThread.h"
Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
//    ui->tableView->horizontalHeader()->hide();
//    ui->tableView->verticalHeader()->hide();
    mThread = new myThread(this);
    connect(mThread, SIGNAL(makeStep()), this, SLOT(onMakeStep()));
}

Dialog::~Dialog()
{
    delete ui;
}

//creates grid
void Dialog::on_SizePushButton_clicked()
{
    QString labelText = "Width: ";
    int width = ui->XspinBox->value();
    int height = ui->YspinBox->value();

    labelText.append(QString::number(width));
    labelText.append(", Height: ");
    labelText.append(QString::number(height));
    ui->sizeLabel->setText(labelText);

    myDelegate = new Delegate(this);

    //create and set up table model. the grid will have additional 2 rows and columns on each side
    //that will not be seen by user
    model = new QStandardItemModel(height + 4, width + 4, this);

    for (int col = 0; col < width + 4; col ++)
    {
        for (int row = 0; row < height + 4; row ++)
        {
            QModelIndex index = model->index(row, col, QModelIndex());
            model->setData(index, 0);
        }
    }

    //populate and set up table
    ui->tableView->setModel(model);
    ui->tableView->setItemDelegate(myDelegate);
    setUpVisibility(width, height);

}

void Dialog::setUpCellSize(int cellSize)
{
    ui->tableView->horizontalHeader()->setDefaultSectionSize(cellSize);
    ui->tableView->verticalHeader()->setDefaultSectionSize(cellSize);
}

//click to set cell to live/dead condition
void Dialog::on_tableView_clicked(const QModelIndex &index)
{
    if(index.data().toInt() == 1)
    {
        model->setData(index, 0);
    }
    else
    {
        model->setData(index, 1);
    }
    ui->tableView->setModel(model);

}

//
void Dialog::makeStep()
{
        QList<QModelIndex> toKill;
        QList<QModelIndex> toResurrect;

        QList<QModelIndex> aliveIndexes= findLiveCells();
        int aliveInNeighborhood;
        QModelIndex index;

        //algorithm goes through alive cells and their neighbors
        foreach(index, aliveIndexes)
        {
            aliveInNeighborhood = 0;
            if (!toKill.contains(index) && !toResurrect.contains(index))   //skip this index if it's already in kill/resurrect list
            {
                for (int col = index.column() -1; col <= index.column() +1; col++)
                    for(int row = index.row() -1; row <= index.row() +1; row++)
                    {
                        QModelIndex tempIndex = model->index(row, col, QModelIndex());
                        aliveInNeighborhood = countAliveNeighbors(tempIndex);
                        if (tempIndex.data()==0 && aliveInNeighborhood == 3 && !toResurrect.contains(tempIndex))
                        {
                            toResurrect.append(tempIndex);
                        }
                        if (tempIndex.data() == 1 && (aliveInNeighborhood < 3 || aliveInNeighborhood > 4) && !toKill.contains(tempIndex))
                        {
                            toKill.append(tempIndex);
                        }
                    }
            }
        }

        if (toKill.count() == 0 && toResurrect.count() == 0)
        {
            mThread->stop();
            return;
        }

        killCell(toKill);
        resurrectCell(toResurrect);
}

void Dialog::killCell(QList<QModelIndex> indexes)
{
    for (int i = 0; i < indexes.size(); i++)
    model->setData(indexes.value(i), 0);
    ui->tableView->setModel(model);
}

void Dialog::resurrectCell(QList<QModelIndex> indexes)
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
void Dialog::setUpVisibility(int width, int height)
{

    for (int col = 0; col < width + 4; col++)
    {
        if ((col < 2) || (col > width + 1))
            ui->tableView->hideColumn(col);
        else
            ui->tableView->showColumn(col);
    }

    for (int row = 0; row < height + 4; row++)
    {
        if ((row < 2) || (row > height + 1))
            ui->tableView->hideRow(row);
        else
            ui->tableView->showRow(row);
    }
}

void Dialog::on_stepButton_clicked()
{
    makeStep();
}

void Dialog::on_startButton_clicked()
{
    mThread->start(QThread::HighestPriority);
}

void Dialog::on_pauseButton_clicked()
{
    mThread->stop();
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
