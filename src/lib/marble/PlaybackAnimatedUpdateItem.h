//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#ifndef PLAYBACKANIMATEDUPDATEITEM_H
#define PLAYBACKANIMATEDUPDATEITEM_H

#include "PlaybackItem.h"
#include "GeoDataAnimatedUpdate.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"

namespace Marble
{
class PlaybackAnimatedUpdateItem : public PlaybackItem
{
    Q_OBJECT
public:
    PlaybackAnimatedUpdateItem( const GeoDataAnimatedUpdate* animatedUpdate );
    const GeoDataAnimatedUpdate* animatedUpdate() const;
    double duration() const;
    void play();
    void pause();
    void seek( double position );
    void stop();

private:
    GeoDataDocument* rootDocument( GeoDataObject* ) const;
    GeoDataPlacemark* findPlacemark( GeoDataFeature*, const QString& ) const;
    const GeoDataAnimatedUpdate* m_animatedUpdate;
};
}
#endif