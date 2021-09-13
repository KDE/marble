// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Mayank Madan <maddiemadan@gmail.com>
//

#ifndef MARBLE_KMLBALLOONSTYLETAGWRITER_H
#define MARBLE_KMLBALLOONSTYLETAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class KmlBalloonStyleTagWriter : public GeoTagWriter
{
public:
    bool write( const GeoNode *node, GeoWriter& writer ) const override;
};

}

#endif
