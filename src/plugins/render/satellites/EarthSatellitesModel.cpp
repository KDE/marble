//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#include "EarthSatellitesModel.h"

#include "MarbleDebug.h"
#include "EarthSatellitesItem.h"
#include "MarbleClock.h"

#include "sgp4/sgp4io.h"
#include <locale.h>

namespace Marble {

EarthSatellitesModel::EarthSatellitesModel( GeoDataTreeModel *treeModel,
                                            const PluginManager *pluginManager,
                                            const MarbleClock *clock )
    : TrackerPluginModel( treeModel, pluginManager ),
      m_clock( clock )
{
    connect(m_clock, SIGNAL(timeChanged()),
            this, SLOT(update()));
}

void EarthSatellitesModel::parseFile( const QString &id,
                                      const QByteArray &file )
{
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

        EarthSatellitesItem *item = new EarthSatellitesItem( satelliteName,
                                                             satrec, m_clock );
        addItem( item );
    }

    //Reset to environment
    setlocale( LC_NUMERIC, "" );

    endUpdateItems();
    emit fileParsed( id );
}

} // namespace Marble

#include "EarthSatellitesModel.moc"
