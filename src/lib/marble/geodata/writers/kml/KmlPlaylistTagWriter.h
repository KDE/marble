// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Adrian Draghici <draghici.adrian.b@gmail.com>
//

#ifndef MARBLE_KMLPLAYLISTTAGWRITER_H
#define MARBLE_KMLPLAYLISTTAGWRITER_H

#include "GeoDataTourControl.h"
#include "GeoTagWriter.h"

namespace Marble
{

class GeoDataSoundCue;
class GeoDataWait;

class KmlPlaylistTagWriter : public GeoTagWriter
{
public:
    bool write(const GeoNode *node, GeoWriter &writer) const override;

private:
    static void writeTourPrimitive(const GeoDataTourPrimitive *primitive, GeoWriter &writer);
    static void writeTourControl(const GeoDataTourControl &tourControl, GeoWriter &writer);
    static void writeWait(const GeoDataWait &wait, GeoWriter &writer);
    static void writeSoundCue(const GeoDataSoundCue &cue, GeoWriter &writer);
    static QString playModeToString(GeoDataTourControl::PlayMode playMode);
};

}

#endif
