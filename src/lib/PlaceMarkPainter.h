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
// PlaceMarkPainter is responsible for drawing the PlaceMarks on the map
//


#ifndef PLACEMARKPAINTER_H
#define PLACEMARKPAINTER_H


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
class VisiblePlaceMark;
class ViewParams;

/**
 * Layouts and paints the place marks with a passed
 * QPainter.
 */
class PlaceMarkPainter : public QObject
{
    Q_OBJECT

 public:
    /**
     * Creates a new place mark painter.
     */
    PlaceMarkPainter( QObject *parent = 0 );

    /**
     * Destroys the place mark painter.
     */
    ~PlaceMarkPainter();

    /**
     * Layouts the paints the place marks.
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
     * Sets the @p color that shall be used for painting the labels.
     */
    void setLabelColor( const QColor &color );

    /**
     * Returns a list of model indexes that are at position @p pos.
     */
    QVector<QPersistentModelIndex> whichPlaceMarkAt( const QPoint &pos ) const;

 private:
    void sphericalPaintPlaceFolder(QPainter*, int imgwidth, int imgheight,
                          ViewParams *viewParams,
                          const QAbstractItemModel*,
                          const QItemSelectionModel*,
                          Quaternion,
                          bool firstTime );
    void rectangularPaintPlaceFolder(QPainter*, int imgwidth, int imgheight,
                          ViewParams *viewParams,
                          const QAbstractItemModel*,
                          const QItemSelectionModel*,
                          Quaternion,
                          bool firstTime );

    void   labelFontData( VisiblePlaceMark *mark,
                          QFont &font, int &fontwidth, bool isSelected );
    bool   roomForLabel( const QVector<VisiblePlaceMark*> &currentsec,
                         VisiblePlaceMark *mark,
                         int textwidth,
                         int x, int y );
    void drawLabelText( QPainter& textpainter, const QString &name, const QFont &font );
    void drawLabelPixmap( VisiblePlaceMark *mark, int textWidth,
                          const QFont &font, bool isSelected );

    bool testXBug();

 private:
    QFont  m_font_regular;
    QFont  m_font_regular_italics;
    QFont  m_font_regular_underline;
    QFont  m_font_mountain;

    QVector<VisiblePlaceMark*> m_visiblePlaceMarks;
    QHash<QPersistentModelIndex, VisiblePlaceMark*> m_allPlaceMarks;

    QColor  m_labelcolor;
    int     m_fontheight;
    int     m_fontascent;
    int     m_labelareaheight;

    QVector< int > m_weightfilter;
    QPixmap  m_empty;
    double   m_widthscale;

    bool     m_useXWorkaround;  // Indicates need for an X windows workaround.
};


#endif // PLACEMARKPAINTER_H
