//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#include "DgmlTextureTagWriter.h"

#include "GeoSceneTypes.h"
#include "GeoWriter.h"
#include "GeoSceneTexture.h"
#include "DownloadPolicy.h"
#include "DgmlElementDictionary.h"
#include <TileId.h>

namespace Marble
{

static GeoTagWriterRegistrar s_writerData( GeoTagWriter::QualifiedName( GeoSceneTypes::GeoSceneTextureType,
                                                                            dgml::dgmlTag_nameSpace20 ),
                                               new DgmlTextureTagWriter() );


bool DgmlTextureTagWriter::write(const GeoNode *node, GeoWriter& writer) const
{
    const GeoSceneTexture *texture = static_cast<const GeoSceneTexture*>( node );
    writer.writeStartElement( dgml::dgmlTag_Texture );
    
    writer.writeStartElement( dgml::dgmlTag_SourceDir );
    writer.writeAttribute( "format", texture->fileFormat() );
    if( texture->expire() ) {
        writer.writeAttribute( "expire", QString::number( texture->expire() ) );
    }
    writer.writeOptionalElement( "installmap", texture->installMap() );
    writer.writeEndElement();
    
    if( texture->hasMaximumTileLevel() )
    {
        writer.writeStartElement( dgml::dgmlTag_StorageLayout );
        writer.writeAttribute( "maximumTileLevel", texture->fileFormat() );
        writer.writeEndElement();
    }
    
    if ( texture->downloadUrls().size() > 0 )
    {
        for( int i = 0; i < texture->downloadUrls().size(); ++i )
        {
            QString protocol = texture->downloadUrls().at(i).toString().left( texture->downloadUrls().at(i).toString().indexOf( ':' ) );
            QString host =  QString( texture->downloadUrls().at(i).encodedHost() );
            QString path =  QString( texture->downloadUrls().at(i).encodedPath() );
            QString query = QString( texture->downloadUrls().at(i).encodedQuery() );

            writer.writeStartElement( dgml::dgmlTag_DownloadUrl );
            writer.writeAttribute( "protocol", protocol );
            writer.writeAttribute( "host", host );
            writer.writeAttribute( "path", path );
            writer.writeAttribute( "query", query );
            writer.writeEndElement();
        }
    }
    
    foreach( DownloadPolicy const *policy, texture->downloadPolicies() )
    {
        writer.writeStartElement( dgml::dgmlTag_DownloadPolicy );
        
        if( policy->key().usage() == DownloadBrowse )
        {
            writer.writeAttribute( "Browse", QString::number( policy->maximumConnections() ) );
        }
        
        else if( policy->key().usage()  == DownloadBulk )
        {
            writer.writeAttribute( "Bulk", QString::number( policy->maximumConnections() ) );
        }
        
        writer.writeEndElement();    
    }
    
    writer.writeStartElement( dgml::dgmlTag_Projection );
    if( texture->projection() == GeoSceneTexture::Mercator ) {
        writer.writeAttribute( "name", "Mercator" );
    } else if ( texture->projection() == GeoSceneTexture::Equirectangular ) {
        writer.writeAttribute( "name", "Equirectangular" );        
    }
    writer.writeEndElement();
    
    writer.writeEndElement();
    return true;
}

};