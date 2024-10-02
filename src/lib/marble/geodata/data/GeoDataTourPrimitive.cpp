// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2017 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "GeoDataTourPrimitive.h"

#include "GeoDataAnimatedUpdate.h"
#include "GeoDataFlyTo.h"
#include "GeoDataSoundCue.h"
#include "GeoDataTourControl.h"
#include "GeoDataTypes.h"
#include "GeoDataWait.h"

namespace Marble
{

bool GeoDataTourPrimitive::operator==(const GeoDataTourPrimitive &other) const
{
    if (nodeType() != other.nodeType()) {
        return false;
    }

    if (nodeType() == GeoDataTypes::GeoDataAnimatedUpdateType) {
        const auto &thisUpdate = static_cast<const GeoDataAnimatedUpdate &>(*this);
        const auto &otherUpdate = static_cast<const GeoDataAnimatedUpdate &>(other);

        return thisUpdate == otherUpdate;
    } else if (nodeType() == GeoDataTypes::GeoDataFlyToType) {
        const auto &thisFlyTo = static_cast<const GeoDataFlyTo &>(*this);
        const auto &otherFlyTo = static_cast<const GeoDataFlyTo &>(other);

        return thisFlyTo == otherFlyTo;
    } else if (nodeType() == GeoDataTypes::GeoDataSoundCueType) {
        const auto &thisCue = static_cast<const GeoDataSoundCue &>(*this);
        const auto &otherCue = static_cast<const GeoDataSoundCue &>(other);

        return thisCue == otherCue;
    } else if (nodeType() == GeoDataTypes::GeoDataTourControlType) {
        const auto &thisControl = static_cast<const GeoDataTourControl &>(*this);
        const auto &otherControl = static_cast<const GeoDataTourControl &>(other);

        return thisControl == otherControl;
    } else if (nodeType() == GeoDataTypes::GeoDataWaitType) {
        const auto &thisWait = static_cast<const GeoDataWait &>(*this);
        const auto &otherWait = static_cast<const GeoDataWait &>(other);

        return thisWait == otherWait;
    }

    return false;
}

} // namespace Marble
