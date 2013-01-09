//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Anders Lund <anders@alweb.dk>
//

#include "OpenCachingComModel.h"
#include "OpenCachingComItem.h"

#include "MarbleGlobal.h"
#include "MarbleModel.h"
#include "GeoDataCoordinates.h"
#include "MarbleDebug.h"

#include <QtCore/QDebug>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>

namespace Marble {

OpenCachingComModel::OpenCachingComModel( const MarbleModel *marbleModel, QObject *parent )
    : AbstractDataPluginModel( "opencachingcom", parent )
    , m_marbleModel( marbleModel )
{
    updateHome();
    connect(m_marbleModel, SIGNAL(homeChanged(GeoDataCoordinates)), SLOT(updateHome()));

    // translate known values for tags and cache types
    // What would be a nice place for this??? Put here, since this object is only
    // constructed once.
    // note to translators: "cache" in this context means "geocache",
    // please refer to opencaching.com and wikipedia if in doubt.
    tr("Boat required");
    tr("Chirp");
    tr("Diving");
    tr("Exclusive");
    tr("Historic site");
    tr("Letterbox");
    tr("Night");
    tr("Tree climbing");
    tr("UV light");

    tr("Traditional Cache");
    tr("Multi-cache");
    tr("Virtual Cache");
    tr("Puzzle Cache");
    tr("Unknown Cache");
}

OpenCachingComModel::~OpenCachingComModel()
{
}

void OpenCachingComModel::getAdditionalItems( const GeoDataLatLonAltBox& box, const MarbleModel *model, qint32 number )
{
    if( model->planetId() != "earth" )
    {
        return;
    }

    if ( m_previousbox.contains( box ) )
    {
        return;
    }

    QString url("http://www.opencaching.com/api/geocache/?Authorization=");
    url += AUTHKEY + QString("&bbox=%1,%2,%3,%4")
        .arg( box.south( GeoDataCoordinates::Degree ) )
        .arg( box.west(GeoDataCoordinates::Degree ) )
        .arg( box.north(GeoDataCoordinates::Degree ) )
        .arg( box.east(GeoDataCoordinates::Degree ) );
    if(!m_previousbox.isNull())
    {
        url += QString("&exclude_bbox=%1,%2,%3,%4")
            .arg( m_previousbox.south( GeoDataCoordinates::Degree ) )
            .arg( m_previousbox.west(GeoDataCoordinates::Degree ) )
            .arg( m_previousbox.north(GeoDataCoordinates::Degree ) )
            .arg( m_previousbox.east(GeoDataCoordinates::Degree ) );
    }
    url += "&limit=" + QString::number( number );
    // TODO Limit to user set tags/types/difficulty - when there is a config dialog...

    m_previousbox = box;

//     qDebug()<<"Fetching more caches: "<<url;
    downloadDescriptionFile( QUrl( url ) );
}

void OpenCachingComModel::parseFile( const QByteArray& file )
{
    QScriptEngine engine;

    // Qt requires parentheses around json code
    QScriptValue data = engine.evaluate( '(' + QString::fromUtf8( file ) + ')' );
    QVariantList caches = data.toVariant().toList();

//     qDebug()<<"parsing "<<caches.size()<<" items";
    QList<AbstractDataPluginItem*> items;
    while (!caches.isEmpty())
    {
        QVariantMap map = caches.takeFirst().toMap();
        if ( !findItem( map["oxcode"].toString() ) )
        {
            items << new OpenCachingComItem( map, this );
        }
    }
    addItemsToList(items);
}

void OpenCachingComModel::fetchData(const QString& url, const QString &type, OpenCachingComItem *item)
{
    downloadItem(url, type, item);
}

const MarbleModel* OpenCachingComModel::marbleModel()
{
    return m_marbleModel;
}

const GeoDataCoordinates OpenCachingComModel::home() const
{
    return m_homeCoordinates;
}

void OpenCachingComModel::updateHome()
{
    qreal lon, lat;
    int zoom;
    m_marbleModel->home( lon, lat, zoom );
    m_homeCoordinates = GeoDataCoordinates(lon, lat, 0, GeoDataCoordinates::Degree);
}

} // namespace Marble

#include "OpenCachingComModel.moc"
