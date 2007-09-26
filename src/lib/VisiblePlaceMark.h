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
// VisiblePlaceMarks are those PlaceMarks which become visible on the map
//


#ifndef VISIBLEPLACEMARK_H
#define VISIBLEPLACEMARK_H

#include <QtGui/QFont>
#include <QtGui/QPixmap>
#include <QtCore/QPersistentModelIndex>
#include <QtCore/QRect>

#include "PlaceMarkModel.h"

/**
 * @short A class which represents the visible place marks on a map.
 *
 * This class is used by PlaceMarkLayout to pass the visible place marks
 * to the PlaceMarkPainter.
 */
class VisiblePlaceMark
{
 public:
    /**
     * Creates a new visible place mark.
     */
    VisiblePlaceMark();

    /**
     * Destroys the visible place mark.
     */
    ~VisiblePlaceMark();

    /**
     * Returns the index of the place mark model which
     * is associated with this visible plave mark.
     */
    const QPersistentModelIndex& modelIndex() const;

    /**
     * Sets the @p index of the place mark model which
     * is associated with this visible plave mark.
     */
    void setModelIndex( const QPersistentModelIndex &index );

    /**
     * Returns the name of the place mark.
     */
    const QString name() const;

    /**
     * Returns the pixmap of the place mark symbol.
     */
    const QPixmap& symbolPixmap() const;

    /**
     * Returns the size of the place mark symbol.
     */
    const QSize& symbolSize() const;

    /**
     * Returns the position of the place mark symbol on the map.
     */
    const QPoint& symbolPos() const;

    /**
     * Sets the position of the place mark symbol on the map.
     */
    void setSymbolPos( const QPoint& sympos );

    /**
     * Returns the pixmap of the place mark name label.
     */
    const QPixmap& labelPixmap() const;

    /**
     * Sets the @p pixmap of the place mark name label.
     */
    void setLabelPixmap( const QPixmap& pixmap );

    /**
     * Returns the font used for the place mark name label.
     */
    const QFont& labelFont() const;

    /**
     * Sets the @p font used for the place mark name label.
     */
    void setLabelFont( const QFont& font );

    /**
     * Returns the area covered by the place mark name label on the map.
     */
    const QRect& labelRect() const;

    /**
     * Sets the @p area covered by the place mark name label on the map.
     */
    void setLabelRect( const QRect& area );

 private:
    QPersistentModelIndex m_modelIndex;

    // View stuff
    QPoint      m_symbolPos;    // position of the placemark's symbol
    QPixmap     m_labelPixmap;  // the text label (most often name)
    QFont       m_labelFont;    // the text label font (most often name)
    QRect       m_labelRect;    // bounding box of label

    mutable QString     m_name;         // cached value
    mutable QSize       m_symbolSize;   // cached value
    mutable QPixmap     m_symbolPixmap; // cached value
};

#endif // VISIBLEPLACEMARK_H
