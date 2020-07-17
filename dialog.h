#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QtCore>
#include <QtGui>
#include <QDebug>
#include "delegate.h"
#include "myThread.h"
QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

    friend class TestGui;

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();

    QList<int> const checkNeighbors(QModelIndex &index);

    QList<QModelIndex> const findLiveCells();

    void killCell(QList<QModelIndex>);

    void resurrectCell(QList<QModelIndex>);

    int countAliveNeighbors(QModelIndex);

    void setUpVisibility(int width, int height);

    void makeStep();

    void setUpCellSize(int);

    myThread *mThread;

private slots:
    void on_SizePushButton_clicked();

    void on_tableView_clicked(const QModelIndex &index);

    void on_stepButton_clicked();

    void on_startButton_clicked();

    void on_pauseButton_clicked();

    void on_speedSlider_valueChanged(int value);

    void on_cellSizeSlider_valueChanged(int value);

public slots:
    void onMakeStep();

private:
    Ui::Dialog *ui;
    QStandardItemModel *model;
    Delegate *myDelegate;


};
#endif // DIALOG_H
