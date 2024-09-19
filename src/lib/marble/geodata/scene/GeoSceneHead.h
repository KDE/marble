/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_GEOSCENEHEAD_H
#define MARBLE_GEOSCENEHEAD_H

#include "GeoDocument.h"

#include <geodata_export.h>

#include <QtGlobal>

class QString;

namespace Marble
{

class GeoSceneIcon;
class GeoSceneZoom;
class GeoSceneLicense;

class GeoSceneHeadPrivate;

/**
 * @short General properties and identifiers of a GeoScene document.
 */
class GEODATA_EXPORT GeoSceneHead : public GeoNode
{
public:
    GeoSceneHead();
    ~GeoSceneHead() override;

    const char *nodeType() const override;

    QString name() const;
    void setName(const QString &name);
    QString target() const;
    void setTarget(const QString &target);
    QString theme() const;
    void setTheme(const QString &theme);

    /** Planet radius, 0.0 if unknown */
    qreal radius() const;
    void setRadius(qreal radius);

    QString mapThemeId() const;

    QString description() const;
    void setDescription(const QString &);

    bool visible() const;
    void setVisible(bool visible);

    const GeoSceneZoom *zoom() const;
    GeoSceneZoom *zoom();

    const GeoSceneIcon *icon() const;
    GeoSceneIcon *icon();

    const GeoSceneLicense *license() const;
    GeoSceneLicense *license();

private:
    Q_DISABLE_COPY(GeoSceneHead)
    GeoSceneHeadPrivate *const d;
};

}

#endif
