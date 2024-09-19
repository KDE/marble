// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Abhinav Gangwar <abhgang@gmail.com>
//

#ifndef MARBLE_KMLSIMPLEDATATAGWRITER_H
#define MARBLE_KMLSIMPLEDATATAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class KmlSimpleDataTagWriter : public GeoTagWriter
{
public:
    bool write(const GeoNode *node, GeoWriter &writer) const override;
};

}

#endif
