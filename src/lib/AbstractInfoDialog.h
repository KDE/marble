//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_ABSTRACTINFODIALOG_H
#define MARBLE_ABSTRACTINFODIALOG_H

#include "GeoDataCoordinates.h"
#include "marble_export.h"

#include <QtCore/QSizeF>
#include <QtCore/QPointF>
#include <QtCore/QUrl>
#include <QtCore/QString>

namespace Marble {

/**
 * @brief Interface for popup dialogs
 *
 * Popup dialogs can appear either fixed to a geo position (setCoordinates) or
 * fixed to a screen position (setPosition).
 *
 * Currently they are meant to be used in conjuntion with RenderPlugin to control
 * their rendering and visibility.
 *
 * @todo Refactor plugin interface and strip functionality from plugin loading.
 * It makes little sense to tie these dialogs to a plugin
 */
class MARBLE_EXPORT AbstractInfoDialog
{
public:
    virtual ~AbstractInfoDialog();

    /**
     * @brief Change the size (pixel units) of the dialog
     * @param size New dialog size
     */
    virtual void setSize( const QSizeF &size ) = 0;

    /**
     * @brief setPosition Stick the dialog to the given screen position
     * @param position Top-left corner of the dialog in screen coordinates
     */
    virtual void setPosition( const QPointF &position ) = 0;

    /**
     * @brief setCoordinates Stick the dialog to the given geo position
     * @param coordinates Geo position to stick the dialog to
     * @param alignment Dialog alignment with regard to the geo position
     * from the point of view of the observer (camera). Qt::AlignCenter for
     * example places the dialog center on top of the geo position, while
     * Qt::AlignRight | Qt::AlignVCenter would make it float to the right
     * of the geo position
     */
    virtual void setCoordinates( const GeoDataCoordinates &coordinates, Qt::Alignment alignment ) = 0;

    /**
     * @brief Change the content to display by loading from an URL
     * @param url URL containing the content to display
     */
    virtual void setUrl( const QUrl &url ) = 0;

    /**
     * @brief Change the content to display by passing the content directly
     * @param html Content to display
     */
    virtual void setContent( const QString &html ) = 0;
};

}

#endif
