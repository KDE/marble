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


#include <QtCore/QPersistentModelIndex>
#include <QtCore/QRect>
#include <QtCore/QSet>
#include <QtCore/QVector>

#include "Quaternion.h"

#include "GeoDataPlacemark.h"   // In geodata/data/

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
    explicit PlaceMarkLayout( QObject *parent = 0 );

    /**
     * Destroys the place mark painter.
     */
    ~PlaceMarkLayout();

    /**
     * Layouts the place marks.
     *
     * @param painter The painter that is used for painting.
     * @param viewParams Parameters that influence the painting.
     * @param placeMarkModel The PlaceMarkModel or a proxy model on top of it.
     * @param selectionModel The selection model for the PlaceMarkModel.
     * @param planetAxis The position of the planet axis.
     * @param firstTime Whether the map is painted the first time.
     */
    void paintPlaceFolder( QPainter *painter, ViewParams *viewParams,
                           const QAbstractItemModel  *placeMarkModel,
                           const QItemSelectionModel *selectionModel,
                           bool firstTime = true );

    /**
     * Returns a the maximum height of all possible labels.
     * WARNING: This is a really slow method as it traverses all placemarks
     * to check the labelheight.
     * FIXME: Once a StyleManager that manages all styles has been implemented
     * just traverse all existing styles. 
     */

    int maxLabelHeight( const QAbstractItemModel* model,
                        const QItemSelectionModel* selectionModel ) const;

    /**
     * Returns a list of model indexes that are at position @p pos.
     */
    QVector<QPersistentModelIndex> whichPlaceMarkAt( const QPoint &pos );

    PlaceMarkPainter* placeMarkPainter() const;

 public Q_SLOTS:

    void requestStyleReset();

 private:
    void styleReset();

    QRect  roomForLabel ( GeoDataStyle * style,
                         const QVector<VisiblePlaceMark*> &currentsec,
                         const int x, const int y,
                         const int textWidth );

    int    placeMarksOnScreenLimit() const;

 private:
    PlaceMarkPainter *m_placeMarkPainter;

    QVector<VisiblePlaceMark*> m_paintOrder;
    QVector<VisiblePlaceMark*> m_placeMarkPool;
    QHash<QPersistentModelIndex, VisiblePlaceMark*> m_visiblePlaceMarks;

    QList<QPersistentModelIndex> m_persistentIndexList;

    QVector< int > m_weightfilter;

    int     m_maxLabelHeight;
    bool    m_styleResetRequested;
};


#endif // PLACEMARKLAYOUT_H
