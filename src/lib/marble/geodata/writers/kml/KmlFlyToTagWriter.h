// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>
//

#ifndef MARBLE_KMLFLYTOTAGWRITER_H
#define MARBLE_KMLFLYTOTAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class KmlFlyToTagWriter : public GeoTagWriter
{
public:
    bool write(const GeoNode *node, GeoWriter &writer) const override;
};

}
#endif
