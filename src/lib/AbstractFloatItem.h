//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
//

#ifndef MARBLE_ABSTRACTFLOATITEM_H
#define MARBLE_ABSTRACTFLOATITEM_H

#include <QtCore/QPointF>
#include <QtCore/QSizeF>
#include <QtCore/QString>
#include <QtCore/Qt>

#include <QtGui/QPen>
#include <QtGui/QFont>

#include "RenderPlugin.h"
#include "FrameGraphicsItem.h"
#include "marble_export.h"


class QMenu;

namespace Marble
{

class AbstractFloatItemPrivate;

/**
 * @short The abstract class that creates an "empty" float item.
 *
 */

class MARBLE_EXPORT AbstractFloatItem : public RenderPlugin, public FrameGraphicsItem
{
    Q_OBJECT

 public:
    explicit AbstractFloatItem( const QPointF &point = QPointF( 10.0, 10.0 ),
                                const QSizeF &size = QSizeF( 150.0, 50.0 ) );
    virtual ~AbstractFloatItem();

    QPen pen() const;
    void setPen( const QPen &pen );

    QFont font() const;
    void setFont( const QFont &font );

    bool render( GeoPainter *painter, ViewportParams *viewport,
                 const QString& renderPos = "FLOAT_ITEM", GeoSceneLayer * layer = 0 );

    virtual bool renderOnMap( GeoPainter *painter, ViewportParams *viewport,
                              const QString& renderPos, GeoSceneLayer * layer = 0 );

    virtual QString renderPolicy() const;

    virtual QStringList renderPosition() const;

    void setVisible( bool visible );

    bool visible() const;

    bool positionLocked();

 public slots:
    void setPositionLocked( bool lock );
    void show();
    void hide();

 protected:
    virtual bool eventFilter( QObject *object, QEvent *e );
    QMenu* contextMenu();

 private:
    Q_DISABLE_COPY( AbstractFloatItem )
    AbstractFloatItemPrivate * const d;
};

}

#endif
