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

class VisiblePlaceMark
{
 public:
    VisiblePlaceMark();
    ~VisiblePlaceMark();

    const QPersistentModelIndex& modelIndex() const { return m_modelIndex; }
    void setModelIndex( const QPersistentModelIndex &modelIndex ) { m_modelIndex = modelIndex; }

    const QString name() const
    {
        if ( m_name.isEmpty() )
            m_name = m_modelIndex.data( Qt::DisplayRole ).toString();

        return m_name;
    }
    const QPixmap& symbolPixmap() const
    {
        if ( m_symbolPixmap.isNull() )
            m_symbolPixmap = m_modelIndex.data( Qt::DecorationRole ).value<QPixmap>();
        return  m_symbolPixmap;
    }
    const QSize& symbolSize() const
    {
        if ( !m_symbolSize.isValid() )
          m_symbolSize = m_modelIndex.data( PlaceMarkModel::SymbolSizeRole ).toSize();
        return m_symbolSize;
    }

    const QPoint& symbolPos() const { return m_symbolPos; }
    void setSymbolPos( const QPoint& sympos ) { m_symbolPos = sympos; }

    const QPixmap& labelPixmap() const { return m_labelPixmap; }
    void setLabelPixmap( const QPixmap& labelPixmap ) { m_labelPixmap = labelPixmap; }

    const QFont& labelFont() const { return m_labelFont; }
    void setLabelFont( const QFont& labelFont ) { m_labelFont = labelFont; }

    const QRect& labelRect() const { return m_labelRect; }
    void setLabelRect( const QRect& labelRect ) { m_labelRect = labelRect; }

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
