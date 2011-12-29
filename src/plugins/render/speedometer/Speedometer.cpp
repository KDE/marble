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
#include "PluginAboutDialog.h"

#include <QtGui/QLCDNumber>

namespace Marble
{

Speedometer::Speedometer( const QPointF &point, const QSizeF &size )
    : AbstractFloatItem( point, size ),
      m_widgetItem( 0 ), m_aboutDialog( 0 )
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
    return tr( "Display the current cruising speed." );
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

void Speedometer::updateLocation( GeoDataCoordinates coordinates, qreal speed )
{
    Q_UNUSED( coordinates );

    speed *= METER2KM / SEC2HOUR;
    QString speedUnit;

    switch ( m_locale->measurementSystem() ) {
    case QLocale::ImperialSystem:
        //miles per hour
        speedUnit = tr("mph");
        speed *= KM2MI;
        break;

    case QLocale::MetricSystem:
        //kilometers per hour
        speedUnit = tr("km/h");
        break;
    }

    m_widget.speed->display( speed );
    m_widget.speedUnit->setText( speedUnit );
    m_widgetItem->update();
    emit repaintNeeded();
}

QDialog *Speedometer::aboutDialog()
{
    if ( !m_aboutDialog ) {
        // Initializing about dialog
        m_aboutDialog = new PluginAboutDialog();
        m_aboutDialog->setName( "Speedometer Plugin" );
        m_aboutDialog->setVersion( "0.1" );
        // FIXME: Can we store this string for all of Marble
        m_aboutDialog->setAboutText( tr( "<br/>(c) 2011 The Marble Project<br /><br/><a href=\"http://edu.kde.org/marble\">http://edu.kde.org/marble</a>" ) );

        QList<Author> authors;
        Author bernhard;
        bernhard.name = QString::fromUtf8( "Bernhard Beschow" );
        bernhard.task = tr( "Developer" );
        bernhard.email = "bbeschow@cs.tu-berlin.de";
        authors.append( bernhard );
        m_aboutDialog->setAuthors( authors );
    }
    return m_aboutDialog;
}

}

Q_EXPORT_PLUGIN2( Speedometer, Marble::Speedometer )

#include "Speedometer.moc"
