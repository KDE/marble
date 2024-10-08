// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2019 Torsten Rahn <rahn@kde.org>
//

#ifndef MARBLE_DECLARATIVE_GEOPOLYLINEITEM_H
#define MARBLE_DECLARATIVE_GEOPOLYLINEITEM_H

#include "GeoDataCoordinates.h"
#include "GeoDataLineString.h"
#include <QObject>
#include <QQuickItem>
#include <QtQml>

/**
 * Represents a coordinate with the properties of a name and coordinates
 *
 * @todo: Introduce GeoDataCoordinates
 */
namespace Marble
{
class MarbleQuickItem;

class GeoPolyline : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(qreal x READ readonlyX NOTIFY readonlyXChanged)
    Q_PROPERTY(qreal y READ readonlyY NOTIFY readonlyYChanged)
    Q_PROPERTY(qreal width READ readonlyWidth NOTIFY readonlyWidthChanged)
    Q_PROPERTY(qreal height READ readonlyHeight NOTIFY readonlyHeightChanged)

    Q_PROPERTY(Marble::MarbleQuickItem *map READ map WRITE setMap NOTIFY mapChanged)

    Q_PROPERTY(QVariantList geoCoordinates READ geoCoordinates WRITE setGeoCoordinates NOTIFY geoCoordinatesChanged)
    Q_PROPERTY(QVariantList screenCoordinates READ screenCoordinates NOTIFY screenCoordinatesChanged)
    Q_PROPERTY(bool observable READ observable NOTIFY observableChanged)
    Q_PROPERTY(bool tessellate READ tessellate WRITE setTessellate NOTIFY tessellateChanged)
    Q_PROPERTY(QColor lineColor READ lineColor WRITE setLineColor NOTIFY lineColorChanged)
    Q_PROPERTY(qreal lineWidth READ lineWidth WRITE setLineWidth NOTIFY lineWidthChanged)

    // When enabled only those screenCoordinates are calculated and rendered that are visible on the screen.
    // Setting this to false can severely reduce performance. Therefore this defaults to true.
    Q_PROPERTY(bool clipScreenCoordinates READ clipScreenCoordinates WRITE setClipScreenCoordinates NOTIFY clipScreenCoordinatesChanged)

public:
    /** Constructor */
    explicit GeoPolyline(QQuickItem *parent = nullptr);

    MarbleQuickItem *map() const;

    void setMap(MarbleQuickItem *map);

    bool observable() const;

    QVariantList geoCoordinates() const;

    void setGeoCoordinates(const QVariantList &geoCoordinates);

    QVariantList screenCoordinates() const;

    QColor lineColor() const;
    qreal lineWidth() const;
    bool tessellate() const;
    bool clipScreenCoordinates() const;

    void setLineColor(const QColor &lineColor);
    void setLineWidth(const qreal lineWidth);
    void setTessellate(bool tessellate);
    void setClipScreenCoordinates(bool clipped);

    qreal readonlyX() const;

    qreal readonlyY() const;

    qreal readonlyWidth() const;

    qreal readonlyHeight() const;

Q_SIGNALS:
    void mapChanged(MarbleQuickItem *map);
    void observableChanged(bool observable);
    void geoCoordinatesChanged();
    void screenCoordinatesChanged();
    void lineColorChanged(QColor lineColor);
    void lineWidthChanged(qreal lineWidth);

    void tessellateChanged(bool tessellate);
    void clipScreenCoordinatesChanged(bool enabled);

    void readonlyXChanged();
    void readonlyYChanged();
    void readonlyWidthChanged();
    void readonlyHeightChanged();

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

private:
    MarbleQuickItem *m_map = nullptr;
    bool m_observable;
    GeoDataLineString m_lineString;
    QVariantList m_geoCoordinates;
    QList<QPolygonF> m_screenPolygons;
    QVariantList m_screenCoordinates;
    QColor m_lineColor;
    qreal m_lineWidth;
    bool m_tessellate;
    bool m_clipScreenCoordinates;

    void updateScreenPositions();
};
}

#endif // MARBLE_DECLARATIVE_GEOPOLYLINEITEM_H
