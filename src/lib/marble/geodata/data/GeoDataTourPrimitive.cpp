// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2017 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "GeoDataTourPrimitive.h"

#include "GeoDataAnimatedUpdate.h"
#include "GeoDataFlyTo.h"
#include "GeoDataSoundCue.h"
#include "GeoDataTourControl.h"
#include "GeoDataWait.h"
#include "GeoDataTypes.h"

namespace Marble {

bool GeoDataTourPrimitive::operator==(const GeoDataTourPrimitive &other) const
{
    if (nodeType() != other.nodeType()) {
        return false;
    }

    if (nodeType() ==  GeoDataTypes::GeoDataAnimatedUpdateType) {
        const GeoDataAnimatedUpdate &thisUpdate = static_cast<const GeoDataAnimatedUpdate &>(*this);
        const GeoDataAnimatedUpdate &otherUpdate = static_cast<const GeoDataAnimatedUpdate &>(other);

        return thisUpdate == otherUpdate;
    }
    else if (nodeType() == GeoDataTypes::GeoDataFlyToType) {
        const GeoDataFlyTo &thisFlyTo = static_cast<const GeoDataFlyTo &>(*this);
        const GeoDataFlyTo &otherFlyTo = static_cast<const GeoDataFlyTo &>(other);

        return thisFlyTo == otherFlyTo;
    }
    else if (nodeType() == GeoDataTypes::GeoDataSoundCueType) {
        const GeoDataSoundCue &thisCue = static_cast<const GeoDataSoundCue &>(*this);
        const GeoDataSoundCue &otherCue = static_cast<const GeoDataSoundCue &>(other);

        return thisCue == otherCue;
    }
    else if (nodeType() == GeoDataTypes::GeoDataTourControlType) {
        const GeoDataTourControl &thisControl = static_cast<const GeoDataTourControl &>(*this);
        const GeoDataTourControl &otherControl = static_cast<const GeoDataTourControl &>(other);

        return thisControl == otherControl;
    }
    else if (nodeType() == GeoDataTypes::GeoDataWaitType) {
        const GeoDataWait &thisWait = static_cast<const GeoDataWait &>(*this);
        const GeoDataWait &otherWait = static_cast<const GeoDataWait &>(other);

        return thisWait == otherWait;
    }

    return false;
}

} // namespace Marble
