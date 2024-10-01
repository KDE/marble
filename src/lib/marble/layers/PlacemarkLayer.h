// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2006-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007-2008 Inge Wallin <ingwa@kde.org>
// SPDX-FileCopyrightText: 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

//
// PlacemarkLayer is responsible for drawing the Placemarks on the map
//

#ifndef MARBLE_PLACEMARKLAYER_H
#define MARBLE_PLACEMARKLAYER_H

#include "LayerInterface.h"
#include <QObject>

#include <QList>
#include <QPainter>

#include "PlacemarkLayout.h"

class QAbstractItemModel;
class QItemSelectionModel;
class QString;

namespace Marble
{

class GeoPainter;
class GeoSceneLayer;
class MarbleClock;
class ViewportParams;
class StyleBuilder;

struct Fragment {
    QVarLengthArray<QPainter::PixmapFragment, 16> fragments;
    QPixmap pixmap;
};

class PlacemarkLayer : public QObject, public LayerInterface
{
    Q_OBJECT

public:
    PlacemarkLayer(QAbstractItemModel *placemarkModel,
                   QItemSelectionModel *selectionModel,
                   MarbleClock *clock,
                   const StyleBuilder *styleBuilder,
                   QObject *parent = nullptr);
    ~PlacemarkLayer() override;

    /**
     * @reimp
     */
    QStringList renderPosition() const override;

    /**
     * @reimp
     */
    qreal zValue() const override;

    /**
     * @reimp
     */
    bool render(GeoPainter *painter, ViewportParams *viewport, const QString &renderPos = QLatin1String("NONE"), GeoSceneLayer *layer = nullptr) override;

    RenderState renderState() const override;

    QString runtimeTrace() const override;

    /**
     * Returns a list of model indexes that are at position @p pos.
     */
    QList<const GeoDataFeature *> whichPlacemarkAt(const QPoint &pos);

    bool hasPlacemarkAt(const QPoint &pos);

    bool isDebugModeEnabled() const;
    void setDebugModeEnabled(bool enabled);

    void setLevelTagDebugModeEnabled(bool enabled);
    bool levelTagDebugModeEnabled() const;
    void setDebugLevelTag(int level);

public Q_SLOTS:
    // earth
    void setShowPlaces(bool show);
    void setShowCities(bool show);
    void setShowTerrain(bool show);
    void setShowOtherPlaces(bool show);

    // other planets
    void setShowLandingSites(bool show);
    void setShowCraters(bool show);
    void setShowMaria(bool show);

    void requestStyleReset();
    void setTileLevel(int tileLevel);

Q_SIGNALS:
    void repaintNeeded();

private:
    void renderDebug(GeoPainter *painter, ViewportParams *viewport, const QList<VisiblePlacemark *> &placemarks) const;

    PlacemarkLayout m_layout;
    bool m_debugModeEnabled;
    bool m_levelTagDebugModeEnabled;
    int m_tileLevel;
    int m_debugLevelTag;
};

}

#endif
