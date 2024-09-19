// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Andrew Manson <g.real.ate@gmail.com>
//

#ifndef MARBLE_KMLPOINTTAGWRITER_H
#define MARBLE_KMLPOINTTAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class KmlPointTagWriter : public GeoTagWriter
{
public:
    bool write(const GeoNode *node, GeoWriter &writer) const override;
};

}

#endif
