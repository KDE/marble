// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Andrew Manson <g.real.ate@gmail.com>
// SPDX-FileCopyrightText: 2013 Thibaut Gridel <tgridel@free.fr>
// SPDX-FileCopyrightText: 2014 Calin Cruceru <crucerucalincristian@gmail.com>
//

// Self
#include "PlacemarkTextAnnotation.h"

// Qt
#include <QApplication>
#include <QDebug>
#include <QPalette>

// Marble
#include "AbstractProjection.h"
#include "GeoDataIconStyle.h"
#include "GeoDataLabelStyle.h"
#include "GeoDataPlacemark.h"
#include "GeoDataStyle.h"
#include "GeoPainter.h"
#include "MarbleDirs.h"
#include "SceneGraphicsTypes.h"
#include "ViewportParams.h"

namespace Marble
{

PlacemarkTextAnnotation::PlacemarkTextAnnotation(GeoDataPlacemark *placemark)
    : SceneGraphicsItem(placemark)
    , m_movingPlacemark(false)
{
    if (placemark->style()->iconStyle().iconPath().isNull()) {
        GeoDataStyle::Ptr newStyle(new GeoDataStyle(*placemark->style()));
        newStyle->iconStyle().setIconPath(MarbleDirs::path(QStringLiteral("bitmaps/redflag_22.png")));
        placemark->setStyle(newStyle);
    }
    setPaintLayers(QStringList() << QStringLiteral("PlacemarkTextAnnotation"));
}

PlacemarkTextAnnotation::~PlacemarkTextAnnotation()
{
    // nothing to do
}

void PlacemarkTextAnnotation::paint(GeoPainter *painter, const ViewportParams *viewport, const QString &layer, int tileZoomLevel)
{
    Q_UNUSED(layer);
    Q_UNUSED(painter);
    Q_UNUSED(tileZoomLevel);
    m_viewport = viewport;

    GeoDataStyle::Ptr newStyle(new GeoDataStyle(*placemark()->style()));
    GeoDataLabelStyle labelStyle = newStyle->labelStyle();

    if (labelStyle.color() != QApplication::palette().highlight().color())
        m_labelColor = labelStyle.color();

    if (hasFocus()) {
        labelStyle.setColor(QApplication::palette().highlight().color());
    } else {
        labelStyle.setColor(m_labelColor);
    }

    newStyle->setLabelStyle(labelStyle);
    placemark()->setStyle(newStyle);

    qreal x, y;
    viewport->currentProjection()->screenCoordinates(placemark()->coordinate(), viewport, x, y);
    m_region = QRegion(x - 10, y - 10, 20, 20);
}

bool PlacemarkTextAnnotation::containsPoint(const QPoint &eventPos) const
{
    return m_region.contains(eventPos);
}

void PlacemarkTextAnnotation::dealWithItemChange(const SceneGraphicsItem *other)
{
    Q_UNUSED(other);
}

void PlacemarkTextAnnotation::move(const GeoDataCoordinates &source, const GeoDataCoordinates &destination)
{
    Q_UNUSED(source);
    qreal lat = destination.latitude();
    qreal lon = destination.longitude();
    GeoDataCoordinates::normalizeLonLat(lon, lat);
    placemark()->setCoordinate(lon, lat);
}

const char *PlacemarkTextAnnotation::graphicType() const
{
    return SceneGraphicsTypes::SceneGraphicTextAnnotation;
}

QColor PlacemarkTextAnnotation::labelColor() const
{
    return m_labelColor;
}

bool PlacemarkTextAnnotation::mousePressEvent(QMouseEvent *event)
{
    setRequest(SceneGraphicsItem::NoRequest);

    if (state() == SceneGraphicsItem::Editing) {
        if (event->button() == Qt::LeftButton) {
            m_movingPlacemark = true;
        } else if (event->button() == Qt::RightButton) {
            setRequest(SceneGraphicsItem::ShowPlacemarkRmbMenu);
        }

        return true;
    }

    return false;
}

bool PlacemarkTextAnnotation::mouseMoveEvent(QMouseEvent *event)
{
    setRequest(SceneGraphicsItem::NoRequest);

    qreal lon, lat;
    m_viewport->geoCoordinates(event->pos().x(), event->pos().y(), lon, lat, GeoDataCoordinates::Radian);

    if (m_movingPlacemark) {
        placemark()->setCoordinate(lon, lat);
        return true;
    } else {
        setRequest(SceneGraphicsItem::ChangeCursorPlacemarkHover);
        return true;
    }

    return false;
}

bool PlacemarkTextAnnotation::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    setRequest(SceneGraphicsItem::NoRequest);

    m_movingPlacemark = false;
    return true;
}

void PlacemarkTextAnnotation::dealWithStateChange(SceneGraphicsItem::ActionState previousState)
{
    Q_UNUSED(previousState);
}

}
