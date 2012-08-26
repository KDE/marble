//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Rene Kuettner <rene@bitkanal.net>
//

#include "OrbiterSatellitesModel.h"

#include <QtCore/QUrl>

#include "MarbleDebug.h"
#include "MarbleClock.h"
#include "MarbleDirs.h"

#include "mex/planetarySats.h"

#include "OrbiterSatellitesItem.h"

namespace Marble {

OrbiterSatellitesModel::OrbiterSatellitesModel(
    GeoDataTreeModel *treeModel,
    const PluginManager *pluginManager,
    const MarbleClock *clock )
    : TrackerPluginModel( treeModel, pluginManager ),
      m_clock( clock ),
      m_lcPlanet( QString() ),
      m_catalogs( QString() )
{
    connect( m_clock, SIGNAL( timeChanged() ), this, SLOT( update() ) );
}

OrbiterSatellitesModel::~OrbiterSatellitesModel()
{
}

void OrbiterSatellitesModel::setPlanet( const QString &planetId )
{
    if( m_lcPlanet != planetId ) {

        mDebug() << "Planet changed from" << m_lcPlanet << "to" << planetId;
        m_lcPlanet = planetId;

        // reload catalogs
        clear();
        foreach( const QString &catalog, m_catalogs ) {
            downloadFile( QUrl( catalog ), catalog.section( '/', -1 ) );
        }
    }
}

void OrbiterSatellitesModel::parseFile( const QString &id,
                                        const QByteArray &file )
{
    mDebug() << "Reading orbiter catalog from:" << id;
    QTextStream ts(file);

    QString planet( m_lcPlanet.left(1).toUpper() + m_lcPlanet.mid(1) );
    QByteArray bplanet = planet.toLocal8Bit();
    char *cplanet = const_cast<char*>( bplanet.constData() );

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

        QString name = QString("%1 (%2)").arg( elms[0], elms[1] );
        QString planet( elms[2] );

        if( planet.toLower() != m_lcPlanet ) {
            continue;
        }

        mDebug() << "Loading orbiter object:" << name;

        PlanetarySats *planSat = new PlanetarySats();
        planSat->setPlanet( cplanet );

        planSat->setStateVector(
            elms[6].toFloat() - 2400000.5,
            elms[7].toFloat(),  elms[8].toFloat(),  elms[9].toFloat(),
            elms[10].toFloat(), elms[11].toFloat(), elms[12].toFloat() );

        planSat->stateToKepler();

        addItem( new OrbiterSatellitesItem( name, planSat, m_clock ) );
    }

    endUpdateItems();
}

void OrbiterSatellitesModel::downloadFile(const QUrl &url, const QString &id)
{
    // add url to list of known catalogs
    if( !m_catalogs.contains( url.toString() ) ) {
        m_catalogs.append( url.toString() );
    }

    TrackerPluginModel::downloadFile( url, id );
}

} // namespace Marble

#include "OrbiterSatellitesModel.moc"

