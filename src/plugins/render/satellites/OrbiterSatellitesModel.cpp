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
      m_lcPlanet( QString() )
{
    connect( m_clock, SIGNAL( timeChanged() ), this, SLOT( update() ) );
}

OrbiterSatellitesModel::~OrbiterSatellitesModel()
{
}

void OrbiterSatellitesModel::setPlanet( const QString &planetId )
{
    if( m_lcPlanet != planetId ) {

        qDebug() << "Planet changed from" << m_lcPlanet << "to" << planetId;
        m_lcPlanet = planetId;

        // FIXME placeholder until a final location for the data has
        //       been chosen
        parseFile( MarbleDirs::localPath() + "/cache/planetarysats.txt",
            QByteArray() );
   }
}

void OrbiterSatellitesModel::parseFile( const QString &id,
                                        const QByteArray &file )
{
    Q_UNUSED( file );

    clear();

    QString planet( m_lcPlanet.left(1).toUpper() + m_lcPlanet.mid(1) );
    char *cplanet = planet.toLocal8Bit().data();

    PlanetarySats *planSat = new PlanetarySats();
    planSat->setPlanet( cplanet );
    qDebug() << "Using planet" << planet;

    //QString path = MarbleDirs::localPath() + "/cache/planetarysats.txt";
    char *cpath = id.toLocal8Bit().data();
    planSat->setSatFile( cpath );
    qDebug() << "Using planetary sat data from" << id;

    if( planSat->getStateVector(1) == 0 )
    {
            qDebug() << "No planetary satellite data available";
            return;
    }

    planSat->stateToKepler();

    // FIXME this should be fixed in mex
    char cs_name[81];
    planSat->getSatName( (char*)&cs_name );
    cs_name[80] = '\0'; // sanity
    QString s_name = QString::fromAscii( (char*)&cs_name );
    qDebug() << "Found planetary satellite" << s_name;

    OrbiterSatellitesItem *item = new OrbiterSatellitesItem(
                    s_name, planSat, m_clock );

    beginUpdateItems();
    addItem( item );
    endUpdateItems();
}

} // namespace Marble

#include "OrbiterSatellitesModel.moc"

