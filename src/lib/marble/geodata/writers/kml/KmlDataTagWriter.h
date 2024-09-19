// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Gaurav Gupta <1989.gaurav@googlemail.com>
//

#ifndef MARBLE_KMLDATATAGWRITER_H
#define MARBLE_KMLDATATAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class KmlDataTagWriter : public GeoTagWriter
{
public:
    bool write(const GeoNode *node, GeoWriter &writer) const override;
};

}

#endif
