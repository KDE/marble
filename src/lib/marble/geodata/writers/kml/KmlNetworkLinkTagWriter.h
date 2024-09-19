// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Adrian Draghici <draghici.adrian.b@gmail.com>
//

#ifndef MARBLE_KMLNETWORKLINKTAGWRITER_H
#define MARBLE_KMLNETWORKLINKTAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class KmlNetworkLinkTagWriter : public GeoTagWriter
{
public:
    bool write(const GeoNode *node, GeoWriter &writer) const override;
};

}

#endif
