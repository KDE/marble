/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_GEOSCENEFILTER_H
#define MARBLE_GEOSCENEFILTER_H

#include <QList>
#include <QString>

#include "GeoDocument.h"

namespace Marble
{

class GeoScenePalette;

/**
 * @short Filter of a GeoScene document.
 */

class GeoSceneFilter : public GeoNode
{
public:
    explicit GeoSceneFilter(const QString &name);
    ~GeoSceneFilter() override;

    QString name() const;
    void setName(const QString &name);

    QString type() const;
    void setType(const QString &type);

    QList<const GeoScenePalette *> palette() const;
    void addPalette(const GeoScenePalette *palette);
    int removePalette(const GeoScenePalette *palette); // returns # of items removed

    const char *nodeType() const override;

private:
    QString m_name;
    QString m_type;
    QList<const GeoScenePalette *> m_palette;
};

}

#endif
