// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Thibaut Gridel <tgridel@free.fr>
//

#ifndef MARBLE_KMLMULTITRACKTAGWRITER_H
#define MARBLE_KMLMULTITRACKTAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class KmlMultiTrackTagWriter : public GeoTagWriter
{
public:
    bool write(const GeoNode *node, GeoWriter &writer) const override;
};

}

#endif
