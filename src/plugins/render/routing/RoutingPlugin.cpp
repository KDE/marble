//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Siddharth Srivastava <akssps011@gmail.com>
//

#include "RoutingPlugin.h"

#include "ui_RoutingItemWidget.h"

#include "MarbleWidget.h"
#include "routing/RoutingManager.h"
#include "routing/RoutingModel.h"
#include "MarbleDirs.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "MarbleMap.h"
#include "MarbleDataFacade.h"
#include "gps/PositionTracking.h"
#include "GeoPainter.h"
#include "ViewportParams.h"
#include "WidgetGraphicsItem.h"
#include "MarbleGraphicsGridLayout.h"
#include "global.h"

#include <QtGui/QIcon>
#include <QtCore/QRect>
#include <QtGui/QColor>
#include <QtGui/QPixmap>
#include <QtGui/QWidget>

using namespace Marble;

RoutingPlugin::RoutingPlugin( const QPointF &point )
      :AbstractFloatItem( point ),
       m_routingManager( 0 ),
       m_routingModel( 0 ),
       m_remainingTime( 0.0 ),
       m_remainingDistance( 0.0 ),
       m_widgetItem( 0 ),
       m_routingItem( 0 )
{
    setEnabled( true );
    setVisible( false );
}

RoutingPlugin::~RoutingPlugin ()
{

}

QStringList RoutingPlugin::backendTypes() const
{
    return QStringList( "routing" );
}

QString RoutingPlugin::name() const
{
    return tr( "Routing" );
}

QString RoutingPlugin::guiString() const
{
    return tr( "&Routing" );
}

QString RoutingPlugin::nameId() const
{
    return QString("routing");
}

QString RoutingPlugin::description() const
{
    return tr( "Shows the information about the route" );
}

QIcon RoutingPlugin::icon() const
{
    return QIcon();
}

void RoutingPlugin::initialize()
{
    QWidget *widget = new QWidget( 0 );
    m_routingItem = new Ui::RoutingItemWidget;
    m_routingItem->setupUi( widget );
    m_widgetItem = new WidgetGraphicsItem( this );
    m_widgetItem->setWidget( widget );
    MarbleGraphicsGridLayout *gridLayout = new MarbleGraphicsGridLayout( 1, 1 );
    gridLayout->addItem( m_widgetItem ,0 ,0 );
    setLayout( gridLayout );
}

bool RoutingPlugin::isInitialized() const
{
    return m_widgetItem;
}

void RoutingPlugin::setDestinationInformation( qreal remainingTime, qreal remainingDistance )
{
    m_remainingTime = remainingTime;
    m_remainingDistance = remainingDistance;
}

bool RoutingPlugin::eventFilter(QObject *object, QEvent *e)
{
    if( m_routingManager )    {
        return AbstractFloatItem::eventFilter( object, e );
    }

    MarbleWidget *marbleWidget = dynamic_cast<MarbleWidget*> ( object );
    m_routingManager = dataFacade()->routingManager();
    m_routingModel = m_routingManager->routingModel();

    connect( m_routingModel, SIGNAL( nextInstruction( qreal, qreal ) ),
            this, SLOT( setDestinationInformation( qreal, qreal ) ) );

    PositionTracking *tracking = marbleWidget->map()->model()->positionTracking();
    connect( tracking, SIGNAL( gpsLocation( GeoDataCoordinates, qreal ) ),
             this, SLOT( setCurrentLocation( GeoDataCoordinates, qreal ) ) );

    return AbstractFloatItem::eventFilter( object, e );
}

void RoutingPlugin::setCurrentLocation( GeoDataCoordinates position, qreal speed )
{
    Q_UNUSED(speed);
    m_currentPosition = position;
    bool hasRoute = m_routingModel->rowCount() != 0;
    setVisible( hasRoute );
    if ( hasRoute ) {
        showRoutingItem();
    }
}

void RoutingPlugin::showRoutingItem()
{
    if( m_remainingTime < 60 ) {
        m_routingItem->timeUnitLabel->setText( tr( "Minutes" ) );
        m_routingItem->remainingTimeLabel->setText( QString::number( m_remainingTime, 'f', 2 ) );
    }
    else {
        m_routingItem->timeUnitLabel->setText( tr( "Hours" ) );
        m_routingItem->remainingTimeLabel->setText( QString::number( m_remainingTime * MIN2HOUR, 'f', 2 ) );
    }

    if( m_remainingDistance < 1000 ) {
        m_routingItem->distanceUnitLabel->setText( tr( "Metres" ) );
        m_routingItem->remainingDistanceLabel->setText( QString::number( m_remainingDistance, 'f', 3 ) );
    }
    else {
        m_routingItem->distanceUnitLabel->setText( tr( "KM" ) );
        m_routingItem->remainingDistanceLabel->setText( QString::number( m_remainingDistance * METER2KM , 'f', 3 ) );
    }

    qreal totalDistance =  ( m_routingManager->routingModel()->totalDistance() ) ;
    if( !m_routingModel->deviatedFromRoute() && m_remainingDistance !=0 && m_remainingTime != 0 ) {
        m_routingItem->distanceCoveredProgressBar->setRange( 0, qRound( totalDistance ) );
        m_routingItem->distanceCoveredProgressBar->setValue( qRound ( totalDistance - m_remainingDistance ) );
    }
    if ( m_remainingDistance == 0 && m_remainingTime == 0 ) {
        m_routingItem->distanceCoveredProgressBar->setValue( qRound( totalDistance ) );
    }
}

Q_EXPORT_PLUGIN2( RoutingPlugin, Marble::RoutingPlugin )

#include "RoutingPlugin.moc"
