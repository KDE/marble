//
// This file is part of the Marble Virtual Globe.
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

class QAbstractItemModel;
class QSortFilterProxyModel;
class QItemSelectionModel;
class QPainter;
class QPoint;


namespace Marble
{

class GeoDataCoordinates;
class GeoDataPlacemark;
class GeoDataStyle;
class PlacemarkPainter;
class TileId;
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
    PlacemarkLayout( QAbstractItemModel  *placemarkModel,
                     QItemSelectionModel *selectionModel,
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
     */
    void paintPlaceFolder( QPainter *painter, ViewParams *viewParams );

    /**
     * Returns a the maximum height of all possible labels.
     * WARNING: This is a really slow method as it traverses all placemarks
     * to check the labelheight.
     * FIXME: Once a StyleManager that manages all styles has been implemented
     * just traverse all existing styles. 
     */

    int maxLabelHeight() const;

    TileId placemarkToTileId( const GeoDataCoordinates& coords, int popularity ) const;

    /**
     * Returns a list of model indexes that are at position @p pos.
     */
    QVector<const GeoDataPlacemark*> whichPlacemarkAt( const QPoint &pos );

 public Q_SLOTS:

    void requestStyleReset();
    void setCacheData();

 private:
    void styleReset();

    QRect  roomForLabel( GeoDataStyle * style,
                         const QVector<VisiblePlacemark*> &currentsec,
                         const int x, const int y,
                         const int textWidth );

    int    placemarksOnScreenLimit() const;

 private:
    Q_DISABLE_COPY( PlacemarkLayout )
    QSortFilterProxyModel  *const m_placemarkModel;
    QItemSelectionModel *const m_selectionModel;

    PlacemarkPainter *m_placemarkPainter;

    QVector<VisiblePlacemark*> m_paintOrder;
    QHash<GeoDataPlacemark*, VisiblePlacemark*> m_visiblePlacemarks;

    /// map providing the list of placemark belonging in TileId as key
    QMap<TileId, QList<GeoDataPlacemark*> > m_placemarkCache;

    QVector< int > m_weightfilter;

    int     m_maxLabelHeight;
    bool    m_styleResetRequested;
};

}

#endif
