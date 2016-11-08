//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#ifndef GEODATASOUNDCUE_H
#define GEODATASOUNDCUE_H

#include "GeoDataTourPrimitive.h"

#include <QString>

namespace Marble
{

/**
 */
class GEODATA_EXPORT GeoDataSoundCue : public GeoDataTourPrimitive
{
public:
    GeoDataSoundCue();
    ~GeoDataSoundCue();

    bool operator==( const GeoDataSoundCue &other ) const;
    bool operator!=( const GeoDataSoundCue &other ) const;
    const char *nodeType() const;

    QString href() const;
    void setHref(const QString &url);

    double delayedStart() const;
    void setDelayedStart(double pause);

private:
    QString m_href;
    double m_delayedStart;
};

} // namespace Marble

#endif // GEODATASOUNDCUE_H
