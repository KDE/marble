// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
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
    explicit PlaybackTourControlItem(const GeoDataTourControl *tourControl);
    const GeoDataTourControl *tourControl() const;
    double duration() const override;
    void play() override;
    void pause() override;
    void seek(double position) override;
    void stop() override;

private:
    const GeoDataTourControl *m_tourControl;
};

}
#endif
