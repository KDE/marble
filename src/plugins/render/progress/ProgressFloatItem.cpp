//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2010,2011  Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "ProgressFloatItem.h"

#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleModel.h"
#include "MarbleWidget.h"
#include "ViewportParams.h"
#include "HttpDownloadManager.h"

#include <QtCore/QRect>
#include <QtGui/QColor>
#include <QtCore/QMutexLocker>
#include <QtGui/QPaintDevice>
#include <QtGui/QPainter>

namespace Marble
{

ProgressFloatItem::ProgressFloatItem()
    : AbstractFloatItem( 0 )
{
}

ProgressFloatItem::ProgressFloatItem( const MarbleModel *marbleModel )
    : AbstractFloatItem( marbleModel, QPointF( -10.5, -150.5 ), QSizeF( 40.0, 40.0 ) ),
      m_isInitialized( false ),
      m_totalJobs( 0 ),
      m_completedJobs ( 0 ),
      m_progressHideTimer(),
      m_progressShowTimer(),
      m_active( false ),
      m_fontSize( 0 ),
      m_repaintTimer()
{
    // This timer is responsible to activate the automatic display with a small delay
    m_progressShowTimer.setInterval( 250 );
    m_progressShowTimer.setSingleShot( true );
    connect( &m_progressShowTimer, SIGNAL(timeout()), this, SLOT(show()) );

    // This timer is responsible to hide the automatic display when downloads are finished
    m_progressHideTimer.setInterval( 750 );
    m_progressHideTimer.setSingleShot( true );
    connect( &m_progressHideTimer, SIGNAL(timeout()), this, SLOT(hideProgress()) );

    // Repaint timer
    m_repaintTimer.setSingleShot( true );
    m_repaintTimer.setInterval( 1000 );
    connect( &m_repaintTimer, SIGNAL(timeout()), this, SIGNAL(repaintNeeded()) );

    // The icon resembles the pie chart
    QImage canvas( 16, 16, QImage::Format_ARGB32 );
    canvas.fill( Qt::transparent );
    QPainter painter( &canvas );
    painter.setRenderHint( QPainter::Antialiasing, true );
    painter.setPen( QColor ( Qt::black ) );
    painter.drawEllipse( 1, 1, 14, 14 );
    painter.setPen( Qt::NoPen );
    painter.setBrush( QBrush( QColor( Qt::darkGray ) ) );
    painter.drawPie( 2, 2, 12, 12, 1440, -1325 ); // 23 percent of a full circle
    m_icon = QIcon( QPixmap::fromImage( canvas ) );

    // Plugin is enabled by default
    setEnabled( true );

    // Plugin is visible by default on devices with small screens only
    setVisible( MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen );    
}

ProgressFloatItem::~ProgressFloatItem ()
{
    // nothing to do
}

QStringList ProgressFloatItem::backendTypes() const
{
    return QStringList( "progress" );
}

QString ProgressFloatItem::name() const
{
    return tr( "Download Progress Indicator" );
}

QString ProgressFloatItem::guiString() const
{
    return tr( "&Download Progress" );
}

QString ProgressFloatItem::nameId() const
{
    return QString( "progress" );
}

QString ProgressFloatItem::version() const
{
    return "1.0";
}

QString ProgressFloatItem::description() const
{
    return tr( "Shows a pie chart download progress indicator" );
}

QString ProgressFloatItem::copyrightYears() const
{
    return "2010, 2011";
}

QList<PluginAuthor> ProgressFloatItem::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( QString::fromUtf8( "Dennis Nienhüser" ), "earthwings@gentoo.org" )
            << PluginAuthor( "Bernhard Beschow", "bbeschow@cs.tu-berlin.de" );
}

QIcon ProgressFloatItem::icon() const
{
    return m_icon;
}

void ProgressFloatItem::initialize()
{
    const HttpDownloadManager* manager = marbleModel()->downloadManager();
    Q_ASSERT( manager );
    connect( manager, SIGNAL(progressChanged(int,int)), this, SLOT(handleProgress(int,int)) , Qt::UniqueConnection );
    connect( manager, SIGNAL(jobRemoved()), this, SLOT(removeProgressItem()), Qt::UniqueConnection );

    m_isInitialized = true;
}

bool ProgressFloatItem::isInitialized() const
{
    return m_isInitialized;
}

