//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef LABELGRAPHICSITEM_H
#define LABELGRAPHICSITEM_H

// Marble
#include "FrameGraphicsItem.h"
#include "marble_export.h"

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
    LabelGraphicsItem( MarbleGraphicsItem *parent = 0 );

    void setText( const QString& text );
    void clear();

 protected:
    void paintContent( GeoPainter *painter, ViewportParams *viewport,
                       const QString& renderPos, GeoSceneLayer * layer );

 private:
    Q_DISABLE_COPY( LabelGraphicsItem )

    LabelGraphicsItemPrivate * const d;
};

} // namespace Marble

#endif // LABELGRAPHICSITEM_H
