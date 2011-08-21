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

#include <locale.h>

#include "sgp4/sgp4io.h"

#include "MarbleDebug.h"

#include "SatellitesItem.h"

#include <QtCore/QStringList>
#include <QtCore/QUrl>

using namespace Marble;

SatellitesModel::SatellitesModel( Marble::PluginManager* pluginManager, QObject* parent )
    : AbstractDataPluginModel( "satellites", pluginManager, parent )
{
}

void SatellitesModel::getAdditionalItems( const Marble::GeoDataLatLonAltBox& box, const Marble::MarbleModel* model, qint32 number )
{
    // Nothing to do, the datasets are world wide
    return;
}

void SatellitesModel::refreshItems(const QStringList &tleList)
{
    clear();

    foreach ( const QString &tle, tleList ) {
        downloadDescriptionFile( QUrl( "http://www.celestrak.com/NORAD/elements/" + tle ) );
    }
}


void SatellitesModel::parseFile( const QByteArray &file )
{
    //FIXME: terrible hack because twoline2rv uses sscanf
    setlocale(LC_NUMERIC, "C");

    QList<QByteArray> tleLines = file.split( '\n' );
    double startmfe, stopmfe, deltamin;
    elsetrec satrec;
    int i = 0;
    // File format: One line of description, two lines of TLE, last line is empty
    if ( tleLines.size() % 3 != 1 ) {
        mDebug() << "Malformated satellite data file";
    }
    while ( i < tleLines.size() - 1 ) {
        QString satelliteName( tleLines.at( i++ ) );
        char line1[80];
        char line2[80];
        qstrcpy( line1, tleLines.at( i++ ).constData() );
        qstrcpy( line2, tleLines.at( i++ ).constData() );
        twoline2rv( line1, line2, 'c', 'd', 'i', wgs84,
                    startmfe, stopmfe, deltamin, satrec );
        if ( satrec.error != 0 ) {
            mDebug() << "Error: " << satrec.error;
            return;
        }
        addItemToList( new SatellitesItem( satelliteName.trimmed(), satrec, this ) );
    }

    //Reset to environment
    setlocale(LC_NUMERIC, "");

    emit itemsUpdated();
}

#include "SatellitesModel.moc"
