// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
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
    explicit PlaybackAnimatedUpdateItem(GeoDataAnimatedUpdate *animatedUpdate);
    const GeoDataAnimatedUpdate *animatedUpdate() const;
    double duration() const override;
    void play() override;
    void pause() override;
    void seek(double) override;
    void stop() override;
    bool isApplied() const;

private:
    static bool canDelete(const GeoDataFeature &feature);
    GeoDataDocument *rootDocument(GeoDataObject *object) const;
    GeoDataFeature *findFeature(GeoDataFeature *feature, const QString &id) const;
    GeoDataAnimatedUpdate *m_animatedUpdate;
    QList<GeoDataFeature *> m_deletedObjects;
    GeoDataDocument *m_rootDocument;
    bool m_playing;
};

}
#endif
