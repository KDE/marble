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
#include "GeoDataWait.h"
#include "GeoDataFlyTo.h"
#include "GeoDataSoundCue.h"

namespace Marble
{

class KmlPlaylistTagWriter : public GeoTagWriter
{
public:
    virtual bool write( const GeoNode *node, GeoWriter& writer ) const;

private:
    void writeTourPrimitive( const GeoNode *primitive, GeoWriter& writer ) const;
    void writeTourControl( const GeoDataTourControl* tourControl, GeoWriter& writer ) const;
    void writeWait( const GeoDataWait* wait, GeoWriter& writer ) const;
    void writeSoundCue( const GeoDataSoundCue *cue, GeoWriter &writer ) const;
    QString playModeToString( GeoDataTourControl::PlayMode playMode ) const;
};

}

#endif
