// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
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
    ~GeoDataTourControl() override;

    bool operator==(const GeoDataTourControl &other) const;
    bool operator!=(const GeoDataTourControl &other) const;
    const char *nodeType() const override;

    PlayMode playMode() const;
    void setPlayMode(PlayMode mode);

private:
    PlayMode m_playMode;
};

} // namespace Marble

#endif // GEODATATOURCONTROL_H
