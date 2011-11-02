//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "Speedometer.h"

#include "MarbleDebug.h"
#include "MarbleLocale.h"
#include "MarbleModel.h"
#include "MarbleWidget.h"
#include "PositionTracking.h"
#include "WidgetGraphicsItem.h"
#include "MarbleGraphicsGridLayout.h"
#include "ViewportParams.h"

#include <QtGui/QLCDNumber>

namespace Marble
{

Speedometer::Speedometer( const QPointF &point, const QSizeF &size )
    : AbstractFloatItem( point, size ),
      m_marbleWidget( 0 ),
      m_widgetItem( 0 )
{
    setVisible( false );
    setCacheMode( NoCache );
}

Speedometer::~Speedometer()
{
}

QStringList Speedometer::backendTypes() const
{
    return QStringList( "speedometer" );
}

QString Speedometer::name() const
{
    return tr( "Speedometer" );
}

QString Speedometer::guiString() const
{
    return tr( "&Speedometer" );
}

QString Speedometer::nameId() const
{
    return QString( "speedometer" );
}

QString Speedometer::description() const
{
    return tr( "A float item showing current travelling speed." );
}

QIcon Speedometer::icon () const
{
    return QIcon();
}

void Speedometer::initialize ()
{
    if ( !m_widgetItem ) {
        QWidget *widget = new QWidget;
        m_widget.setupUi( widget );
        m_widgetItem = new WidgetGraphicsItem( this );
        m_widgetItem->setWidget( widget );
        m_widgetItem->setCacheMode( MarbleGraphicsItem::DeviceCoordinateCache );

        MarbleGraphicsGridLayout *layout = new MarbleGraphicsGridLayout( 1, 1 );
        layout->addItem( m_widgetItem, 0, 0 );
        setLayout( layout );
        setPadding( 0 );

        m_locale = MarbleGlobal::getInstance()->locale();
        connect( marbleModel()->positionTracking(), SIGNAL( gpsLocation(GeoDataCoordinates,qreal) ),
                this, SLOT( updateLocation(GeoDataCoordinates,qreal) ) );
    }
}

bool Speedometer::isInitialized () const
{
    return m_widgetItem;
}

bool Speedometer::eventFilter( QObject *object, QEvent *e )
{
    if ( m_marbleWidget || !enabled() || !visible() ) {
        return AbstractFloatItem::eventFilter( object, e );
    }

    MarbleWidget *widget = qobject_cast<MarbleWidget*>( object );

    if ( widget ) {
        m_marbleWidget = widget;
    }

    return AbstractFloatItem::eventFilter( object, e );
}


void Speedometer::forceRepaint()
{
    m_widgetItem->update();
    if ( m_marbleWidget ) {
        // Trigger a repaint of the float item.
        m_marbleWidget->setAttribute( Qt::WA_NoSystemBackground, false );
        update();
        m_marbleWidget->repaint();
        bool const mapCoversViewport = m_marbleWidget->viewport()->mapCoversViewport();
        m_marbleWidget->setAttribute( Qt::WA_NoSystemBackground, mapCoversViewport );
    }
}

void Speedometer::updateLocation( GeoDataCoordinates coordinates, qreal)
{
    PositionTracking *tracking = marbleModel()->positionTracking();
    qreal speed = tracking->speed();
    QString speedUnit;
    QString distanceString;

    switch ( m_locale->distanceUnit() ) {
    case MilesFeet:
        //miles per hour
        speedUnit = tr("mph");
        speed *= KM2MI;
        distanceString = tr("ft");
        break;

    default:
        //kilometers per hour
        speedUnit = tr("km/h");
        distanceString = tr("m");
        break;
    }

    m_widget.speed->display( speed );
    m_widget.speedUnit->setText( speedUnit );
    forceRepaint();
}

}

Q_EXPORT_PLUGIN2( Speedometer, Marble::Speedometer )

#include "Speedometer.moc"
