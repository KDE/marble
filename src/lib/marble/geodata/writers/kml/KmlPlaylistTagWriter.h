//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013       Adrian Draghici <draghici.adrian.b@gmail.com>
//

#ifndef MARBLE_KMLPLAYLISTTAGWRITER_H
#define MARBLE_KMLPLAYLISTTAGWRITER_H

#include "GeoTagWriter.h"
#include "GeoDataTourControl.h"

namespace Marble
{

class GeoDataSoundCue;
class GeoDataWait;

class KmlPlaylistTagWriter : public GeoTagWriter
{
public:
    bool write( const GeoNode *node, GeoWriter& writer ) const override;

private:
    static void writeTourPrimitive(const GeoDataTourPrimitive *primitive, GeoWriter &writer);
    static void writeTourControl(const GeoDataTourControl &tourControl, GeoWriter& writer );
    static void writeWait(const GeoDataWait &wait, GeoWriter &writer);
    static void writeSoundCue(const GeoDataSoundCue &cue, GeoWriter &writer);
    static QString playModeToString( GeoDataTourControl::PlayMode playMode );
};

}

#endif
