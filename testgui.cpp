#include <QtWidgets>
#include <QtTest/QtTest>
#include "dialog.h"
#include "ui_dialog.h"
class TestGui: public QObject
{
    Q_OBJECT
private:
//    Dialog d;
private slots:
//    void testGui_data();
    void testSizeSpinner();
    void testSizeSpinner_data();
    void countAliveCellsInDeadGrid();
    void oneCellStep();
    void cellsReproduce();
    void testResurrect();
    void testKill();
    void testZoomSlider();
};
void TestGui::testSizeSpinner_data()
{
    QTest::addColumn<QTestEventList>("events");
    QTest::addColumn<int>("expected");

    QTestEventList list1;
    list1.addKeyClick(Qt::Key_Delete);
    list1.addKeyClicks("1000");
    QTest::newRow("1000") << list1 << 100;

    QTestEventList list2;
    list2.addKeyClick(Qt::Key_Delete);
    list2.addKeyClicks("10");
    QTest::newRow("10") << list2 << 10;

    QTestEventList list3;
    list3.addKeyClick(Qt::Key_Delete);
    list3.addKeyClicks("-666");
    QTest::newRow("-666") << list3 << 66;

    QTestEventList list4;
    list4.addKeyClick(Qt::Key_Delete);
    list4.addKeyClicks("500");
    QTest::newRow("500") << list4 << 500;
}

void TestGui::testSizeSpinner()
{
    Dialog d;
    QFETCH(QTestEventList, events);
    QFETCH(int, expected);

        d.ui->XspinBox->setValue(1);
        events.simulate(d.ui->XspinBox);
        QCOMPARE(d.ui->XspinBox->text().toInt(), expected);
}

void TestGui::countAliveCellsInDeadGrid()
{
    Dialog d;
    QTest::keyClicks(d.ui->XspinBox, "50");
    QTest::keyClicks(d.ui->YspinBox, "25");
    QTest::mouseClick(d.ui->SizePushButton, Qt::LeftButton);
    int aliveCells = d.findLiveCells().count();
    QCOMPARE(0, aliveCells);
}

void TestGui::oneCellStep()
{
    Dialog d;
    QTest::keyClicks(d.ui->XspinBox, "50");
    QTest::keyClicks(d.ui->YspinBox, "25");
    QTest::mouseClick(d.ui->SizePushButton, Qt::LeftButton);
    QModelIndex index = d.model->index(5, 5, QModelIndex());
    d.model->setData(index, 1);
    d.makeStep();
    int aliveCells = d.findLiveCells().count();
    QCOMPARE(0, aliveCells);
}

void TestGui::cellsReproduce()
{
    Dialog d;
    QTest::keyClicks(d.ui->XspinBox, "50");
    QTest::keyClicks(d.ui->YspinBox, "25");
    QTest::mouseClick(d.ui->SizePushButton, Qt::LeftButton);
    QModelIndex index1 = d.model->index(5, 5, QModelIndex());
    QModelIndex index2 = d.model->index(6, 5, QModelIndex());
    QModelIndex index3 = d.model->index(6, 6, QModelIndex());
    d.model->setData(index1, 1);
    d.model->setData(index2, 1);
    d.model->setData(index3, 1);
    d.makeStep();
    int aliveCells = d.findLiveCells().count();
    QCOMPARE(4, aliveCells);
}

void TestGui::testResurrect()
{
    Dialog d;
    QTest::keyClicks(d.ui->XspinBox, "50");
    QTest::keyClicks(d.ui->YspinBox, "25");
    QTest::mouseClick(d.ui->SizePushButton, Qt::LeftButton);
    QList<QModelIndex> indexes;
    indexes.append( d.model->index(5, 5, QModelIndex()));
    indexes.append (d.model->index(6, 5, QModelIndex()));
    indexes.append (d.model->index(6, 6, QModelIndex()));
    d.resurrectCells(indexes);
    int aliveCells = d.findLiveCells().count();
    QCOMPARE(3, aliveCells);

}

void TestGui::testKill()
{
    Dialog d;
    QTest::keyClicks(d.ui->XspinBox, "50");
    QTest::keyClicks(d.ui->YspinBox, "25");
    QTest::mouseClick(d.ui->SizePushButton, Qt::LeftButton);
    QList<QModelIndex> indexes;
    indexes.append( d.model->index(5, 5, QModelIndex()));
    indexes.append (d.model->index(6, 5, QModelIndex()));
    indexes.append (d.model->index(6, 6, QModelIndex()));
    d.killCells(indexes);
    int aliveCells = d.findLiveCells().count();
    QCOMPARE(0, aliveCells);
};

void TestGui::testZoomSlider()
{
    Dialog d;
    d.ui->cellSizeSlider->setValue(50);
    QCOMPARE(50, d.ui->tableView->horizontalHeader()->defaultSectionSize());
}

//uncomment this line and comment main class to run tests
//QTEST_MAIN(TestGui)
#include "testgui.moc"
