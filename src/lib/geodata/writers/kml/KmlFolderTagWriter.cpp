//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Gaurav Gupta <1989.gaurav@googlemail.com>
//

#include "KmlFolderTagWriter.h"

#include "GeoDataFolder.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "MarbleDebug.h"
#include "GeoDataExtendedData.h"

#include "GeoDataTypes.h"

#include <QtCore/QVector>

namespace Marble
{

static GeoTagWriterRegistrar s_writerDocument( GeoTagWriter::QualifiedName(GeoDataTypes::GeoDataFolderType,
                                                                            kml::kmlTag_nameSpace22),
                                               new KmlFolderTagWriter() );

bool KmlFolderTagWriter::write( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataFolder *folder = static_cast<const GeoDataFolder*>(node);

    writer.writeStartElement( kml::kmlTag_Folder );

    //Writing folder name
    writer.writeOptionalElement( "name", folder->name() );

    if( !folder->extendedData().isEmpty() ){
		writeElement( &folder->extendedData(), writer );
    }

    //Write all containing features
    QVector<GeoDataFeature*>::ConstIterator it =  folder->constBegin();
    QVector<GeoDataFeature*>::ConstIterator const end = folder->constEnd();

    for ( ; it != end; ++it ) {
        writeElement( *it, writer );
    }

    //close folder tag
    writer.writeEndElement();
    return true;
}

}
