//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_LABELGRAPHICSITEM_H
#define MARBLE_LABELGRAPHICSITEM_H

// Marble
#include "FrameGraphicsItem.h"
#include "marble_export.h"

class QImage;
class QIcon;

namespace Marble
{

class LabelGraphicsItemPrivate;

/**
 * A label item provides an Item that displays text or images/pixmaps.
 * The text is displayed as plain text.
 * The item also provides frames.
 */
class MARBLE_EXPORT LabelGraphicsItem : public FrameGraphicsItem
{
 public:
    explicit LabelGraphicsItem( MarbleGraphicsItem *parent = 0 );
    ~LabelGraphicsItem();

    void setContentSize( const QSizeF &contentSize );

    QString text() const;
    void setText( const QString& text );

    QImage image() const;
    void setImage( const QImage& image, const QSize& size = QSize() );

    QIcon icon() const;
    void setIcon( const QIcon& icon, const QSize& size );

    void setMinimumSize( const QSizeF& size );
    QSizeF minimumSize() const;

    void clear();

 protected:
    void paintContent( QPainter *painter );

 private:
    Q_DISABLE_COPY( LabelGraphicsItem )

    LabelGraphicsItemPrivate * const d;
};

} // namespace Marble

#endif
