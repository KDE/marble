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
#include "GeoDataFolder.h"
#include "GeoDataGroundOverlay.h"
#include "GeoDataScreenOverlay.h"
#include "GeoDataPhotoOverlay.h"

namespace Marble
{
class PlaybackAnimatedUpdateItem : public PlaybackItem
{
    Q_OBJECT
public:
    PlaybackAnimatedUpdateItem( GeoDataAnimatedUpdate *animatedUpdate );
    const GeoDataAnimatedUpdate* animatedUpdate() const;
    double duration() const;
    void play();
    void pause();
    void seek( double position );
    void stop();

private:
    bool canDelete( const char* nodeType ) const;

    GeoDataDocument* rootDocument( GeoDataObject *object ) const;
    GeoDataFeature* findFeature( GeoDataFeature* feature, const QString& id ) const;
    GeoDataAnimatedUpdate* m_animatedUpdate;
    QList<GeoDataFeature*> m_createdObjects;
    QList<GeoDataFeature*> m_deletedObjects;
    GeoDataDocument* m_rootDocument;
};
}
#endif
