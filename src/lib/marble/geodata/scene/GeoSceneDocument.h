/*
    SPDX-FileCopyrightText: 2007 Nikolas Zimmermann <zimmermann@kde.org>
    SPDX-FileCopyrightText: 2007 Murad Tagirov <tmurad@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_GEOSCENEDOCUMENT_H
#define MARBLE_GEOSCENEDOCUMENT_H

#include "GeoDocument.h"
#include <QObject>

#include <geodata_export.h>

namespace Marble
{

class GeoSceneHead;
class GeoSceneLegend;
class GeoSceneMap;
class GeoSceneSettings;

class GeoSceneDocumentPrivate;

/**
 * @short A container for features parsed from the DGML file.
 */
class GEODATA_EXPORT GeoSceneDocument : public QObject, public GeoDocument, public GeoNode
{
    Q_OBJECT

public:
    GeoSceneDocument();
    ~GeoSceneDocument() override;

    const char *nodeType() const override;

    bool isGeoSceneDocument() const override
    {
        return true;
    }

    const GeoSceneHead *head() const;
    GeoSceneHead *head();

    const GeoSceneMap *map() const;
    GeoSceneMap *map();

    const GeoSceneSettings *settings() const;
    GeoSceneSettings *settings();

    const GeoSceneLegend *legend() const;
    GeoSceneLegend *legend();

Q_SIGNALS:
    void valueChanged(const QString &, bool);

private:
    Q_DISABLE_COPY(GeoSceneDocument)
    GeoSceneDocumentPrivate *const d;
};

}

#endif
