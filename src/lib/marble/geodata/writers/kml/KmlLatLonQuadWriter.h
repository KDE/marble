// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>
//

#ifndef KMLLATLONQUADWRITER_H
#define KMLLATLONQUADWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class KmlLatLonQuadWriter : public GeoTagWriter
{
public:
    bool write(const GeoNode *node, GeoWriter &writer) const override;
};

}

#endif // KMLLATLONQUADWRITER_H
