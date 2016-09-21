//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Marek Hakala <hakala.marek@gmail.com>
//

#include "KmlUpdateTagWriter.h"
#include "GeoDataUpdate.h"
#include "GeoDataCreate.h"
#include "GeoDataDelete.h"
#include "GeoDataChange.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "GeoDataAbstractView.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerUpdate( GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataUpdateType,
                                             kml::kmlTag_nameSpaceOgc22 ), new KmlUpdateTagWriter );

bool KmlUpdateTagWriter::write( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataUpdate *update = static_cast<const GeoDataUpdate*>( node );
    KmlObjectTagWriter::writeIdentifiers( writer, update );
    writer.writeStartElement( kml::kmlTag_Update );
    writer.writeElement( kml::kmlTag_targetHref, update->targetHref() );

    if( update->change() && update->change()->size() > 0 ){
        writer.writeStartElement( kml::kmlTag_Change );
        QVector<GeoDataFeature*>::ConstIterator it =  update->change()->constBegin();
        QVector<GeoDataFeature*>::ConstIterator const end = update->change()->constEnd();

        for ( ; it != end; ++it ){
            writeElement( *it, writer );
        }
        writer.writeEndElement();
    } else if( update->create() && update->create()->size() > 0 ){
        writer.writeStartElement( kml::kmlTag_Create );
        QVector<GeoDataFeature*>::ConstIterator it =  update->create()->constBegin();
        QVector<GeoDataFeature*>::ConstIterator const end = update->create()->constEnd();

        for ( ; it != end; ++it ){
            writeElement( *it, writer );
        }
        writer.writeEndElement();
    } else if( update->getDelete() && update->getDelete()->size() > 0 ){
        writer.writeStartElement( kml::kmlTag_Delete );
        QVector<GeoDataFeature*>::ConstIterator it =  update->getDelete()->constBegin();
        QVector<GeoDataFeature*>::ConstIterator const end = update->getDelete()->constEnd();

        for ( ; it != end; ++it ){
            writeElement( *it, writer );
        }
        writer.writeEndElement();
    }

    writer.writeEndElement();
    return true;
}

}
