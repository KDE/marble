/*
    Copyright (C) 2008 Torsten Rahn <rahn@kde.org>

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

#include "GeoSceneLayer.h"

#include <limits>
#include "GeoSceneFilter.h"
#include "GeoSceneTypes.h"

namespace Marble
{

// FIXME: Filters are a Dataset.

GeoSceneAbstractDataset::GeoSceneAbstractDataset( const QString& name )
    : m_name( name ),
      m_fileFormat(),
      m_expire( std::numeric_limits<int>::max() )
{
}

QString GeoSceneAbstractDataset::name() const
{
    return m_name;
}

QString GeoSceneAbstractDataset::fileFormat() const
{
    return m_fileFormat;
}

void GeoSceneAbstractDataset::setFileFormat( const QString& fileFormat )
{
    m_fileFormat = fileFormat;
}

int GeoSceneAbstractDataset::expire() const
{
    return m_expire;
}

void GeoSceneAbstractDataset::setExpire( int expire )
{
    m_expire = expire;
}

class GeoSceneLayerPrivate
{
  public:
    GeoSceneLayerPrivate(){}
    ~GeoSceneLayerPrivate(){}
    
    const char* nodeType() const
    {
        return GeoSceneTypes::GeoSceneLayerType;
    }
};

GeoSceneLayer::GeoSceneLayer( const QString& name )
    : m_filter( 0 ),
      m_name( name ),
      m_backend(),
      m_role(),
      m_tiled( true ),
      d( new GeoSceneLayerPrivate )
{
}

GeoSceneLayer::~GeoSceneLayer()
{
   qDeleteAll( m_datasets );
}

const char* GeoSceneLayer::nodeType() const
{
    return d->nodeType();
}

void GeoSceneLayer::addDataset( GeoSceneAbstractDataset* dataset )
{
    // Remove any dataset that has the same name
    QVector<GeoSceneAbstractDataset *>::iterator it = m_datasets.begin();
    while (it != m_datasets.end()) {
        GeoSceneAbstractDataset * currentAbstractDataset = *it;
        if ( currentAbstractDataset->name() == dataset->name() ) {
            delete currentAbstractDataset;
            it = m_datasets.erase(it);
            break;
        }
        else {
            ++it;
        }
     }

    if ( dataset ) {
        m_datasets.append( dataset );
    }
}

const GeoSceneAbstractDataset* GeoSceneLayer::dataset( const QString& name ) const
{
    GeoSceneAbstractDataset* dataset = 0;

    QVector<GeoSceneAbstractDataset*>::const_iterator it = m_datasets.constBegin();
    QVector<GeoSceneAbstractDataset*>::const_iterator end = m_datasets.constEnd();
    for (; it != end; ++it) {
        if ( (*it)->name() == name ) {
            dataset = *it;
            break;
        }
    }
    return dataset;
}

// implement non-const method by means of const method,
// for details, see "Effective C++" (third edition)
GeoSceneAbstractDataset* GeoSceneLayer::dataset( const QString& name )
{
    return const_cast<GeoSceneAbstractDataset*>
        ( static_cast<GeoSceneLayer const *>( this )->dataset( name ));
}

const GeoSceneAbstractDataset * GeoSceneLayer::groundDataset() const
{
    if ( m_datasets.isEmpty() )
        return 0;

    return m_datasets.first();
}

// implement non-const method by means of const method,
// for details, see "Effective C++" (third edition)
GeoSceneAbstractDataset * GeoSceneLayer::groundDataset()
{
    return const_cast<GeoSceneAbstractDataset*>
        ( static_cast<GeoSceneLayer const *>( this )->groundDataset() );
}

QVector<GeoSceneAbstractDataset *> GeoSceneLayer::datasets() const
{
    return m_datasets;
}

QString GeoSceneLayer::name() const
{
    return m_name;
}

QString GeoSceneLayer::backend() const
{
    return m_backend;
}

void GeoSceneLayer::setBackend( const QString& backend )
{
    m_backend = backend;
}

bool GeoSceneLayer::isTiled() const
{
    return m_tiled;
}

void GeoSceneLayer::setTiled( bool tiled )
{
    m_tiled = tiled;
}

QString GeoSceneLayer::role() const
{
    return m_role;
}

void GeoSceneLayer::setRole( const QString& role )
{
    m_role = role;
}

const GeoSceneFilter* GeoSceneLayer::filter() const
{
    return m_filter;
}

GeoSceneFilter* GeoSceneLayer::filter()
{
    return m_filter;
}

void GeoSceneLayer::addFilter( GeoSceneFilter * filter )
{
    m_filter = filter;
}

void GeoSceneLayer::removeFilter( GeoSceneFilter * filter )
{
    if ( filter == m_filter ) {
        m_filter = 0;
    }
}

}
