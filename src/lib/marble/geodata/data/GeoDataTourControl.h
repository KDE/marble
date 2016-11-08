//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#ifndef GEODATATOURCONTROL_H
#define GEODATATOURCONTROL_H

#include "GeoDataTourPrimitive.h"

namespace Marble
{

/**
 */
class GEODATA_EXPORT GeoDataTourControl : public GeoDataTourPrimitive
{
public:
    enum PlayMode {
        Play,
        Pause
    };

    GeoDataTourControl();
    ~GeoDataTourControl();

    bool operator==( const GeoDataTourControl &other ) const;
    bool operator!=( const GeoDataTourControl &other ) const;
    const char *nodeType() const;

    PlayMode playMode() const;
    void setPlayMode(PlayMode mode);

private:
    PlayMode m_playMode;
};

} // namespace Marble

#endif // GEODATATOURCONTROL_H
