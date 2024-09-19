
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Shou Ya <shouyalovelyx@gmail.com>
//

#ifndef MARBLE_KMLLATLONBOXWRITER_H
#define MARBLE_KMLLATLONBOXWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class KmlLatLonBoxWriter : public GeoTagWriter
{
public:
    bool write(const GeoNode *node, GeoWriter &writer) const override;
};

}

#endif
