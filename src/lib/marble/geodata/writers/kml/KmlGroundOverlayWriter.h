// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Shou Ya <shouyalovelyx@gmail.com>
//

#ifndef MARBLE_KMLGROUNDOVERLAYWRITER_H
#define MARBLE_KMLGROUNDOVERLAYWRITER_H

#include "GeoTagWriter.h"
#include "KmlOverlayTagWriter.h"

#include "MarbleGlobal.h"

namespace Marble
{

class KmlGroundOverlayWriter : public KmlOverlayTagWriter
{
public:
    KmlGroundOverlayWriter();

    bool writeMid(const GeoNode *node, GeoWriter &writer) const override;

    static QString altitudeModeToString(AltitudeMode mode);

    static void writeAltitudeMode(GeoWriter &writer, AltitudeMode altitudeMode);
};

}

#endif
