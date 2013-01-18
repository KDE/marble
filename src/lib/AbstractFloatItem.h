//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
// Copyright 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#ifndef MARBLE_ABSTRACTFLOATITEM_H
#define MARBLE_ABSTRACTFLOATITEM_H

#include <QtCore/QPointF>
#include <QtCore/QSizeF>
#include <QtCore/QString>
#include <QtCore/Qt>

#include <QtGui/QPen>
#include <QtGui/QFont>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QHelpEvent>
#include <QtGui/QWidget>

#include "RenderPlugin.h"
#include "FrameGraphicsItem.h"
#include "marble_export.h"


class QMenu;

namespace Marble
{

class AbstractFloatItemPrivate;

/**
 * @brief The abstract class for float item plugins
 *
 * Float Item is a variant of Marble render plugins
 * It keeps floating on top of the map at a given screen position
 *
 * Good examples are Overview Map, License
 *
 */

class MARBLE_EXPORT AbstractFloatItem : public RenderPlugin, public FrameGraphicsItem
{
    Q_OBJECT

 public:
    explicit AbstractFloatItem( const MarbleModel *marbleModel,
                                const QPointF &point = QPointF( 10.0, 10.0 ),
                                const QSizeF &size = QSizeF( 150.0, 50.0 ) );
    virtual ~AbstractFloatItem();

    virtual QHash<QString,QVariant> settings() const;
    virtual void setSettings(const QHash<QString, QVariant> &settings);

    /**
     * @brief current pen for rendering
     * @return pen
     */
    QPen pen() const;

    /**
     * @brief setting current pen for rendering
     * @param pen
     */
    void setPen( const QPen &pen );

    /**
     * @brief current font for rendering
     * @return font
     */
    QFont font() const;

    /**
     * @brief setting current font for rendering
     * @param font
     */
    void setFont( const QFont &font );

    bool render( GeoPainter *painter, ViewportParams *viewport,
                 const QString& renderPos = "FLOAT_ITEM", GeoSceneLayer * layer = 0 );
    virtual QString renderPolicy() const;
    virtual QStringList renderPosition() const;

    /**
     * @brief Set visibility of the float item
     *
     * Float items can be visible or invisible.
     * It's possible to check visibility with @see visible
     *
     * @param visible visibility of the item
     */
    void setVisible( bool visible );

    /**
     * @brief Check visibility of the float item
     *
     * Float items can be visible or invisible.
     * It's possible to set visibility with @see setVisible
     *
     * @return visible or not
     */
    bool visible() const;

    /**
     * @brief Check is position locked
     *
     * Float Item position can be locked. If it is,
     * the item can't be moved with the cursor (in the UI)
     *
     * To set it use @see setPositionLocked
     *
     * @return position locked or not
     */
    bool positionLocked() const;

 public Q_SLOTS:
    /**
     * @brief Set is position locked
     * @param lock is locked?
     *
     * Float Item position can be locked. If it is,
     * item can't be moved with cursor (in UI)
     *
     * To check it use @see positionLocked
     *
     */
    void setPositionLocked( bool lock );

    /**
     * @brief Show the item
     *
     * If the item was hidden this function will show it
     *
     */
    void show();

    /**
     * @brief Hide the item
     *
     * If the item was shown this function will hide it
     *
     */
    void hide();

 protected:
    virtual bool eventFilter( QObject *object, QEvent *e );
    virtual void contextMenuEvent ( QWidget *w, QContextMenuEvent *e );
    virtual void toolTipEvent( QHelpEvent *e );
    virtual void changeViewport( ViewportParams *viewport );
    QMenu* contextMenu();

 private:
    Q_DISABLE_COPY( AbstractFloatItem )
    AbstractFloatItemPrivate * const d;
};

}

#endif
