// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_KMLOBJECTTAGWRITER_H
#define MARBLE_KMLOBJECTTAGWRITER_H

#include <GeoDataObject.h>

namespace Marble
{
class GeoWriter;

class KmlObjectTagWriter
{
public:
    /**
     * Parses the id and targetId attributes and assign their values to the given object
     */
    static void writeIdentifiers(GeoWriter &writer, const GeoDataObject *object);

private:
    KmlObjectTagWriter() = delete; // not implemented
};

}

#endif
