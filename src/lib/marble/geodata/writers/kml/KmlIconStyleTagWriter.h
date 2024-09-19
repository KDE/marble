// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_KMLICONSTYLETAGWRITER_H
#define MARBLE_KMLICONSTYLETAGWRITER_H

#include "GeoDataHotSpot.h"
#include "GeoTagWriter.h"
#include "KmlColorStyleTagWriter.h"

namespace Marble
{

class KmlIconStyleTagWriter : public KmlColorStyleTagWriter
{
public:
    KmlIconStyleTagWriter();

protected:
    bool writeMid(const GeoNode *node, GeoWriter &writer) const override;
    bool isEmpty(const GeoNode *node) const override;

private:
    static QString unitString(GeoDataHotSpot::Units unit);
};

}

#endif
