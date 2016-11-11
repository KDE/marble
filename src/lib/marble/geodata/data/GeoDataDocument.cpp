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

#include "GeoDataStyle.h"
#include "GeoDataStyleMap.h"
#include "GeoDataNetworkLinkControl.h"
#include "GeoDataSchema.h"

#include "MarbleDebug.h"

#include <QDataStream>

namespace Marble
{

GeoDataDocument::GeoDataDocument()
    : GeoDataContainer( new GeoDataDocumentPrivate )
{
}

GeoDataDocument::GeoDataDocument( const GeoDataDocument& other )
    : GeoDocument(),
      GeoDataContainer(other, new GeoDataDocumentPrivate(*other.d_func()))
{
}

GeoDataDocument::~GeoDataDocument()
{
}

GeoDataDocument& GeoDataDocument::operator=(const GeoDataDocument& other)
{
    if (this != &other) {
        Q_D(GeoDataDocument);
        *d = *other.d_func();
    }

    return *this;
}

bool GeoDataDocument::operator==( const GeoDataDocument &other ) const
{
    if (!GeoDataContainer::equals(other)) {
        return false;
    }

    Q_D(const GeoDataDocument);
    const GeoDataDocumentPrivate* const other_d = other.d_func();
    if (!(d->m_styleHash.size() == other_d->m_styleHash.size() &&
          d->m_styleMapHash == other_d->m_styleMapHash &&
          d->m_schemaHash == other_d->m_schemaHash &&
          d->m_filename == other_d->m_filename &&
          d->m_baseUri == other_d->m_baseUri &&
          d->m_networkLinkControl == other_d->m_networkLinkControl &&
          d->m_property == other_d->m_property &&
          d->m_documentRole == other_d->m_documentRole)) {
        return false;
    }

    auto iter = d->m_styleHash.constBegin();
    auto const end = d->m_styleHash.constEnd();
    for (; iter != end; ++iter) {
        if (!other_d->m_styleHash.contains(iter.key())) {
            return false;
        }

        if (*iter.value() != *other_d->m_styleHash[iter.key()]) {
            return false;
        }
    }

    return true;
}

bool GeoDataDocument::operator!=( const GeoDataDocument &other ) const
{
    return !this->operator==( other );
}

const char* GeoDataDocument::nodeType() const
{
    return GeoDataTypes::GeoDataDocumentType;
}


GeoDataFeature * GeoDataDocument::clone() const
{
    return new GeoDataDocument(*this);
}

DocumentRole GeoDataDocument::documentRole() const
{
    Q_D(const GeoDataDocument);
    return d->m_documentRole;
}

void GeoDataDocument::setDocumentRole( DocumentRole role )
{
    Q_D(GeoDataDocument);
    d->m_documentRole = role;
}

QString GeoDataDocument::property() const
{
    Q_D(const GeoDataDocument);
    return d->m_property;
}

void GeoDataDocument::setProperty( const QString& property )
{
    Q_D(GeoDataDocument);
    d->m_property = property;
}

QString GeoDataDocument::fileName() const
{
    Q_D(const GeoDataDocument);
    return d->m_filename;
}

void GeoDataDocument::setFileName( const QString &value )
{
    Q_D(GeoDataDocument);
    d->m_filename = value;
}

QString GeoDataDocument::baseUri() const
{
    Q_D(const GeoDataDocument);
    return d->m_baseUri;
}

void GeoDataDocument::setBaseUri( const QString &baseUrl )
{
    Q_D(GeoDataDocument);
    d->m_baseUri = baseUrl;
}

GeoDataNetworkLinkControl GeoDataDocument::networkLinkControl() const
{
    Q_D(const GeoDataDocument);
    return d->m_networkLinkControl;
}

void GeoDataDocument::setNetworkLinkControl( const GeoDataNetworkLinkControl &networkLinkControl )
{
    Q_D(GeoDataDocument);
    d->m_networkLinkControl = networkLinkControl;
}

void GeoDataDocument::addStyle( const GeoDataStyle::Ptr &style )
{
    Q_D(GeoDataDocument);
    d->m_styleHash.insert(style->id(), style);
    d->m_styleHash[style->id()]->setParent(this);
}

void GeoDataDocument::removeStyle( const QString& styleId )
{
    Q_D(GeoDataDocument);
    d->m_styleHash.remove(styleId);
}

GeoDataStyle::Ptr GeoDataDocument::style( const QString& styleId )
{
    /*
     * FIXME: m_styleHash always should contain at least default
     *        GeoDataStyle element
     */
    Q_D(GeoDataDocument);
    return d->m_styleHash[styleId];
}

GeoDataStyle::ConstPtr GeoDataDocument::style( const QString &styleId ) const
{
    Q_D(const GeoDataDocument);
    return d->m_styleHash.value(styleId);
}

QList<GeoDataStyle::ConstPtr> GeoDataDocument::styles() const
{
    Q_D(const GeoDataDocument);
    QList<GeoDataStyle::ConstPtr> result;
    foreach(auto const & style, d->m_styleHash.values()) {
        result << style;
    }

    return result;
}

QList<GeoDataStyle::Ptr> GeoDataDocument::styles()
{
    Q_D(GeoDataDocument);
    return d->m_styleHash.values();
}

void GeoDataDocument::addStyleMap( const GeoDataStyleMap& map )
{
    Q_D(GeoDataDocument);
    d->m_styleMapHash.insert(map.id(), map);
    d->m_styleMapHash[map.id()].setParent(this);
}

void GeoDataDocument::removeStyleMap( const QString& mapId )
{
    Q_D(GeoDataDocument);
    d->m_styleMapHash.remove(mapId);
}

GeoDataStyleMap& GeoDataDocument::styleMap( const QString& styleId )
{
    Q_D(GeoDataDocument);
    return d->m_styleMapHash[styleId];
}

GeoDataStyleMap GeoDataDocument::styleMap( const QString &styleId ) const
{
    Q_D(const GeoDataDocument);
    return d->m_styleMapHash.value(styleId);
}

QList<GeoDataStyleMap> GeoDataDocument::styleMaps() const
{
    Q_D(const GeoDataDocument);
    return d->m_styleMapHash.values();
}

void GeoDataDocument::addSchema( const GeoDataSchema& schema )
{
    Q_D(GeoDataDocument);
    d->m_schemaHash.insert(schema.id(), schema);
    d->m_schemaHash[schema.id()].setParent(this);
}

void GeoDataDocument::removeSchema( const QString& schemaId )
{
    Q_D(GeoDataDocument);
    GeoDataSchema schema = d->m_schemaHash.take(schemaId);
    schema.setParent( 0 );
}

GeoDataSchema GeoDataDocument::schema( const QString& schemaId ) const
{
    Q_D(const GeoDataDocument);
    return d->m_schemaHash.value(schemaId);
}

GeoDataSchema &GeoDataDocument::schema( const QString &schemaId )
{
    Q_D(GeoDataDocument);
    return d->m_schemaHash[schemaId];
}

QList<GeoDataSchema> GeoDataDocument::schemas() const
{
    Q_D(const GeoDataDocument);
    return d->m_schemaHash.values();
}

void GeoDataDocument::pack( QDataStream& stream ) const
{
    Q_D(const GeoDataDocument);
    GeoDataContainer::pack( stream );

    stream << d->m_styleHash.size();

    for( QMap<QString, GeoDataStyle::Ptr>::const_iterator iterator
          = d->m_styleHash.constBegin();
        iterator != d->m_styleHash.constEnd();
        ++iterator ) {
        iterator.value()->pack( stream );
    }
}


void GeoDataDocument::unpack( QDataStream& stream )
{
    Q_D(GeoDataDocument);
    GeoDataContainer::unpack( stream );

    int size = 0;

    stream >> size;
    for( int i = 0; i < size; i++ ) {
        GeoDataStyle::Ptr style;
        style->unpack( stream );
        d->m_styleHash.insert(style->id(), style);
    }
}

}
