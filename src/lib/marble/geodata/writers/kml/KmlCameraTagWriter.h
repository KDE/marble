// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>
//

#ifndef MARBLE_KMLCAMERATAGWRITER_H
#define MARBLE_KMLCAMERATAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class KmlCameraTagWriter : public GeoTagWriter
{
public:
    bool write(const GeoNode *node, GeoWriter &writer) const override;
};

}

#endif
