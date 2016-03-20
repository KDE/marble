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

namespace Marble
{

class GeoDataAnimatedUpdate;
class GeoDataDocument;
class GeoDataFeature;
class GeoDataObject;

class PlaybackAnimatedUpdateItem : public PlaybackItem
{
    Q_OBJECT
public:
    explicit PlaybackAnimatedUpdateItem( GeoDataAnimatedUpdate *animatedUpdate );
    const GeoDataAnimatedUpdate* animatedUpdate() const;
    double duration() const;
    void play();
    void pause();
    void seek( double );
    void stop();
    bool isApplied() const;

private:
    bool canDelete( const char* nodeType ) const;
    GeoDataDocument* rootDocument( GeoDataObject *object ) const;
    GeoDataFeature* findFeature( GeoDataFeature* feature, const QString& id ) const;
    GeoDataAnimatedUpdate* m_animatedUpdate;
    QList<GeoDataFeature*> m_deletedObjects;
    GeoDataDocument* m_rootDocument;
    bool m_playing;
};

}
#endif
