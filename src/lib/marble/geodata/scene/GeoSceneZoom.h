/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_GEOSCENEZOOM_H
#define MARBLE_GEOSCENEZOOM_H

#include "GeoDocument.h"
#include <geodata_export.h>

namespace Marble
{

/**
 * @short Zoom properties of a GeoScene document.
 */
class GEODATA_EXPORT GeoSceneZoom : public GeoNode
{
public:
    GeoSceneZoom();

    int minimum() const;
    void setMinimum(int name);
    int maximum() const;
    void setMaximum(int target);
    bool discrete() const;
    void setDiscrete(bool theme);

    const char *nodeType() const override;

private:
    // FIXME: d-pointerfy

    int m_minimum;
    int m_maximum;
    bool m_discrete;
};

}

#endif
