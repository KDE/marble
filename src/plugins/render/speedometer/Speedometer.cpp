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

#include "MarbleGlobal.h"
#include "MarbleDebug.h"
#include "MarbleLocale.h"
#include "MarbleModel.h"
#include "PositionTracking.h"
#include "WidgetGraphicsItem.h"
#include "MarbleGraphicsGridLayout.h"
#include "ViewportParams.h"

namespace Marble
{

Speedometer::Speedometer()
    : AbstractFloatItem( 0 ),
      m_locale( 0 ),
      m_widgetItem( 0 )
{
}

Speedometer::Speedometer( const MarbleModel *marbleModel )
    : AbstractFloatItem( marbleModel, QPointF( 10.5, 110 ), QSizeF( 135.0, 80.0 ) ),
      m_locale( 0 ),
      m_widgetItem( 0 )
{
    setVisible( false );

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
    return QStringList(QStringLiteral("speedometer"));
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
    return QStringLiteral("speedometer");
}

QString Speedometer::version() const
{
    return QStringLiteral("1.0");
}

QString Speedometer::description() const
{
    return tr( "Display the current cruising speed." );
}

QString Speedometer::copyrightYears() const
{
    return QStringLiteral("2011");
}

QVector<PluginAuthor> Speedometer::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Bernhard Beschow"), QStringLiteral("bbeschow@cs.tu-berlin.de"));
}

QIcon Speedometer::icon () const
{
    return QIcon(QStringLiteral(":/icons/speedometer.png"));
}

void Speedometer::initialize ()
{
    if ( !m_widgetItem ) {
        QWidget *widget = new QWidget;
        m_widget.setupUi( widget );
        m_widgetItem = new WidgetGraphicsItem( this );
        m_widgetItem->setWidget( widget );

        MarbleGraphicsGridLayout *layout = new MarbleGraphicsGridLayout( 1, 1 );
        layout->addItem( m_widgetItem, 0, 0 );
        setLayout( layout );
        setPadding( 0 );

        m_locale = MarbleGlobal::getInstance()->locale();
        connect( marbleModel()->positionTracking(), SIGNAL(gpsLocation(GeoDataCoordinates,qreal)),
                this, SLOT(updateLocation(GeoDataCoordinates,qreal)) );
    }
}

bool Speedometer::isInitialized () const
{
    return m_widgetItem;
}

void Speedometer::updateLocation( const GeoDataCoordinates& coordinates, qreal speed )
{
    Q_UNUSED( coordinates );

    speed *= METER2KM / SEC2HOUR;
    QString speedUnit;

    switch ( m_locale->measurementSystem() ) {
    case MarbleLocale::ImperialSystem:
        //miles per hour
        speedUnit = tr("mph");
        speed *= KM2MI;
        break;

    case MarbleLocale::MetricSystem:
        //kilometers per hour
        speedUnit = tr("km/h");
        break;

    case MarbleLocale::NauticalSystem:
        // nm per hour (kt)
        speedUnit = tr("kt");
        speed *= KM2NM;
        break;
    }

    m_widget.speed->display( speed );
    m_widget.speedUnit->setText( speedUnit );

    update();
    emit repaintNeeded();
}

}

#include "moc_Speedometer.cpp"
