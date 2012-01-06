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
// VisiblePlacemarks are those Placemarks which become visible on the map
//

#ifndef MARBLE_VISIBLEPLACEMARK_H
#define MARBLE_VISIBLEPLACEMARK_H

#include "GeoDataPlacemark.h"

#include <QtGui/QPixmap>
#include <QtCore/QPoint>
#include <QtCore/QRect>
#include <QtCore/QString>

namespace Marble
{

/**
 * @short A class which represents the visible place marks on a map.
 *
 * This class is used by PlacemarkLayout to pass the visible place marks
 * to the PlacemarkPainter.
 */
class VisiblePlacemark
{
 public:
    VisiblePlacemark( const GeoDataPlacemark *placemark );

    /**
     * Returns the index of the place mark model which
     * is associated with this visible place mark.
     */
    const GeoDataPlacemark* placemark() const;

    /**
     * Returns the name of the place mark.
     */
    const QString name() const;

    /**
     * Returns the pixmap of the place mark symbol.
     */
    const QPixmap& symbolPixmap() const;

    /**
     * Returns the state of the place mark.
     */
    bool selected() const;

    /**
     * Sets the state of the place mark.
     */
    void setSelected( bool selected );

    /**
     * Returns the position of the place mark symbol on the map.
     */
    const QPoint& symbolPosition() const;

    /**
     * Sets the @p position of the place mark symbol on the map.
     */
    void setSymbolPosition( const QPoint& position );

    /**
     * Returns the pixmap of the place mark name label.
     */
    const QPixmap& labelPixmap() const;

    /**
     * Sets the @p pixmap of the place mark name label.
     */
    void setLabelPixmap( const QPixmap& pixmap );

    /**
     * Returns the area covered by the place mark name label on the map.
     */
    const QRect& labelRect() const;

    /**
     * Sets the @p area covered by the place mark name label on the map.
     */
    void setLabelRect( const QRect& area );

 private:
    const GeoDataPlacemark *m_placemark;

    // View stuff
    QPoint      m_symbolPosition; // position of the placemark's symbol
    bool        m_selected;       // state of the placemark
    QPixmap     m_labelPixmap;    // the text label (most often name)
    QRect       m_labelRect;      // bounding box of label

    mutable QString     m_name;         // cached value
    mutable QPixmap     m_symbolPixmap; // cached value
};

}

#endif
