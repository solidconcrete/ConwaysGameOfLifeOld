#include "delegate.h"

Delegate::Delegate(QObject *parent) :
    QItemDelegate(parent)
{

}

QWidget * Delegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    QLabel *label = new QLabel(parent);
    return label;
}

void Delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.data() == 0)
        painter->fillRect(option.rect, QColor(0,0,0));
    else
        painter->fillRect(option.rect, QColor(250,250,3));
}

void Delegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
