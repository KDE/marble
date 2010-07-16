//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin  <ps_ml@gmx.de>
//

#ifndef MARBLE_GEORENDERERVIEW_H
#define MARBLE_GEORENDERERVIEW_H

// Marble

// Qt
#include <QtCore/QModelIndex>
#include <QtCore/QRect>
#include <QtGui/QRegion>
#include <QtGui/QAbstractItemView>
#include <QtGui/QItemSelectionModel>
#include <QtGui/QPen>


class QItemSelection;

namespace Marble
{
class GeoDataDocument;
class GeoDataGeometry;
class GeoPainter;

class GeoRendererView : public QAbstractItemView
{
 public:
    GeoRendererView( QWidget * parent = 0 );
    
    QRect visualRect( const QModelIndex &index ) const;
    void scrollTo( const QModelIndex &index, ScrollHint hint = EnsureVisible );
    QModelIndex indexAt( const QPoint &point ) const;
    
    void setGeoPainter( GeoPainter* painter );
 protected:
    QModelIndex moveCursor( QAbstractItemView::CursorAction cursorAction,
                        Qt::KeyboardModifiers modifiers );

    int horizontalOffset() const { return 0; };
    int verticalOffset() const { return 0; };

    bool isIndexHidden( const QModelIndex &index ) const;

    void setSelection( const QRect&, QItemSelectionModel::SelectionFlags command );

    void paintEvent( QPaintEvent *event );
    
    void renderIndex( QModelIndex &index );

    QRegion visualRegionForSelection( const QItemSelection &selection ) const;

 private:
    GeoPainter *m_painter;
    GeoDataDocument *m_root;
    QString m_styleUrl;

    void setBrushStyle( QString styleId );
    void setPenStyle( QString styleId );
    bool renderGeoDataGeometry( GeoDataGeometry *geometry, QString styleUrl );

    QBrush m_currentBrush;
    QPen m_currentPen;

};

} // namespace Marble
#endif // MARBLE_GEORENDERERVIEW_H
