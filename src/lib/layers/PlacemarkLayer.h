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


#include <QtCore/QObject>
#include "LayerInterface.h"

#include <QtCore/QVector>
#include <QtGui/QColor>

#include "PlacemarkLayout.h"

class QAbstractItemModel;
class QFont;
class QItemSelectionModel;
class QPainter;
class QString;

namespace Marble
{

class GeoPainter;
class GeoSceneLayer;
class MarbleClock;
class ViewportParams;
class VisiblePlacemark;

static const qreal s_labelOutlineWidth = 2.5;


class PlacemarkLayer : public QObject, public LayerInterface
{
    Q_OBJECT

 public:
    PlacemarkLayer( QAbstractItemModel *placemarkModel,
                    QItemSelectionModel *selectionModel,
                    MarbleClock *clock,
                    QObject *parent = 0 );
    ~PlacemarkLayer();

    /**
     * @reimp
     */
    QStringList renderPosition() const;

    /**
     * @reimp
     */
    qreal zValue() const;

    /**
     * @reimp
     */
    bool render( GeoPainter *painter, ViewportParams *viewport,
                 const QString &renderPos = "NONE", GeoSceneLayer *layer = 0 );

    /**
     * Returns a list of model indexes that are at position @p pos.
     */
    QVector<const GeoDataPlacemark*> whichPlacemarkAt( const QPoint &pos );

    // FIXME: To be removed after MapTheme / KML refactoring
    void setDefaultLabelColor( const QColor& color );

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

 Q_SIGNALS:
   void repaintNeeded();

 private:
    enum LabelStyle {
        Normal = 0,
        Glow,
        Selected
    };

    void drawLabelText( QPainter &labelPainter, const QString &text, const QFont &labelFont, LabelStyle labelStyle, const QColor &color );
    void drawLabelPixmap( VisiblePlacemark *mark );

    bool testXBug();

 private:
    PlacemarkLayout m_layout;
    bool m_useXWorkaround;  // Indicates need for an X windows workaround.

    // FIXME: To be removed after MapTheme / KML refactoring
    QColor m_defaultLabelColor;
};

}

#endif
