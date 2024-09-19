// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#include "InstructionTransformation.h"

#include <cmath>

namespace Marble
{

RoutingInstructions InstructionTransformation::process(const RoutingWaypoints &model)
{
    RoutingInstructions result;

    int lastAngle = 0;
    for (int i = 0; i < model.size(); ++i) {
        const RoutingWaypoint &item = model[i];
        int newAngle = 180 + lastAngle;
        if (i < model.size() - 1) {
            newAngle = qRound(180.0 / M_PI * item.point().bearing(model[i + 1].point()));
        }
        int angle = (newAngle - lastAngle + 540) % 360;
        Q_ASSERT(angle >= 0 && angle <= 360);
        if (result.isEmpty() || !result.last().append(item, angle)) {
            result.push_back(RoutingInstruction(item));
        }
        lastAngle = newAngle;
    }

    for (int i = 0; i < result.size(); ++i) {
        result[i].setSuccessor(i < result.size() - 1 ? &result[i + 1] : nullptr);
        result[i].setPredecessor(i ? &result[i - 1] : nullptr);
    }

    return result;
}

} // namespace Marble
