//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#include "SatellitesModel.h"

#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "SatellitesMSCItem.h"
#include "SatellitesTLEItem.h"

#include "MarbleClock.h"
#include "GeoDataPlacemark.h"
#include "GeoDataStyle.h"
#include "GeoDataIconStyle.h"
#include "GeoDataLabelStyle.h"
#include "GeoDataLineStyle.h"

#include <planetarySats.h>
#include <sgp4io.h>

#include <clocale>

namespace Marble {

SatellitesModel::SatellitesModel( GeoDataTreeModel *treeModel,
                                  const MarbleClock *clock )
    : TrackerPluginModel( treeModel ),
      m_clock( clock ),
      m_currentColorIndex( 0 )
{
    setupColors();
    connect(m_clock, SIGNAL(timeChanged()), this, SLOT(update()));
}

void SatellitesModel::setupColors()
{
    m_colorList.push_back( Oxygen::brickRed4 );
    m_colorList.push_back( Oxygen::raspberryPink4 );
    m_colorList.push_back( Oxygen::burgundyPurple4 );
    m_colorList.push_back( Oxygen::grapeViolet4 );
    m_colorList.push_back( Oxygen::skyBlue4 );
    m_colorList.push_back( Oxygen::seaBlue4 );
    m_colorList.push_back( Oxygen::emeraldGreen4 );
    m_colorList.push_back( Oxygen::forestGreen4 );
    m_colorList.push_back( Oxygen::sunYellow4 );
    m_colorList.push_back( Oxygen::hotOrange4 );
    m_colorList.push_back( Oxygen::aluminumGray4 );
    m_colorList.push_back( Oxygen::woodBrown4 );
}

QColor SatellitesModel::nextColor()
{
    if (m_colorList.isEmpty()) {
        return Oxygen::brickRed4;
    }
    if (m_currentColorIndex < m_colorList.size()) {
        m_currentColorIndex++;
        return m_colorList[m_currentColorIndex-1];
    } else {
        m_currentColorIndex = 1;
        return m_colorList[0];
    }
    return Oxygen::brickRed4;
}

void SatellitesModel::loadSettings( const QHash<QString, QVariant> &settings )
{
    QStringList idList = settings[QStringLiteral("idList")].toStringList();
    m_enabledIds = idList;

    updateVisibility();
}

void SatellitesModel::setPlanet( const QString &planetId )
{
    if( m_lcPlanet != planetId ) {

        mDebug() << "Planet changed from" << m_lcPlanet << "to" << planetId;
        m_lcPlanet = planetId;

        updateVisibility();
    }
}

void SatellitesModel::updateVisibility()
{
    beginUpdateItems();

    for( TrackerPluginItem *obj: items() ) {
        SatellitesMSCItem *oItem = dynamic_cast<SatellitesMSCItem*>(obj);
        if( oItem != NULL ) {
            bool enabled = ( ( oItem->relatedBody().toLower() == m_lcPlanet ) &&
                             ( m_enabledIds.contains( oItem->id() ) ) );
            oItem->setEnabled( enabled );

            if( enabled ) {
                oItem->update();
            }
        }

        SatellitesTLEItem *eItem = dynamic_cast<SatellitesTLEItem*>(obj);
        if( eItem != NULL ) {
            // TLE satellites are always earth satellites
            bool enabled = (m_lcPlanet == QLatin1String("earth"));
            eItem->setEnabled( enabled );

            if( enabled ) {
                eItem->update();
            }
        }
    }

    endUpdateItems();
}

void SatellitesModel::parseFile( const QString &id,
                                 const QByteArray &data )
{
    // catalog files are comma serparated while tle files
    // are not allowed to contain commas, so we use this
    // to distinguish between those two
    if( data.contains( ',' ) ) {
        parseCatalog( id, data );
    } else {
        parseTLE( id, data );
    }

    emit fileParsed( id );
}

void SatellitesModel::parseCatalog( const QString &id,
                                    const QByteArray &data )
{
    // For details see:
    // http://techbase.kde.org/Projects/Marble/SatelliteCatalogFormat

    mDebug() << "Reading satellite catalog from:" << id;

    QTextStream ts(data);
    int index = 1;

    beginUpdateItems();

    QString line = ts.readLine();
    for( ; !line.isNull(); line = ts.readLine() ) {

        if (line.trimmed().startsWith(QLatin1Char('#'))) {
            continue;
        }

        QStringList elms = line.split(", ");

        // check for '<' instead of '==' in order to allow fields to be added
        // to catalogs later without breaking the code
        if( elms.size() < 14 ) {
            mDebug() << "Skipping line:" << elms << "(" << line << ")";
            continue;
        }

        QString name( elms[0] );
        QString category( elms[1] );
        QString body( elms[2] );
        QByteArray body8Bit = body.toLocal8Bit();
        char *cbody = const_cast<char*>( body8Bit.constData() );

        mDebug() << "Loading" << category << name;

        PlanetarySats *planSat = new PlanetarySats();
        planSat->setPlanet( cbody );

        planSat->setStateVector(
            elms[7].toFloat() - 2400000.5,
            elms[8].toFloat(),  elms[9].toFloat(),  elms[10].toFloat(),
            elms[11].toFloat(), elms[12].toFloat(), elms[13].toFloat() );

        planSat->stateToKepler();

        QDateTime missionStart, missionEnd;
        if( elms[3].toUInt() > 0 ) {
            missionStart = QDateTime::fromTime_t( elms[3].toUInt() );
        }
        if( elms[4].toUInt() > 0 ) {
            missionEnd = QDateTime::fromTime_t( elms[4].toUInt() );
        }

        SatellitesMSCItem *item = new SatellitesMSCItem( name, category, body, id,
                                      missionStart, missionEnd,
                                      index++, planSat, m_clock );
        GeoDataStyle::Ptr style(new GeoDataStyle( *item->placemark()->style() ));
        style->lineStyle().setPenStyle( Qt::SolidLine );
        style->lineStyle().setColor( nextColor() );
        style->labelStyle().setGlow( true );

        // use special icon for moons
        if (category == QLatin1String("Moons")) {
            style->iconStyle().setIconPath(QStringLiteral(":/icons/moon.png"));
        } else {
            style->iconStyle().setIconPath(MarbleDirs::path(QStringLiteral("bitmaps/satellite.png")));
        }

        item->placemark()->setStyle( style );

        item->placemark()->setVisible( ( body.toLower() == m_lcPlanet ) );
        addItem( item );
    }

    endUpdateItems();
}

void SatellitesModel::parseTLE( const QString &id,
                                const QByteArray &data )
{
    mDebug() << "Reading satellite TLE data from:" << id;

    QList<QByteArray> tleLines = data.split( '\n' );
    // File format: One line of description, two lines of TLE, last line is empty
    if ( tleLines.size() % 3 != 1 ) {
        mDebug() << "Malformated satellite data file";
    }

    beginUpdateItems();

    //FIXME: terrible hack because twoline2rv uses sscanf
    setlocale( LC_NUMERIC, "C" );

    double startmfe, stopmfe, deltamin;
    elsetrec satrec;
    int i = 0;
    while ( i < tleLines.size() - 1 ) {
        QString satelliteName = QString( tleLines.at( i++ ) ).trimmed();
        char line1[130];
        char line2[130];
        if( tleLines.at( i ).size() >= 79  ||
            tleLines.at( i+1 ).size() >= 79 ) {
            mDebug() << "Invalid TLE data!";
            return;
        }
        qstrcpy( line1, tleLines.at( i++ ).constData() );
        qstrcpy( line2, tleLines.at( i++ ).constData() );
        twoline2rv( line1, line2, 'c', 'd', 'i', wgs84,
                    startmfe, stopmfe, deltamin, satrec );
        if ( satrec.error != 0 ) {
            mDebug() << "Error: " << satrec.error;
            return;
        }

        SatellitesTLEItem *item = new SatellitesTLEItem( satelliteName, satrec, m_clock );
        GeoDataStyle::Ptr style(new GeoDataStyle( *item->placemark()->style() ));
        style->lineStyle().setPenStyle( Qt::SolidLine );
        style->lineStyle().setColor( nextColor() );
        style->labelStyle().setGlow( true );
        style->iconStyle().setIconPath(MarbleDirs::path(QStringLiteral("bitmaps/satellite.png")));
        item->placemark()->setStyle( style );
        addItem( item );
    }

    //Reset to environment
    setlocale( LC_NUMERIC, "" );

    endUpdateItems();
}

} // namespace Marble

#include "moc_SatellitesModel.cpp"
