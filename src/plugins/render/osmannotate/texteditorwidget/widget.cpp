#include "widget.h"
#include "ui_widget.h"

#include <QtGui/QScrollBar>

Widget::Widget(QWidget *parent)
    : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);

//    QWidgetList scrollBarList = ui->textEdit->scrollBarWidgets( Qt::AlignRight );
//    if (
//    QScrollBar* verticalBar =dynamic_cast<QScrollBar*>( .first() );
//
//    connect( verticalBar, SIGNAL(rangeChanged(int,int)),
//             this, SLOT(expandTextEdit()) );
}

Widget::~Widget()
{
    delete ui;
}

void Widget::expandTextEdit()
{
    ui->textEdit->adjustSize();
}
