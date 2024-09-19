// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Mayank Madan <maddiemadan@gmail.com>
//

#ifndef MARBLE_KMLLISTSTYLETAGWRITER_H
#define MARBLE_KMLLISTSTYLETAGWRITER_H

#include "GeoDataItemIcon.h"
#include "GeoDataListStyle.h"
#include "GeoTagWriter.h"

namespace Marble
{

class KmlListStyleTagWriter : public GeoTagWriter
{
public:
    bool write(const GeoNode *node, GeoWriter &writer) const override;

private:
    static QString itemTypeToString(GeoDataListStyle::ListItemType itemType);
    static QString iconStateToString(GeoDataItemIcon::ItemIconStates state);
};

}

#endif
