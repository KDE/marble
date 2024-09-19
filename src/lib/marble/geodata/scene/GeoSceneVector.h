/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_GEOSCENEVECTOR_H
#define MARBLE_GEOSCENEVECTOR_H

#include <QBrush>
#include <QPen>
#include <QString>

#include "GeoSceneAbstractDataset.h"

namespace Marble
{

/**
 * @short Vector dataset stored in a layer.
 */

class GEODATA_EXPORT GeoSceneVector : public GeoSceneAbstractDataset
{
public:
    explicit GeoSceneVector(const QString &name);
    const char *nodeType() const override;

    QString sourceFile() const;
    void setSourceFile(const QString &sourceFile);

    QString feature() const;
    void setFeature(const QString &feature);

    QPen pen() const;
    void setPen(const QPen &pen);

    QBrush brush() const;
    void setBrush(const QBrush &brush);

private:
    QString m_sourceFile;
    QString m_feature;

    QPen m_pen;
    QBrush m_brush;
};

}

#endif
