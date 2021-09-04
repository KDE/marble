// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_LABELGRAPHICSITEM_H
#define MARBLE_LABELGRAPHICSITEM_H

// Marble
#include "FrameGraphicsItem.h"
#include "marble_export.h"

#include <QSize>

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
    explicit LabelGraphicsItem( MarbleGraphicsItem *parent = nullptr );
    ~LabelGraphicsItem() override;

    void setContentSize( const QSizeF &contentSize ) override;

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
    void paintContent( QPainter *painter ) override;

 private:
    Q_DISABLE_COPY( LabelGraphicsItem )
    Q_DECLARE_PRIVATE(LabelGraphicsItem)
};

} // namespace Marble

#endif
