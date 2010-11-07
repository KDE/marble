//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

//
// PlacemarkLayout is responsible for drawing the Placemarks on the map
//

#ifndef MARBLE_PLACEMARKLAYOUT_H
#define MARBLE_PLACEMARKLAYOUT_H


#include <QtCore/QHash>
#include <QtCore/QModelIndex>
#include <QtCore/QRect>
#include <QtCore/QVector>

#include "GeoDataPlacemark.h"   // In geodata/data/

class QAbstractItemModel;
class QItemSelectionModel;
class QPainter;
class QPoint;


namespace Marble
{

class PlacemarkPainter;
class VisiblePlacemark;
class ViewParams;

/**
 * Layouts the place marks with a passed QPainter.
 */



class PlacemarkLayout : public QObject
{
    Q_OBJECT

 public:
    /**
     * Creates a new place mark layout.
     */
    explicit PlacemarkLayout( const QAbstractItemModel  *placemarkModel,
                              const QItemSelectionModel *selectionModel,
                              QObject *parent = 0 );

    /**
     * Destroys the place mark painter.
     */
    ~PlacemarkLayout();

    /**
     * Layouts the place marks.
     *
     * @param painter The painter that is used for painting.
     * @param viewParams Parameters that influence the painting.
     * @param placemarkModel The PlacemarkModel or a proxy model on top of it.
     * @param selectionModel The selection model for the PlacemarkModel.
     * @param planetAxis The position of the planet axis.
     * @param firstTime Whether the map is painted the first time.
     */
    void paintPlaceFolder( QPainter *painter, ViewParams *viewParams );

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
    QVector<QModelIndex> whichPlacemarkAt( const QPoint &pos );

    PlacemarkPainter* placemarkPainter() const;

 public Q_SLOTS:

    void requestStyleReset();

 private:
    void styleReset();

    QRect  roomForLabel( GeoDataStyle * style,
                         const QVector<VisiblePlacemark*> &currentsec,
                         const int x, const int y,
                         const int textWidth );

    int    placemarksOnScreenLimit() const;

 private:
    Q_DISABLE_COPY( PlacemarkLayout )
    const QAbstractItemModel  *const m_placemarkModel;
    const QItemSelectionModel *const m_selectionModel;

    PlacemarkPainter *m_placemarkPainter;

    QVector<VisiblePlacemark*> m_paintOrder;
    QVector<VisiblePlacemark*> m_placemarkPool;
    QHash<QModelIndex, VisiblePlacemark*> m_visiblePlacemarks;

    QModelIndexList m_indexList;

    QVector< int > m_weightfilter;

    int     m_maxLabelHeight;
    bool    m_styleResetRequested;
};

}

#endif
