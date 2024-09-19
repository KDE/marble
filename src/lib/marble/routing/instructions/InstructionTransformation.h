// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_INSTRUCTIONTRANSFORMATION_H
#define MARBLE_INSTRUCTIONTRANSFORMATION_H

#include "RoutingInstruction.h"
#include "RoutingWaypoint.h"
#include "marble_export.h"

namespace Marble
{

/**
 * Transforms waypoints and metadata into driving directions
 */
class MARBLE_EXPORT InstructionTransformation
{
public:
    /** Transforms waypoints and metadata into driving directions */
    static RoutingInstructions process(const RoutingWaypoints &waypoints);

private:
    // Pure static usage
    InstructionTransformation() = delete;
    Q_DISABLE_COPY(InstructionTransformation)
};

} // namespace Marble

#endif // MARBLE_INSTRUCTIONTRANSFORMATION_H
