//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// The code in this file is largely based on KDE's KLineEdit class
// as included in KDE 4.5. See there for its authors:
// http://api.kde.org/4.x-api/kdelibs-apidocs/kdeui/html/klineedit_8cpp.html
//
// Copyright 2010,2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "MarbleLineEdit.h"
#include "MarbleGlobal.h"

#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QLabel>
#include <QtGui/QStyle>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtCore/QTimer>

namespace Marble
{

class MarbleLineEditPrivate
{
public:
    QLabel* m_clearButton;

    QLabel* m_decoratorButton;

    QPixmap m_clearPixmap;

    QPixmap m_decoratorPixmap;

    QTimer m_progressTimer;

    QVector<QPixmap> m_progressAnimation;

    int m_currentFrame;

    int m_iconSize;

    MarbleLineEditPrivate( MarbleLineEdit* parent );

    void createProgressAnimation();
};

MarbleLineEditPrivate::MarbleLineEditPrivate( MarbleLineEdit* parent ) :
    m_clearButton( new QLabel( parent ) ), m_decoratorButton( new QLabel( parent ) ),
    m_currentFrame( 0 ), m_iconSize( 16 )
{
    m_clearButton->setCursor( Qt::ArrowCursor );
    m_clearButton->setToolTip( QObject::tr( "Clear" ) );
    m_decoratorButton->setCursor( Qt::ArrowCursor );
    createProgressAnimation();
    m_progressTimer.setInterval( 100 );
    if ( MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ) {
        m_iconSize = 32;
    }
}

void MarbleLineEditPrivate::createProgressAnimation()
{
    // Size parameters
    qreal const h = m_iconSize / 2.0; // Half of the icon size
    qreal const q = h / 2.0; // Quarter of the icon size
    qreal const d = 7.5; // Circle diameter
    qreal const r = d / 2.0; // Circle radius

    // Canvas parameters
    QImage canvas( m_iconSize, m_iconSize, QImage::Format_ARGB32 );
    QPainter painter( &canvas );
    painter.setRenderHint( QPainter::Antialiasing, true );
    painter.setPen( QColor ( Qt::gray ) );
    painter.setBrush( QColor( Qt::white ) );

    // Create all frames
    for( double t = 0.0; t < 2 * M_PI; t += M_PI / 8.0 ) {
        canvas.fill( Qt::transparent );
        QRectF firstCircle( h - r + q * cos( t ), h - r + q * sin( t ), d, d );
        QRectF secondCircle( h - r + q * cos( t + M_PI ), h - r + q * sin( t + M_PI ), d, d );
        painter.drawEllipse( firstCircle );
        painter.drawEllipse( secondCircle );
        m_progressAnimation.push_back( QPixmap::fromImage( canvas ) );
    }
}

MarbleLineEdit::MarbleLineEdit( QWidget *parent ) :
        QLineEdit( parent ), d( new MarbleLineEditPrivate( this ) )
{
    updateClearButtonIcon( text() );
    updateClearButton();

    setDecorator( d->m_decoratorPixmap );
    connect( this, SIGNAL( textChanged( QString ) ),
             SLOT( updateClearButtonIcon( QString ) ) );
    connect( &d->m_progressTimer, SIGNAL( timeout() ),
             this, SLOT( updateProgress() ) );
}

MarbleLineEdit::~MarbleLineEdit()
{
    delete d;
}

void MarbleLineEdit::setDecorator(const QPixmap &decorator)
{
    d->m_decoratorPixmap = decorator;
    d->m_decoratorButton->setPixmap( d->m_decoratorPixmap );
    int const padding = 2 + d->m_decoratorPixmap.width();

    QString const prefixDirection = layoutDirection() == Qt::LeftToRight ? "left" : "right";
    QString decoratorStyleSheet;
    if ( !d->m_decoratorPixmap.isNull() ) {
        decoratorStyleSheet = QString( "; padding-%1: %2" ).arg( prefixDirection ).arg( padding );
    }
    // Padding for clear button to avoid text underflow
    QString const postfixDirection  = layoutDirection() == Qt::LeftToRight ? "right" : "left";
    QString styleSheet = QString( ":enabled { padding-%1: %2; %3}").arg( postfixDirection ).arg( padding ).arg( decoratorStyleSheet );

    if ( !MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ) {
        setStyleSheet( styleSheet );
    }
}

void MarbleLineEdit::setBusy(bool busy)
{
    if ( busy ) {
        d->m_progressTimer.start();
    } else {
        d->m_progressTimer.stop();
        d->m_decoratorButton->setPixmap( d->m_decoratorPixmap );
    }
}

void MarbleLineEdit::updateClearButtonIcon( const QString& text )
{
    d->m_clearButton->setVisible( text.length() > 0 );
    if ( d->m_clearButton->pixmap() && !d->m_clearButton->pixmap()->isNull() ) {
        return;
    }

    QString const direction = layoutDirection() == Qt::LeftToRight ? "rtl" : "ltr";
    int const size = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ? 32 : 16;
    QPixmap pixmap = QPixmap( QString( ":/icons/%1x%1/edit-clear-locationbar-%2.png").arg( size ).arg( direction ) );
    d->m_clearButton->setPixmap( pixmap );
}

void MarbleLineEdit::updateClearButton()
{
    const QSize geom = size();
    const int frameWidth = style()->pixelMetric( QStyle::PM_DefaultFrameWidth, 0, this );
    const int pixmapSize = d->m_clearButton->pixmap()->width() + 1;
    const int decoratorSize = d->m_decoratorPixmap.width() + 1;

    int y = ( geom.height() - pixmapSize ) / 2;
    if ( layoutDirection() == Qt::LeftToRight ) {
        d->m_clearButton->move( geom.width() - frameWidth - pixmapSize - decoratorSize, y );
        d->m_decoratorButton->move( frameWidth - decoratorSize + 1, y );
    } else {
        d->m_clearButton->move( frameWidth - decoratorSize + 1, y );
        d->m_decoratorButton->move( geom.width() - frameWidth - pixmapSize - decoratorSize, y );
    }
}

void MarbleLineEdit::updateProgress()
{
    if ( !d->m_progressAnimation.isEmpty() ) {
        d->m_currentFrame = ( d->m_currentFrame + 1 ) % d->m_progressAnimation.size();
        QPixmap frame = d->m_progressAnimation[d->m_currentFrame];
        d->m_decoratorButton->setPixmap( frame );
    }
}

void MarbleLineEdit::mouseReleaseEvent( QMouseEvent* e )
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

    if ( d->m_decoratorButton == childAt( e->pos() ) ) {
        emit decoratorButtonClicked();
    }

    QLineEdit::mouseReleaseEvent( e );
}

void MarbleLineEdit::resizeEvent( QResizeEvent * event )
{
    updateClearButton();
    QLineEdit::resizeEvent( event );
}

} // namespace Marble

#include "MarbleLineEdit.moc"
