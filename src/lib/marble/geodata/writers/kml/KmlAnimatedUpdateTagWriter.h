// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#ifndef MARBLE_KMLANIMATEDUPDATETAGWRITER_H
#define MARBLE_KMLANIMATEDUPDATETAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class KmlAnimatedUpdateTagWriter : public GeoTagWriter
{
public:
    bool write(const GeoNode *node, GeoWriter &writer) const override;
};

}
#endif
