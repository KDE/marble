// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Mayank Madan <maddiemadan@gmail.com>
//

#ifndef MARBLE_KMLSCREENOVERLAYWRITER_H
#define MARBLE_KMLSCREENOVERLAYWRITER_H

#include "GeoDataVec2.h"
#include "KmlOverlayTagWriter.h"

#include "MarbleGlobal.h"

namespace Marble
{

class KmlScreenOverlayWriter : public KmlOverlayTagWriter
{
public:
    KmlScreenOverlayWriter();
    bool writeMid(const GeoNode *node, GeoWriter &writer) const override;

private:
    static void writeVec2(const QString &element, const GeoDataVec2 &vec2, GeoWriter &writer);
    static QString unitToString(GeoDataVec2::Unit unit);
};

}

#endif
