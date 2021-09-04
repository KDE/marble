//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2019 Torsten Rahn <rahn@kde.org>
//

#include "MarbleQuickItem.h"
#include "GeoPolyline.h"
#include "Coordinate.h"

#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>
#include <QSGSimpleTextureNode>
#include <QSGTexture>
#include <QPolygonF>
#include <QtMath>

#include "MarbleGlobal.h"

using Marble::GeoDataCoordinates;
using Marble::EARTH_RADIUS;
using Marble::DEG2RAD;

namespace Marble
{
    GeoPolyline::GeoPolyline(QQuickItem *parent ) :
        QQuickItem( parent ),
        m_map(nullptr),
        m_observable(false),
        m_lineColor(Qt::black),
        m_lineWidth(1),
        m_tessellate(true)
    {
        setFlag(ItemHasContents, true);
    }

    MarbleQuickItem * GeoPolyline::map() const
    {
        return m_map;
    }

    void GeoPolyline::setMap(MarbleQuickItem *map)
    {
        if (m_map == map)
            return;

        m_map = map;

        connect(m_map, &MarbleQuickItem::visibleLatLonAltBoxChanged, this, &GeoPolyline::updateScreenPositions);
        emit mapChanged(m_map);
    }

    void GeoPolyline::updateScreenPositions() {
        if (m_map) {
            qDeleteAll(m_screenPolygons);
            m_screenPolygons.clear();
            m_map->screenCoordinatesFromGeoDataLineString(m_lineString, m_screenPolygons);
            m_screenCoordinates.clear();
            int i = 0;
            for (auto polygon : m_screenPolygons) {
                QVariantList m_polyline;
                QPolygonF screenPolygon = *polygon;
                for (auto node : screenPolygon) {
                    QVariantMap vmap;
                    vmap["x"] = node.x();
                    vmap["y"] = node.y();
                    m_polyline.append(vmap);
                }
                m_screenCoordinates.insert(i, m_polyline);
                ++i;
            }

            QRectF polygonBoundingRect;
            if (m_screenPolygons.length() == 1) {
                polygonBoundingRect = m_screenPolygons[0]->boundingRect();
            }
            else {
                QPolygonF polygons;
                for (auto polygon : m_screenPolygons) {
                    polygons << *polygon;
                }
                polygonBoundingRect = polygons.boundingRect();
            }
            setX(polygonBoundingRect.x());
            setY(polygonBoundingRect.y());
            setWidth(polygonBoundingRect.width());
            setHeight(polygonBoundingRect.height());

            emit screenCoordinatesChanged();
            emit readonlyXChanged();
            emit readonlyYChanged();
            emit readonlyWidthChanged();
            emit readonlyHeightChanged();
            update();
        }
    }

    bool GeoPolyline::observable() const
    {
        return m_observable;
    }

    QVariantList GeoPolyline::geoCoordinates() const
    {
        return m_geoCoordinates;
    }

    void GeoPolyline::setGeoCoordinates(const QVariantList & coordinates)
    {
        m_lineString.clear();
        m_lineString.setTessellate(m_tessellate);
        for(auto item : coordinates) {
            QVariantMap map = item.toMap();
            m_lineString << GeoDataCoordinates(
                                map["lon"].toReal(),
                                map["lat"].toReal(),
                                map["alt"].toReal(),
                                GeoDataCoordinates::Degree
                            );
        }

        if (m_geoCoordinates == coordinates)
            return;

        m_geoCoordinates = coordinates;
        emit geoCoordinatesChanged();
        updateScreenPositions();
    }

    QVariantList GeoPolyline::screenCoordinates() const
    {
        return m_screenCoordinates;
    }

    QColor GeoPolyline::lineColor() const
    {
        return m_lineColor;
    }

    qreal GeoPolyline::lineWidth() const
    {
        return m_lineWidth;
    }

    void GeoPolyline::setLineColor(const QColor& lineColor)
    {
        if (m_lineColor == lineColor)
            return;

        m_lineColor = lineColor;
        emit lineColorChanged(m_lineColor);
    }

    void GeoPolyline::setLineWidth(const qreal lineWidth)
    {
        if (m_lineWidth == lineWidth)
            return;

        m_lineWidth = lineWidth;
        emit lineWidthChanged(m_lineWidth);
    }

    bool GeoPolyline::tessellate() const
    {
        return m_tessellate;
    }

    void GeoPolyline::setTessellate(bool tessellate)
    {
        if (m_tessellate == tessellate)
            return;

        m_tessellate = tessellate;
        emit tessellateChanged(m_tessellate);
    }

    qreal GeoPolyline::readonlyX() const
    {
        return x();
    }

    qreal GeoPolyline::readonlyY() const
    {
        return y();
    }

    qreal GeoPolyline::readonlyWidth() const
    {
        return width();
    }

    qreal GeoPolyline::readonlyHeight() const
    {
        return height();
    }

    QSGNode *GeoPolyline::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *)
    {
        qreal const halfWidth = m_lineWidth;

        delete oldNode;
        oldNode = new QSGNode;

        if (m_screenPolygons.isEmpty()) return oldNode;

        for(int i = 0; i < m_screenPolygons.length(); ++i) {
            QPolygonF * polygon = m_screenPolygons[i];
            QVector<QVector2D> normals;
            int segmentCount = polygon->size() - 1;
            normals.reserve(segmentCount);
            for(int i = 0; i < segmentCount; ++i) {
                normals << QVector2D(polygon->at(i+1) - polygon->at(i)).normalized();
            }
            QSGGeometryNode* lineNode = new QSGGeometryNode;

            QSGGeometry * lineNodeGeo = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), segmentCount*2);
            lineNodeGeo->setDrawingMode(0x0005);
            lineNodeGeo->allocate((segmentCount + 1)*2);


            QSGFlatColorMaterial *material = new QSGFlatColorMaterial;
            material->setColor(m_lineColor);

            lineNode->setGeometry(lineNodeGeo);
            lineNode->setFlag(QSGNode::OwnsGeometry);
            lineNode->setMaterial(material);
            lineNode->setFlag(QSGNode::OwnsMaterial);

            auto points = lineNodeGeo->vertexDataAsPoint2D();
            int k = -1;
            for(int i = 0; i < segmentCount + 1; ++i) {
                auto const & a = mapFromItem(m_map, polygon->at(i));
                auto const & n = normals[qMin(i, segmentCount - 1)].toPointF();
                points[++k].set(a.x() - halfWidth * n.y(), a.y() + halfWidth * n.x());
                points[++k].set(a.x() + halfWidth * n.y(), a.y() - halfWidth * n.x());
            }
            oldNode->appendChildNode(lineNode);
        }

        return oldNode;
    }
}

#include "moc_GeoPolyline.cpp"
