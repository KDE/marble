#include "RoutingLineEdit.h"

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
    setStyleSheet( QString( ":enabled { padding-%1: %2; }").arg( direction).arg( 18 ) );

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

    if ( layoutDirection() == Qt::LeftToRight ) {
        d->m_clearButton->setPixmap( QPixmap( ":/icons/edit-clear-locationbar-rtl.png" ) );
    } else {
        d->m_clearButton->setPixmap( QPixmap ( ":/icons/edit-clear-locationbar-ltr.png" ) );
    }
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
