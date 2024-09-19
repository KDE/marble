// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_KMLLINEARRINGTAGWRITER_H
#define MARBLE_KMLLINEARRINGTAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class KmlLinearRingTagWriter : public GeoTagWriter
{
public:
    bool write(const GeoNode *node, GeoWriter &writer) const override;
};

}

#endif
