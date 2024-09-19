/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_GEOSCENEICON_H
#define MARBLE_GEOSCENEICON_H

#include <QColor>
#include <QString>

#include <geodata_export.h>

#include "GeoDocument.h"

namespace Marble
{

/**
 * @short Icon properties of a GeoScene document.
 */
class GEODATA_EXPORT GeoSceneIcon : public GeoNode
{
public:
    GeoSceneIcon();
    ~GeoSceneIcon() override;

    QString pixmap() const;
    void setPixmap(const QString &);

    QColor color() const;
    void setColor(const QColor &);

    const char *nodeType() const override;

private:
    // FIXME: d-pointerfy

    QString m_pixmap;
    QColor m_color;
};

}

#endif
