//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#include "GeoTrackGraphicsItem.h"

#include "GeoDataLineString.h"
#include "GeoDataTrack.h"
#include "GeoDataFeature.h"
#include "MarbleDebug.h"
#include "StyleBuilder.h"

using namespace Marble;

GeoTrackGraphicsItem::GeoTrackGraphicsItem( const GeoDataFeature *feature, const GeoDataTrack *track )
    : GeoLineStringGraphicsItem( feature, track->lineString() )
{
    setTrack( track );
    if (feature) {
        QString const paintLayer = QLatin1String("Track/") + StyleBuilder::visualCategoryName(feature->visualCategory());
        setPaintLayers(QStringList() << paintLayer);
    }
}

void GeoTrackGraphicsItem::setTrack( const GeoDataTrack* track )
{
    m_track = track;
    update();
}

void GeoTrackGraphicsItem::paint(GeoPainter *painter, const ViewportParams *viewport , const QString &layer)
{
    Q_UNUSED(layer);
    update();
    GeoLineStringGraphicsItem::paint(painter, viewport, layer);
}

void GeoTrackGraphicsItem::update()
{
    setLineString( m_track->lineString() );
}
