// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_KMLSTYLEMAPTAGWRITER_H
#define MARBLE_KMLSTYLEMAPTAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class KmlStyleMapTagWriter : public GeoTagWriter
{
public:
    bool write(const GeoNode *node, GeoWriter &writer) const override;
};

}

#endif
