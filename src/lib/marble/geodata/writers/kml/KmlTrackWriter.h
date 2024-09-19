// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Guillaume Martres <smarter@ubuntu.com>
//

#ifndef MARBLE_KMLTRACKWRITER_H
#define MARBLE_KMLTRACKWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class KmlTrackWriter : public GeoTagWriter
{
public:
    bool write(const GeoNode *node, GeoWriter &writer) const override;
};

}

#endif // MARBLE_KMLTRACKWRITER_H
