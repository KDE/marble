//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// The code in this file is largely based on KDE's KLineEdit class
// as included in KDE 4.5. See there for its authors:
// http://api.kde.org/4.x-api/kdelibs-apidocs/kdeui/html/klineedit_8cpp.html
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "RoutingLineEdit.h"
#include "global.h"

#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QLabel>
#include <QtGui/QStyle>
#include <QtGui/QMouseEvent>

namespace Marble
{

class RoutingLineEditPrivate
{
public:
    QLabel* m_clearButton;

    QPixmap m_clearPixmap;

    RoutingLineEditPrivate( RoutingLineEdit* parent );
};

RoutingLineEditPrivate::RoutingLineEditPrivate( RoutingLineEdit* parent ) :
        m_clearButton( new QLabel( parent ) )
{
    m_clearButton->setCursor( Qt::ArrowCursor );
    m_clearButton->setToolTip( QObject::tr( "Clear" ) );
}

RoutingLineEdit::RoutingLineEdit( QWidget *parent ) :
        QLineEdit( parent ), d( new RoutingLineEditPrivate( this ) )
{
    updateClearButtonIcon( text() );
    updateClearButton();

    // Padding for clear button to avoid text underflow
    QString const direction = layoutDirection() == Qt::LeftToRight ? "right" : "left";
    if ( !MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ) {
        setStyleSheet( QString( ":enabled { padding-%1: %2; }").arg( direction).arg( 18 ) );
    }

    connect( this, SIGNAL( textChanged( QString ) ), SLOT( updateClearButtonIcon( QString ) ) );
}

RoutingLineEdit::~RoutingLineEdit()
{
    delete d;
}

void RoutingLineEdit::updateClearButtonIcon( const QString& text )
{
    d->m_clearButton->setVisible( text.length() > 0 );
    if ( d->m_clearButton->pixmap() && !d->m_clearButton->pixmap()->isNull() ) {
        return;
    }

    QString const direction = layoutDirection() == Qt::LeftToRight ? "rtl" : "ltr";
    int const size = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ? 22 : 16;
    QPixmap pixmap = QPixmap( QString( ":/icons/%1x%1/edit-clear-locationbar-%2.png").arg( size ).arg( direction ) );
    d->m_clearButton->setPixmap( pixmap );
}

void RoutingLineEdit::updateClearButton()
{
    const QSize geom = size();
    const int frameWidth = style()->pixelMetric( QStyle::PM_DefaultFrameWidth, 0, this );
    const int pixmapSize = d->m_clearButton->pixmap()->width();

    int y = ( geom.height() - pixmapSize ) / 2;
    if ( layoutDirection() == Qt::LeftToRight ) {
        d->m_clearButton->move( geom.width() - frameWidth - pixmapSize - 1, y );
    } else {
        d->m_clearButton->move( frameWidth + 1, y );
    }
}

void RoutingLineEdit::mouseReleaseEvent( QMouseEvent* e )
{
    if ( d->m_clearButton == childAt( e->pos() ) ) {
        QString newText;
        if ( e->button() == Qt::MidButton ) {
            newText = QApplication::clipboard()->text( QClipboard::Selection );
            setText( newText );
        } else {
            setSelection( 0, text().size() );
            del();
            emit clearButtonClicked();
        }
        emit textChanged( newText );
    }

    QLineEdit::mouseReleaseEvent( e );
}

void RoutingLineEdit::resizeEvent( QResizeEvent * event )
{
    updateClearButton();
    QLineEdit::resizeEvent( event );
}

} // namespace Marble

#include "RoutingLineEdit.moc"
