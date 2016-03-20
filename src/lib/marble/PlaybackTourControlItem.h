//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#ifndef PLAYBACKTOURCONTROLITEM_H
#define PLAYBACKTOURCONTROLITEM_H

#include "PlaybackItem.h"

namespace Marble
{

class GeoDataTourControl;

class PlaybackTourControlItem : public PlaybackItem
{
public:
    explicit PlaybackTourControlItem( const GeoDataTourControl* tourControl );
    const GeoDataTourControl* tourControl() const;
    double duration() const;
    void play();
    void pause();
    void seek( double position );
    void stop();
private:
    const GeoDataTourControl* m_tourControl;
};

}
#endif
