//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

//
// PlaceMarkLayout is responsible for drawing the PlaceMarks on the map
//


#ifndef PLACEMARKLAYOUT_H
#define PLACEMARKLAYOUT_H


#include <QtGui/QFont>
#include <QtGui/QPainterPath>
#include <QtGui/QPixmap>
#include <QtCore/QPersistentModelIndex>
#include <QtCore/QRect>
#include <QtCore/QSet>
#include <QtCore/QVector>

#include "Quaternion.h"
#include "PlaceMark.h"
#include "PlaceMarkContainer.h"

class QAbstractItemModel;
class QItemSelectionModel;
class QPainter;
class PlaceMarkPainter;
class VisiblePlaceMark;
class ViewParams;

/**
 * Layouts the place marks with a passed QPainter.
 */

class PlaceMarkLayout : public QObject
{
    Q_OBJECT

 public:
    /**
     * Creates a new place mark layout.
     */
    PlaceMarkLayout( QObject *parent = 0 );

    /**
     * Destroys the place mark painter.
     */
    ~PlaceMarkLayout();

    /**
     * Layouts the place marks.
     *
     * @param painter The painter that is used for painting.
     * @param width The width of the image that shall be painted.
     * @param height The height of the image that shall be painted.
     * @param viewParams Parameters that influence the painting.
     * @param placeMarkModel The PlaceMarkModel or a proxy model on top of it.
     * @param selectionModel The selection model for the PlaceMarkModel.
     * @param planetAxis The position of the planet axis.
     * @param firstTime Whether the map is painted the first time.
     */
    void paintPlaceFolder( QPainter *painter, int width, int height,
                           ViewParams *viewParams,
                           const QAbstractItemModel *placeMarkModel,
                           const QItemSelectionModel *selectionModel,
                           Quaternion planetAxis,
                           bool firstTime = true );

    /**
     * Returns a list of model indexes that are at position @p pos.
     */
    QVector<QPersistentModelIndex> whichPlaceMarkAt( const QPoint &pos ) const;

    PlaceMarkPainter* placeMarkPainter() const;

    void reset();

 private:

    void   labelFontData( const QPersistentModelIndex& index,
                          const bool isSelected, QFont &font, int &fontwidth );

    bool   locatedOnScreen ( const QPersistentModelIndex &index, 
                             int &x, int &y, const int &imgwidth, const int &imgheight,
                             const Quaternion inversePlanetAxis,
                             ViewParams * viewParams );

    QRect  roomForLabel ( const QPersistentModelIndex& index,
                         const QVector<VisiblePlaceMark*> &currentsec,
                         const int x, const int y,
                         const int textwidth );

 private:
    PlaceMarkPainter *m_placeMarkPainter;

    QFont  m_font_regular;
    QFont  m_font_regular_italics;
    QFont  m_font_regular_underline;
    QFont  m_font_mountain;

    QVector<VisiblePlaceMark*> m_paintOrder;
    QHash<QPersistentModelIndex, VisiblePlaceMark*> m_visiblePlaceMarks;

    int     m_fontheight;
    int     m_fontascent;
    int     m_labelareaheight;

    QVector< int > m_weightfilter;
};


#endif // PLACEMARKLAYOUT_H
