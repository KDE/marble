//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007-2008 Inge Wallin  <ingwa@kde.org>
// Copyright 2012      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

//
// PlacemarkLayer is responsible for drawing the Placemarks on the map
//

#ifndef MARBLE_PLACEMARKLAYER_H
#define MARBLE_PLACEMARKLAYER_H


#include <QObject>
#include "LayerInterface.h"

#include <QVector>
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


struct Fragment
{
    QVarLengthArray<QPainter::PixmapFragment, 16> fragments;
    QPixmap pixmap;
};

class PlacemarkLayer : public QObject, public LayerInterface
{
    Q_OBJECT

 public:
    PlacemarkLayer( QAbstractItemModel *placemarkModel,
                    QItemSelectionModel *selectionModel,
                    MarbleClock *clock,
                    const StyleBuilder *styleBuilder,
                    QObject *parent = nullptr );
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
    bool render( GeoPainter *painter, ViewportParams *viewport,
                 const QString &renderPos = QLatin1String("NONE"),
                 GeoSceneLayer *layer = nullptr ) override;

    RenderState renderState() const override;

    QString runtimeTrace() const override;

    /**
     * Returns a list of model indexes that are at position @p pos.
     */
    QVector<const GeoDataFeature *> whichPlacemarkAt( const QPoint &pos );

    bool hasPlacemarkAt(const QPoint &pos);

    bool isDebugModeEnabled() const;
    void setDebugModeEnabled(bool enabled);

    void setLevelTagDebugModeEnabled(bool enabled);
    bool levelTagDebugModeEnabled() const;
    void setDebugLevelTag(int level);

    static bool m_useXWorkaround;  // Indicates need for an X windows workaround.
 public Q_SLOTS:
   // earth
   void setShowPlaces( bool show );
   void setShowCities( bool show );
   void setShowTerrain( bool show );
   void setShowOtherPlaces( bool show );

   // other planets
   void setShowLandingSites( bool show );
   void setShowCraters( bool show );
   void setShowMaria( bool show );

   void requestStyleReset();
   void setTileLevel(int tileLevel);

 Q_SIGNALS:
   void repaintNeeded();

 private:
    void renderDebug(GeoPainter *painter, ViewportParams *viewport, const QVector<VisiblePlacemark*> & placemarks) const;
    static bool testXBug();

    PlacemarkLayout m_layout;
    bool m_debugModeEnabled;
    bool m_levelTagDebugModeEnabled;
    int m_tileLevel;
    int m_debugLevelTag;
};

}

#endif
