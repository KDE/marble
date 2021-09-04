/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>

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

#include "GeoSceneAbstractDataset.h"
#include "GeoSceneFilter.h"
#include "GeoSceneTypes.h"

namespace Marble
{

class GeoSceneLayerPrivate
{
  public:
    GeoSceneLayerPrivate(const QString &name);
    ~GeoSceneLayerPrivate();

    /// The vector holding all the data in the layer.
    /// (We want to preserve the order and don't care
    /// much about speed here), so we don't use a hash
    QVector<GeoSceneAbstractDataset *> m_datasets;

    GeoSceneFilter  *m_filter;

    QString          m_name;
    QString          m_backend;
    QString          m_role;

    bool             m_tiled;
};

GeoSceneLayerPrivate::GeoSceneLayerPrivate(const QString &name) :
    m_filter(nullptr),
    m_name(name),
    m_backend(),
    m_role(),
    m_tiled(true)
{
}

GeoSceneLayerPrivate::~GeoSceneLayerPrivate()
{
    qDeleteAll(m_datasets);
}

GeoSceneLayer::GeoSceneLayer(const QString &name) :
    d(new GeoSceneLayerPrivate(name))
{
}

GeoSceneLayer::~GeoSceneLayer()
{
   delete d;
}

const char* GeoSceneLayer::nodeType() const
{
    return GeoSceneTypes::GeoSceneLayerType;
}

void GeoSceneLayer::addDataset( GeoSceneAbstractDataset* dataset )
{
    // Remove any dataset that has the same name
    QVector<GeoSceneAbstractDataset *>::iterator it = d->m_datasets.begin();
    while (it != d->m_datasets.end()) {
        GeoSceneAbstractDataset * currentAbstractDataset = *it;
        if ( currentAbstractDataset->name() == dataset->name() ) {
            delete currentAbstractDataset;
            d->m_datasets.erase(it);
            break;
        }
        else {
            ++it;
        }
     }

    if ( dataset ) {
        d->m_datasets.append( dataset );
    }
}

const GeoSceneAbstractDataset* GeoSceneLayer::dataset( const QString& name ) const
{
    GeoSceneAbstractDataset* dataset = nullptr;

    QVector<GeoSceneAbstractDataset*>::const_iterator it = d->m_datasets.constBegin();
    QVector<GeoSceneAbstractDataset*>::const_iterator end = d->m_datasets.constEnd();
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
    if (d->m_datasets.isEmpty())
        return nullptr;

    return d->m_datasets.first();
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
    return d->m_datasets;
}

QString GeoSceneLayer::name() const
{
    return d->m_name;
}

QString GeoSceneLayer::backend() const
{
    return d->m_backend;
}

void GeoSceneLayer::setBackend( const QString& backend )
{
    d->m_backend = backend;
}

bool GeoSceneLayer::isTiled() const
{
    return d->m_tiled;
}

void GeoSceneLayer::setTiled( bool tiled )
{
    d->m_tiled = tiled;
}

QString GeoSceneLayer::role() const
{
    return d->m_role;
}

void GeoSceneLayer::setRole( const QString& role )
{
    d->m_role = role;
}

const GeoSceneFilter* GeoSceneLayer::filter() const
{
    return d->m_filter;
}

GeoSceneFilter* GeoSceneLayer::filter()
{
    return d->m_filter;
}

void GeoSceneLayer::addFilter( GeoSceneFilter * filter )
{
    d->m_filter = filter;
}

void GeoSceneLayer::removeFilter( GeoSceneFilter * filter )
{
    if (filter == d->m_filter) {
        d->m_filter = nullptr;
    }
}

}
