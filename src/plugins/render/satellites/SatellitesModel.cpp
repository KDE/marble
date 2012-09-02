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
#include "SatellitesCatalogItem.h"
#include "SatellitesTLEItem.h"

#include "MarbleClock.h"
#include "GeoDataPlacemark.h"

#include "sgp4/sgp4io.h"
#include "mex/planetarySats.h"

#include <locale.h>

namespace Marble {

SatellitesModel::SatellitesModel( GeoDataTreeModel *treeModel,
                                  const PluginManager *pluginManager,
                                  const MarbleClock *clock )
    : TrackerPluginModel( treeModel, pluginManager ),
      m_clock( clock )
{
    connect(m_clock, SIGNAL(timeChanged()), this, SLOT(update()));
}

void SatellitesModel::loadSettings( const QHash<QString, QVariant> &settings )
{
    QStringList idList = settings["idList"].toStringList();
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

    foreach( QObject *obj, items() ) {
        SatellitesCatalogItem *oItem = qobject_cast<SatellitesCatalogItem*>(obj);
        if( oItem != NULL ) {
            bool enabled = ( ( oItem->relatedBody().toLower() == m_lcPlanet ) &&
                             ( m_enabledIds.contains( oItem->id() ) ) );
            oItem->setEnabled( enabled );

            if( enabled ) {
                oItem->update();
            }
        }

        SatellitesTLEItem *eItem = qobject_cast<SatellitesTLEItem*>(obj);
        if( eItem != NULL ) {
            // TLE satellites are always earth satellites
            bool enabled = ( m_lcPlanet == "earth" );
            eItem->setEnabled( enabled );

            if( enabled ) {
                eItem->update();
            }
        }
    }

    endUpdateItems();
}

void SatellitesModel::parseFile( const QString &id,
                                 const QByteArray &file )
{
    // catalog files are comma serparated while tle files
    // are not allowed to contain commas, so we use this
    // to distinguish between those two
    if( file.contains( ',' ) ) {
        parseCatalog( id, file );
    } else {
        parseTLE( id, file );
    }

    emit fileParsed( id );
}

void SatellitesModel::parseCatalog( const QString &id,
                                    const QByteArray &file )
{
    mDebug() << "Reading satellite catalog from:" << id;

    QTextStream ts(file);
    int index = 1;

    beginUpdateItems();

    QString line = ts.readLine();
    for( ; !line.isNull(); line = ts.readLine() ) {

        if( line.trimmed().startsWith( "#" ) ) {
            continue;
        }

        QStringList elms = line.split(", ");

        if( elms.size() != 13 ) {
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
            elms[6].toFloat() - 2400000.5,
            elms[7].toFloat(),  elms[8].toFloat(),  elms[9].toFloat(),
            elms[10].toFloat(), elms[11].toFloat(), elms[12].toFloat() );

        planSat->stateToKepler();

        SatellitesCatalogItem *item;
        item = new SatellitesCatalogItem( name, category, body, id, index++,
                                          planSat, m_clock );
        item->placemark()->setVisible( ( body.toLower() == m_lcPlanet ) );

        addItem( item );
    }

    endUpdateItems();
}

void SatellitesModel::parseTLE( const QString &id,
                                const QByteArray &file )
{
    mDebug() << "Reading satellite TLE data from:" << id;

    QList<QByteArray> tleLines = file.split( '\n' );
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
        char line1[80];
        char line2[80];
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

        SatellitesTLEItem *item = new SatellitesTLEItem( satelliteName,
                                                         satrec, m_clock );
        addItem( item );
    }

    //Reset to environment
    setlocale( LC_NUMERIC, "" );

    endUpdateItems();
}

} // namespace Marble

#include "SatellitesModel.moc"