QPainterPath ProgressFloatItem::backgroundShape() const
{
    QPainterPath path;

    if ( active() ) {
        // Circular shape if active, invisible otherwise
        QRectF rect = contentRect();
        qreal width = rect.width();
        qreal height = rect.height();
        path.addEllipse( marginLeft() + 2 * padding(), marginTop() + 2 * padding(), width, height );
    }

    return path;
}

void ProgressFloatItem::paintContent( QPainter *painter )
{
    // Stop repaint timer if it is already running
    m_repaintTimer.stop();

    if ( !active() ) {
        return;
    }

    painter->save();

    qreal completed = 1.0;
    if ( m_totalJobs && m_completedJobs <= m_totalJobs ) {
        completed = (qreal) m_completedJobs / (qreal) m_totalJobs;

        if ( m_completedJobs == m_totalJobs ) {
            m_progressShowTimer.stop();
            m_progressHideTimer.start();
        }
    }

    // Paint progress pie
    int startAngle =  90 * 16; // 12 o' clock
    int spanAngle = -ceil ( 360 * 16 * completed );
    QRectF rect( contentRect() );
    rect.adjust( 1, 1, -1, -1 );

    painter->setBrush( QColor( Qt::white ) );
    painter->setPen( Qt::NoPen );
    painter->drawPie( rect, startAngle, spanAngle );

    // Calculate font size
    QFont myFont = font();
    if ( m_fontSize == 0 ) {
        QString text = "100%";
        int fontSize = myFont.pointSize();
        while( QFontMetrics( myFont ).boundingRect( text ).width() < rect.width() - 4 ) {
            ++fontSize;
            myFont.setPointSize( fontSize );
        }
        m_fontSize = fontSize - 1;
    }

    // Paint progress label
    myFont.setPointSize( m_fontSize );
    QString done = QString::number( (int) ( completed * 100 ) ) + '%';
    int fontWidth = QFontMetrics( myFont ).boundingRect( done ).width();
    QPointF baseline( padding() + 0.5 * ( rect.width() - fontWidth ), 0.75 * rect.height() );
    QPainterPath path;
    path.addText( baseline, myFont, done );

    painter->setFont( myFont );
    painter->setBrush( QBrush() );
    painter->setPen( QPen() );
    painter->drawPath( path );

    painter->restore();
}

bool ProgressFloatItem::eventFilter(QObject *object, QEvent *e)
{
    if ( !enabled() || !visible() ) {
        return false;
    }

    return AbstractFloatItem::eventFilter( object, e );
}

void ProgressFloatItem::removeProgressItem()
{
    m_jobMutex.lock();
    ++m_completedJobs;
    m_jobMutex.unlock();

    if ( enabled() ) {
        if ( !active() && !m_progressShowTimer.isActive() ) {
            m_progressShowTimer.start();
            m_progressHideTimer.stop();
        } else if ( active() ) {
            update();
            scheduleRepaint();
        }
    }
}

void ProgressFloatItem::handleProgress( int current, int queued )
{
    m_jobMutex.lock();
    if ( current < 1 ) {
        m_totalJobs = 0;
        m_completedJobs = 0;
    } else {
        m_totalJobs = qMax<int>( m_totalJobs, queued + current );
    }
    m_jobMutex.unlock();

    if ( enabled() ) {
        if ( !active() && !m_progressShowTimer.isActive() && m_totalJobs > 0 ) {
            m_progressShowTimer.start();
            m_progressHideTimer.stop();
        } else if ( active() ) {
            if ( m_totalJobs < 1 ) {
                m_progressHideTimer.start();
            }
            update();
            scheduleRepaint();
        }
    }
}

void ProgressFloatItem::hideProgress()
{
    if ( enabled() ) {
        setActive( false );

        update();
        emit repaintNeeded( QRegion() );
    }
}

bool ProgressFloatItem::active() const
{
    return m_active;
}

void ProgressFloatItem::setActive( bool active )
{
    m_active = active;
    update();
}

void ProgressFloatItem::show()
{
    setActive( true );

    update();
    emit repaintNeeded( QRegion() );
}

void ProgressFloatItem::scheduleRepaint()
{
    if ( !m_repaintTimer.isActive() ) {
        m_repaintTimer.start();
    }
}

}

Q_EXPORT_PLUGIN2( ProgressFloatItem, Marble::ProgressFloatItem )

#include "ProgressFloatItem.moc"
