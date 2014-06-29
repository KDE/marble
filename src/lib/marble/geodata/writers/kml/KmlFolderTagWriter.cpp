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

#include "GeoDataTypes.h"

#include <QVector>

namespace Marble
{

static GeoTagWriterRegistrar s_writerDocument( GeoTagWriter::QualifiedName(GeoDataTypes::GeoDataFolderType,
                                                                            kml::kmlTag_nameSpaceOgc22),
                                               new KmlFolderTagWriter() );

bool KmlFolderTagWriter::writeMid( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataFolder *folder = static_cast<const GeoDataFolder*>(node);

    //Write all containing features
    QVector<GeoDataFeature*>::ConstIterator it =  folder->constBegin();
    QVector<GeoDataFeature*>::ConstIterator const end = folder->constEnd();

    for ( ; it != end; ++it ) {
        writeElement( *it, writer );
    }

    return true;
}

KmlFolderTagWriter::KmlFolderTagWriter() :
  KmlFeatureTagWriter( kml::kmlTag_Folder )
{
  // nothing to do
}

}
