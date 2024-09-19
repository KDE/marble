// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Marek Hakala <hakala.marek@gmail.com>
//

#ifndef MARBLE_KMLNETWORKLINKCONTROLTAGWRITER_H
#define MARBLE_KMLNETWORKLINKCONTROLTAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class KmlNetworkLinkControlTagWriter : public GeoTagWriter
{
public:
    bool write(const GeoNode *node, GeoWriter &writer) const override;
};

}

#endif
