/*
    Copyright (C) 2007 Murad Tagirov <tmurad@gmail.com>
    Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>

    This file is part of the KDE project

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "GeoDataDocument.h"
#include "GeoDataDocument_p.h"

#include "GeoDataFolder.h"
#include "GeoDataPlacemark.h"
#include "GeoDataStyle.h"
#include "GeoDataStyleMap.h"
#include "GeoDataNetworkLinkControl.h"
#include "GeoDataSchema.h"

#include "MarbleDebug.h"


namespace Marble
{

GeoDataDocument::GeoDataDocument()
    : GeoDataContainer( new GeoDataDocumentPrivate )
{
}

GeoDataDocument::GeoDataDocument( const GeoDataDocument& other )
    : GeoDocument(), GeoDataContainer( other )
{
}

GeoDataDocument::~GeoDataDocument()
{
}

GeoDataDocumentPrivate* GeoDataDocument::p()
{
    return static_cast<GeoDataDocumentPrivate*>(d);
}

const GeoDataDocumentPrivate* GeoDataDocument::p() const
{
    return static_cast<GeoDataDocumentPrivate*>(d);
}

bool GeoDataDocument::operator==( const GeoDataDocument &other ) const
{
    return GeoDataContainer::equals(other) &&
           p()->m_styleHash == other.p()->m_styleHash &&
           p()->m_styleMapHash == other.p()->m_styleMapHash &&
           p()->m_schemaHash == other.p()->m_schemaHash &&
           p()->m_filename == other.p()->m_filename &&
           p()->m_baseUri == other.p()->m_baseUri &&
           p()->m_networkLinkControl == other.p()->m_networkLinkControl &&
           p()->m_property == other.p()->m_property &&
           p()->m_documentRole == other.p()->m_documentRole;
}

bool GeoDataDocument::operator!=( const GeoDataDocument &other ) const
{
    return !this->operator==( other );
}

DocumentRole GeoDataDocument::documentRole() const
{
    return p()->m_documentRole;
}

void GeoDataDocument::setDocumentRole( DocumentRole role )
{
    p()->m_documentRole = role;
}

QString GeoDataDocument::property() const
{
    return p()->m_property;
}

void GeoDataDocument::setProperty( QString property )
{
    p()->m_property = property;
}

QString GeoDataDocument::fileName() const
{
    return p()->m_filename;
}

void GeoDataDocument::setFileName( const QString &value )
{
    detach();
    p()->m_filename = value;
}

QString GeoDataDocument::baseUri() const
{
    return p()->m_baseUri;
}

void GeoDataDocument::setBaseUri( const QString &baseUrl )
{
    detach();
    p()->m_baseUri = baseUrl;
}

GeoDataNetworkLinkControl GeoDataDocument::networkLinkControl() const
{
    return p()->m_networkLinkControl;
}

void GeoDataDocument::setNetworkLinkControl( const GeoDataNetworkLinkControl &networkLinkControl )
{
    detach();
    p()->m_networkLinkControl = networkLinkControl;
}

void GeoDataDocument::addStyle( const GeoDataStyle& style )
{
    detach();
    p()->m_styleHash.insert( style.styleId(), style );
    p()->m_styleHash[ style.styleId() ].setParent( this );
}

void GeoDataDocument::removeStyle( const QString& styleId )
{
    detach();
    p()->m_styleHash.remove( styleId );
}

GeoDataStyle& GeoDataDocument::style( const QString& styleId )
{
    /*
     * FIXME: m_styleHash always should contain at least default
     *        GeoDataStyle element
     */
    return p()->m_styleHash[ styleId ];
}

GeoDataStyle GeoDataDocument::style( const QString &styleId ) const
{
    return p()->m_styleHash.value( styleId );
}

QList<GeoDataStyle> GeoDataDocument::styles() const
{
    return p()->m_styleHash.values();
}

void GeoDataDocument::addStyleMap( const GeoDataStyleMap& map )
{
    detach();
    p()->m_styleMapHash.insert( map.styleId(), map );
    p()->m_styleMapHash[ map.styleId() ].setParent( this );
}

void GeoDataDocument::removeStyleMap( const QString& mapId )
{
    detach();
    p()->m_styleMapHash.remove( mapId );
}

GeoDataStyleMap& GeoDataDocument::styleMap( const QString& styleId )
{
    return p()->m_styleMapHash[ styleId ];
}

GeoDataStyleMap GeoDataDocument::styleMap( const QString &styleId ) const
{
    return p()->m_styleMapHash.value( styleId );
}

QList<GeoDataStyleMap> GeoDataDocument::styleMaps() const
{
    return p()->m_styleMapHash.values();
}

void GeoDataDocument::addSchema( const GeoDataSchema& schema )
{
    detach();
    p()->m_schemaHash.insert( schema.schemaId(), schema );
    p()->m_schemaHash[ schema.schemaId() ].setParent( this );
}

void GeoDataDocument::removeSchema( const QString& schemaId )
{
    detach();
    GeoDataSchema schema = p()->m_schemaHash.take( schemaId );
    schema.setParent( 0 );
}

GeoDataSchema GeoDataDocument::schema( const QString& schemaId ) const
{
    return p()->m_schemaHash.value( schemaId );
}

GeoDataSchema &GeoDataDocument::schema( const QString &schemaId )
{
    return p()->m_schemaHash[ schemaId ];
}

QList<GeoDataSchema> GeoDataDocument::schemas() const
{
    return p()->m_schemaHash.values();
}

void GeoDataDocument::pack( QDataStream& stream ) const
{
    GeoDataContainer::pack( stream );

    stream << p()->m_styleHash.size();
    
    
    for( QMap<QString, GeoDataStyle>::const_iterator iterator 
          = p()->m_styleHash.constBegin(); 
        iterator != p()->m_styleHash.constEnd(); 
        ++iterator ) {
        iterator.value().pack( stream );
    }
}


void GeoDataDocument::unpack( QDataStream& stream )
{
    detach();
    GeoDataContainer::unpack( stream );

    int size = 0;

    stream >> size;
    for( int i = 0; i < size; i++ ) {
        GeoDataStyle style;
        style.unpack( stream );
        p()->m_styleHash.insert( style.styleId(), style );
    }
}

}
