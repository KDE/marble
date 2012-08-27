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
#include "GeoDataPlacemark.h"

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

        update();
    }
}

void OrbiterSatellitesModel::parseFile( const QString &id,
                                        const QByteArray &file )
{
    mDebug() << "Reading orbiter catalog from:" << id;
    QTextStream ts(file);

    clear();

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
        QString type( elms[1] );
        QString body( elms[2] );
        QByteArray body8Bit = body.toLocal8Bit();
        char *cbody = const_cast<char*>( body8Bit.constData() );

        mDebug() << "Loading" << type << name;

        PlanetarySats *planSat = new PlanetarySats();
        planSat->setPlanet( cbody );

        planSat->setStateVector(
            elms[6].toFloat() - 2400000.5,
            elms[7].toFloat(),  elms[8].toFloat(),  elms[9].toFloat(),
            elms[10].toFloat(), elms[11].toFloat(), elms[12].toFloat() );

        planSat->stateToKepler();

        OrbiterSatellitesItem *item;
        item = new OrbiterSatellitesItem( name, type, body, planSat, m_clock );
        item->placemark()->setVisible( ( body.toLower() == m_lcPlanet ) );

        addItem( item );
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

void OrbiterSatellitesModel::update()
{
    beginUpdateItems();

    foreach( QObject *obj, items() ) {
        OrbiterSatellitesItem *item = qobject_cast<OrbiterSatellitesItem*>(obj);
        if( item != NULL ) {
            bool visible = ( item->body().toLower() == m_lcPlanet );
            item->placemark()->setVisible( visible );

            if( visible ) {
                item->update();
            }
        }
    }

    endUpdateItems();
}

} // namespace Marble

#include "OrbiterSatellitesModel.moc"

