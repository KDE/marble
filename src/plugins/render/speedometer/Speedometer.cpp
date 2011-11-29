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

#include "global.h"
#include "MarbleDebug.h"
#include "MarbleLocale.h"
#include "MarbleModel.h"
#include "PositionTracking.h"
#include "WidgetGraphicsItem.h"
#include "MarbleGraphicsGridLayout.h"
#include "ViewportParams.h"

#include <QtGui/QLCDNumber>

namespace Marble
{

Speedometer::Speedometer( const QPointF &point, const QSizeF &size )
    : AbstractFloatItem( point, size ),
      m_widgetItem( 0 )
{
    setVisible( false );
    setCacheMode( NoCache );

    const bool smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    if ( smallScreen ) {
        setPosition( QPointF( 10.5, 10.5 ) );
    }
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

void Speedometer::updateLocation( GeoDataCoordinates coordinates, qreal)
{
    Q_UNUSED( coordinates );

    const PositionTracking *tracking = marbleModel()->positionTracking();
    qreal speed = tracking->speed();
    QString speedUnit;
    QString distanceString;

    switch ( m_locale->measureSystem() ) {
    case QLocale::ImperialSystem:
        //miles per hour
        speedUnit = tr("mph");
        speed *= KM2MI;
        distanceString = tr("ft");
        break;

    case QLocale::MetricSystem:
        //kilometers per hour
        speedUnit = tr("km/h");
        distanceString = tr("m");
        break;
    }

    m_widget.speed->display( speed );
    m_widget.speedUnit->setText( speedUnit );
    m_widgetItem->update();
    emit repaintNeeded();
}

}

Q_EXPORT_PLUGIN2( Speedometer, Marble::Speedometer )

#include "Speedometer.moc"
