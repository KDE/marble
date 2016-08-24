//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Shashan Singh <shashank.personal@gmail.com>
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "PanoramioModel.h"
#include "PanoramioItem.h"
#include "PanoramioParser.h"

// Marble
#include "GeoDataLatLonAltBox.h"
#include "MarbleModel.h"

// Qt
#include <QUrl>
#include <QString>

using namespace Marble;

PanoramioModel::PanoramioModel( const MarbleModel *marbleModel, QObject *parent ) :
    AbstractDataPluginModel( "panoramio", marbleModel, parent ),
    m_marbleWidget( 0 )
{
}

void PanoramioModel::setMarbleWidget( MarbleWidget *widget )
{
    m_marbleWidget = widget;
}

void PanoramioModel::getAdditionalItems( const GeoDataLatLonAltBox &box, qint32 number )
{
    if (marbleModel()->planetId() != QLatin1String("earth")) {
        return;
    }

    // FIXME: Download a list of constant number, because the parser doesn't support
    // loading a file of an unknown length.
    const QUrl jsonUrl(QLatin1String("http://www.panoramio.com/map/get_panoramas.php?from=")
                       + QString::number(0)
                       + QLatin1String("&order=upload_date")
                       + QLatin1String("&set=public")
                       + QLatin1String("&to=")   + QString::number(number)
//                        + QLatin1String("&to=") + QString::number( number )
                       + QLatin1String("&minx=") + QString::number(box.west() * RAD2DEG)
                       + QLatin1String("&miny=") + QString::number(box.south() * RAD2DEG)
                       + QLatin1String("&maxx=") + QString::number(box.east() * RAD2DEG)
                       + QLatin1String("&maxy=") + QString::number(box.north() * RAD2DEG)
                       + QLatin1String("&size=small"));

    downloadDescriptionFile( jsonUrl );
}

void PanoramioModel::parseFile( const QByteArray &file )
{
    PanoramioParser panoramioJsonParser;
    QList<panoramioDataStructure> list
        = panoramioJsonParser.parseAllObjects( file,
                                               numberOfImagesPerFetch );

    QList<panoramioDataStructure>::iterator it;
    for ( it = list.begin(); it != list.end(); ++it ) {
        // Setting the meta information of the current image
        GeoDataCoordinates coordinates( (*it).longitude,
                                        (*it).latitude,
                                        0,
                                        GeoDataCoordinates::Degree );
                                        
        if( itemExists( QString::number( (*it).photo_id ) ) ) {
            continue;
        }
        
        PanoramioItem *item = new PanoramioItem( m_marbleWidget, this );
        item->setCoordinate( coordinates );
        item->setId( QString::number( (*it).photo_id ) );
        item->setPhotoUrl( (*it).photo_url );
        item->setUploadDate( (*it).upload_date );

        downloadItem( QUrl( (*it).photo_file_url ),
                            standardImageSize,
                            item );

        addItemToList( item );
    }
}

#include "moc_PanoramioModel.cpp"
