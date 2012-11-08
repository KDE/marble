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
#include "GeoSceneTiled.h"
#include "DownloadPolicy.h"
#include "DgmlElementDictionary.h"
#include "ServerLayout.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerTexture( GeoTagWriter::QualifiedName( GeoSceneTypes::GeoSceneTextureTileType, dgml::dgmlTag_nameSpace20 ),
                                               new DgmlTextureTagWriter() );

bool DgmlTextureTagWriter::write(const GeoNode *node, GeoWriter& writer) const
{
    const GeoSceneTiled *texture = static_cast<const GeoSceneTiled*>( node );
    writer.writeStartElement( dgml::dgmlTag_Texture );
    writer.writeAttribute( "name", texture->name() );
    writer.writeAttribute( "expire", QString::number( texture->expire() ) );
    
    writer.writeStartElement( dgml::dgmlTag_SourceDir );
    writer.writeAttribute( "format", texture->fileFormat() );
    if( texture->expire() ) {
        writer.writeAttribute( "expire", QString::number( texture->expire() ) );
    }
    writer.writeCharacters( texture->sourceDir() );
    writer.writeEndElement();
    writer.writeOptionalElement( dgml::dgmlTag_InstallMap, texture->installMap() );
    
    writer.writeStartElement( dgml::dgmlTag_StorageLayout );
    if( texture->hasMaximumTileLevel() )
    {
        writer.writeAttribute( "maximumTileLevel", QString::number( texture->maximumTileLevel() ) );
        writer.writeAttribute( "levelZeroColumns", QString::number( texture->levelZeroColumns() ) );
        writer.writeAttribute( "levelZeroRows", QString::number( texture->levelZeroRows() ) );
        writer.writeAttribute( "mode", texture->serverLayout()->name() );
    }
    writer.writeEndElement();
    
    if ( texture->downloadUrls().size() > 0 )
    {
        for( int i = 0; i < texture->downloadUrls().size(); ++i )
        {
            QString protocol = texture->downloadUrls().at(i).toString().left( texture->downloadUrls().at(i).toString().indexOf( ':' ) );
            QString host =  QString( texture->downloadUrls().at(i).host() );
            QString path =  QString( texture->downloadUrls().at(i).path() );
            QString query = QUrl::fromEncoded( texture->downloadUrls().at(i).encodedQuery() ).toString();

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
            writer.writeAttribute( "usage", "Browse" );
            writer.writeAttribute( "maximumConnections", QString::number( policy->maximumConnections() ) );
        }
        
        else if( policy->key().usage()  == DownloadBulk )
        {
            writer.writeAttribute( "usage", "Bulk" );
            writer.writeAttribute( "maximumConnections", QString::number( policy->maximumConnections() ) );
        }
        
        writer.writeEndElement();    
    }
    
    writer.writeStartElement( dgml::dgmlTag_Projection );
    if( texture->projection() == GeoSceneTiled::Mercator ) {
        writer.writeAttribute( "name", "Mercator" );
    } else if ( texture->projection() == GeoSceneTiled::Equirectangular ) {
        writer.writeAttribute( "name", "Equirectangular" );        
    }
    writer.writeEndElement();
    
    writer.writeEndElement();
    return true;
}

}
