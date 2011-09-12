//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>
//

#include "GpsInfo.h"

#include "ui_GpsInfoPlugin.h"

#include "MarbleDebug.h"
#include "MarbleLocale.h"
#include "MarbleModel.h"
#include "MarbleWidget.h"
#include "PositionTracking.h"
#include "WidgetGraphicsItem.h"
#include "MarbleGraphicsGridLayout.h"
#include "ViewportParams.h"

namespace Marble
{

GpsInfo::GpsInfo( const QPointF &point, const QSizeF &size )
    : AbstractFloatItem( point, size ),
      m_marbleWidget( 0 ),
      m_widgetItem( 0 )
{
    setCacheMode( NoCache );
}

GpsInfo::~GpsInfo()
{
}

QStringList GpsInfo::backendTypes() const
{
    return QStringList( "GpsInfo" );
}

QString GpsInfo::name() const
{
    return tr("GpsInfo");
}

QString GpsInfo::guiString() const
{
    return tr("&GpsInfo");
}

QString GpsInfo::nameId() const
{
    return QString( "GpsInfo" );
}

QString GpsInfo::description() const
{
    return tr("This is a float item that provides Gps Information.");
}

QIcon GpsInfo::icon () const
{
    return QIcon();
}

void GpsInfo::initialize ()
{
    if ( !m_widgetItem )
    {
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

bool GpsInfo::isInitialized () const
{
    return m_widgetItem;
}

bool GpsInfo::eventFilter( QObject *object, QEvent *e )
{
    if ( m_marbleWidget || !enabled() || !visible() ) {
        return AbstractFloatItem::eventFilter( object, e );
    }

    MarbleWidget *widget = dynamic_cast<MarbleWidget*> ( object );

    if ( widget ) {
        m_marbleWidget = widget;
    }

    return AbstractFloatItem::eventFilter( object, e );
}


void GpsInfo::forceRepaint()
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

void GpsInfo::updateLocation( GeoDataCoordinates coordinates, qreal)
{
    PositionTracking *tracking = marbleModel()->positionTracking();
    qreal speed = tracking->speed();
    qreal direction = tracking->direction();
    qreal altitude = coordinates.altitude();
    qreal precision = tracking->accuracy().horizontal;
    QString speedString;
    QString distanceString;

    switch ( m_locale->distanceUnit() ) {
    case MilesFeet:
        //miles per hour
        speedString = tr("mph");
        speed *= HOUR2SEC * METER2KM * KM2MI;
        distanceString = tr("ft");
        altitude *= M2FT;
        precision *= M2FT;
        break;

    default:
        //kilometers per hour
        speedString = tr("km/h");
        speed *= HOUR2SEC * METER2KM;
        distanceString = tr("m");
        break;
    }

    m_widget.SpeedValue->setText( QString( " %1 %2" )
                                 .arg( QLocale().toString(speed, 'f', 1 ) )
                                 .arg( speedString ) );
    m_widget.AltitudeValue->setText( QString( " %1 %2" )
                                    .arg( QLocale().toString(altitude, 'f', 1 ) )
                                    .arg( distanceString ) );
    m_widget.DirectionValue->setText( QString( " %1 %2" )
                                     .arg( QLocale().toString(direction, 'f', 1 ) )
                                     .arg( "d" ) );
    m_widget.PrecisionValue->setText( QString( " %1 %2" )
                                     .arg( QLocale().toString(precision, 'f', 1 ) )
                                     .arg( distanceString ) );
    forceRepaint();
}

}

Q_EXPORT_PLUGIN2( GpsInfo, Marble::GpsInfo )

#include "GpsInfo.moc"
