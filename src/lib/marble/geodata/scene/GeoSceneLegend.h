/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_GEOSCENELEGEND_H
#define MARBLE_GEOSCENELEGEND_H

#include "GeoDocument.h"
#include <QList>

#include <geodata_export.h>

namespace Marble
{

class GeoSceneSection;

class GeoSceneLegendPrivate;

/**
 * @short Legend of a GeoScene document.
 */

class GEODATA_EXPORT GeoSceneLegend : public GeoNode
{
public:
    GeoSceneLegend();
    ~GeoSceneLegend() override;

    const char *nodeType() const override;

    /**
     * @brief  Add a section to the legend
     * @param  section  the new section
     */
    void addSection(const GeoSceneSection *);
    QList<const GeoSceneSection *> sections() const;

private:
    Q_DISABLE_COPY(GeoSceneLegend)
    GeoSceneLegendPrivate *const d;
};

}

#endif
