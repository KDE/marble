// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Mayank Madan <maddiemadan@gmail.com>
//

#ifndef MARBLE_KMLLODTAGWRITER_H
#define MARBLE_KMLLODTAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class KmlLodTagWriter : public GeoTagWriter
{
public:
    bool write(const GeoNode *node, GeoWriter &writer) const override;
};

}

#endif
