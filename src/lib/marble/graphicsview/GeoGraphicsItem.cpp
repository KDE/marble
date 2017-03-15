//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "GeoGraphicsItem.h"
#include "GeoGraphicsItem_p.h"

#include "GeoDataPlacemark.h"

// Qt
#include "MarbleDebug.h"

#include <QColor>

using namespace Marble;

GeoGraphicsItem::GeoGraphicsItem( const GeoDataFeature *feature )
    : d( new GeoGraphicsItemPrivate( feature ) )
{
    setFlag( ItemIsVisible, true );
}

GeoGraphicsItem::~GeoGraphicsItem()
{
    delete d;
}

bool GeoGraphicsItem::visible() const
{
    return d->m_flags & ItemIsVisible;
}

void GeoGraphicsItem::setVisible( bool visible )
{
    setFlag( ItemIsVisible, visible );
}

GeoGraphicsItem::GeoGraphicsItemFlags GeoGraphicsItem::flags() const
{
    return d->m_flags;
}

void GeoGraphicsItem::setFlag( GeoGraphicsItemFlag flag, bool enabled )
{
    if( enabled ) {
        d->m_flags = d->m_flags | flag;
    } else {
        d->m_flags = d->m_flags & ~flag;
    }
}

void GeoGraphicsItem::setFlags( GeoGraphicsItemFlags flags )
{
    d->m_flags = flags;
}

const GeoDataFeature* GeoGraphicsItem::feature() const
{
    return d->m_feature;
}

void GeoGraphicsItem::setHighlightStyle( const GeoDataStyle::ConstPtr &highlightStyle)
{
    /**
     * Delete any previously set style
     * and assign the new style @highlightStyle
     */
    d->m_highlightStyle = highlightStyle;
}

GeoDataStyle::ConstPtr GeoGraphicsItem::style() const
{
    /**
     * m_isHighlight is set true when the item is
     * supposed to be colored highlighted
     */
    if ( d->m_highlighted && d->m_highlightStyle ) {
        return d->m_highlightStyle;
    }

    if (!d->m_style) {
        if (const GeoDataPlacemark *placemark = geodata_cast<GeoDataPlacemark>(d->m_feature)) {
            auto styling = StyleParameters(placemark, d->m_renderContext.tileLevel());
            for (auto relation: d->m_relations) {
                if (relation->isVisible()) {
                    styling.relation = relation;
                    break;
                }
            }
            d->m_style = d->m_styleBuilder->createStyle(styling);
        } else {
            d->m_style = d->m_feature->style();
        }
    }

    return d->m_style;
}

void GeoGraphicsItem::setStyleBuilder(const StyleBuilder *styleBuilder)
{
    d->m_styleBuilder = styleBuilder;
}

void GeoGraphicsItem::resetStyle()
{
    d->m_style = GeoDataStyle::ConstPtr();
    handleRelationUpdate(d->m_relations);
}

qreal GeoGraphicsItem::zValue() const
{
    return d->m_zValue;
}

void GeoGraphicsItem::setZValue( qreal z )
{
    d->m_zValue = z;
}

void GeoGraphicsItem::setHighlighted( bool highlight )
{
    d->m_highlighted = highlight;
}

bool GeoGraphicsItem::isHighlighted() const
{
    return d->m_highlighted;
}

QStringList GeoGraphicsItem::paintLayers() const
{
    return d->m_paintLayers;
}

void GeoGraphicsItem::setPaintLayers(const QStringList &paintLayers)
{
    d->m_paintLayers = paintLayers;
}

void GeoGraphicsItem::setRenderContext(const RenderContext &renderContext)
{
    if (renderContext != d->m_renderContext) {
        d->m_renderContext = renderContext;
        d->m_style = GeoDataStyle::ConstPtr();
    }
}

bool GeoGraphicsItem::contains(const QPoint &, const ViewportParams *) const
{
    return false;
}

void GeoGraphicsItem::setRelations(const QSet<const GeoDataRelation*> &relations)
{
    d->m_relations.clear();
    std::copy(relations.begin(), relations.end(), std::back_inserter(d->m_relations));
    std::sort(d->m_relations.begin(), d->m_relations.end(),
    [](const GeoDataRelation * a, const GeoDataRelation * b) {
        return *a < *b;
    });

    d->m_style = GeoDataStyle::ConstPtr();
    handleRelationUpdate(d->m_relations);
}

void GeoGraphicsItem::handleRelationUpdate(const QVector<const GeoDataRelation *> &)
{
    // does nothing
}

int GeoGraphicsItem::minZoomLevel() const
{
    return d->m_minZoomLevel;
}

void GeoGraphicsItem::setMinZoomLevel(int zoomLevel)
{
    d->m_minZoomLevel = zoomLevel;
}

bool GeoGraphicsItem::zValueLessThan(GeoGraphicsItem *one, GeoGraphicsItem *two)
{
    return one->d->m_zValue < two->d->m_zValue;
}

bool GeoGraphicsItem::styleLessThan(GeoGraphicsItem *one, GeoGraphicsItem *two)
{
    return reinterpret_cast<quint64>(one->d->m_style.data()) < reinterpret_cast<quint64>(two->d->m_style.data());
}

bool GeoGraphicsItem::zValueAndStyleLessThan(GeoGraphicsItem *one, GeoGraphicsItem *two)
{
    if (one->d->m_zValue == two->d->m_zValue) {
        return reinterpret_cast<quint64>(one->d->m_style.data()) < reinterpret_cast<quint64>(two->d->m_style.data());
    }

    return one->d->m_zValue < two->d->m_zValue;
}


bool RenderContext::operator==(const RenderContext &other) const
{
    return m_tileLevel == other.m_tileLevel;
}

bool RenderContext::operator!=(const RenderContext &other) const
{
    return !operator==(other);
}

int RenderContext::tileLevel() const
{
    return m_tileLevel;
}

RenderContext::RenderContext(int tileLevel) :
    m_tileLevel(tileLevel)
{
    // nothing to do
}
