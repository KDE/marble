#include "MarbleTextEdit.h"

#include <QDebug>
#include <QWidget>

MarbleTextEdit::MarbleTextEdit(QWidget * parent)
        :QTextEdit(parent)
{
}

void MarbleTextEdit::resizeEvent ( QResizeEvent * event )
{
    if( viewport()->size().height() > this->size().height() ) {
        mDebug() << "Viewport Bigger than widget!!!!!!";
    } else {
        mDebug() << "viewport Smaller than widget";
        this->resize( viewport()->size() );
    }
    QTextEdit::resizeEvent( event );
}
