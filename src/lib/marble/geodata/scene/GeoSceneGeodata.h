// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef MARBLE_GEOSCENEGEODATA_H
#define MARBLE_GEOSCENEGEODATA_H

#include "GeoDocument.h"
#include "GeoSceneAbstractDataset.h"
#include <geodata_export.h>

#include <QBrush>
#include <QPen>

namespace Marble
{

class GEODATA_EXPORT GeoSceneGeodata : public GeoSceneAbstractDataset
{
public:
    explicit GeoSceneGeodata(const QString &name);
    ~GeoSceneGeodata() override;
    const char *nodeType() const override;

    bool operator==(const GeoSceneGeodata &other) const;

    QString property() const;
    void setProperty(const QString &property);

    QString sourceFile() const;
    void setSourceFile(const QString &sourceFile);

    QString colorize() const;
    void setColorize(const QString &colorize);

    qreal alpha() const;
    void setAlpha(qreal alpha);

    QPen pen() const;
    void setPen(const QPen &pen);

    QBrush brush() const;
    void setBrush(const QBrush &brush);

    int renderOrder() const;
    void setRenderOrder(int renderOrder);

    QList<QColor> colors() const;
    void setColors(const QList<QColor> &colors);

private:
    QString m_property;
    QString m_sourceFile;
    QString m_colorize;
    qreal m_alpha;
    QPen m_pen;
    QBrush m_brush;
    int m_renderOrder;
    QList<QColor> m_colors;
};

}

#endif
