//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Dennis Nienhüser <nienhueser@kde.org>
//

#include "KmlObjectTagWriter.h"

#include "GeoWriter.h"

namespace Marble {

void KmlObjectTagWriter::writeIdentifiers( GeoWriter &writer, const GeoDataObject *object )
{
    if ( object && !object->id().isEmpty() ) {
        writer.writeAttribute( "id", object->id().toUtf8() );
    }
    if ( object && !object->targetId().isEmpty() ) {
        writer.writeAttribute( "targetId", object->targetId().toUtf8() );
    }
}

}
