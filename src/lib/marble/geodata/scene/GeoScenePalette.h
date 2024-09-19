/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_GEOSCENEPALETTE_H
#define MARBLE_GEOSCENEPALETTE_H

#include <QString>

#include "GeoDocument.h"

namespace Marble
{

/**
 * @short Palette of a GeoScene document.
 */

class GeoScenePalette : public GeoNode
{
public:
    GeoScenePalette(const QString &type, const QString &file);

    QString type() const;
    void setType(const QString &type);

    QString file() const;
    void setFile(const QString &file);

    bool operator==(const GeoScenePalette &rhs) const;

    const char *nodeType() const override;

private:
    QString m_type;
    QString m_file;
};

}

#endif
