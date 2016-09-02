//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012   Mohammed Nafees   <nafees.technocool@gmail.com>
// Copyright 2012   Dennis Nienhüser  <nienhueser@kde.org>
// Copyright 2012   Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#ifndef POPUPLAYER_H
#define POPUPLAYER_H

#include "LayerInterface.h"

#include <QObject>
#include <QUrl>

class QSizeF;

namespace Marble
{

class GeoDataCoordinates;
class MarbleWidget;

/**
 * @brief The PopupLayer class
 *
 * A popup dialog opening on top of the map. The content is shown in a QWebView,
 * acting like a minimalistic web browser.
 *
 */
class MARBLE_EXPORT PopupLayer : public QObject, public LayerInterface
{
    Q_OBJECT
public:
    explicit PopupLayer( MarbleWidget *widget, QObject* parent = 0 );
    ~PopupLayer();

    QStringList renderPosition() const;
    bool render( GeoPainter *painter, ViewportParams *viewport,
                 const QString &, GeoSceneLayer * );
    virtual bool eventFilter( QObject *, QEvent * );
    qreal zValue() const;

    RenderState renderState() const;

    virtual QString runtimeTrace() const { return QStringLiteral("PopupLayer"); }

    /**
     * @brief Is popup item visible
     *
     * If popup item visible, it will return `true`,
     * otherwise - `false`
     *
     * @return visibility of the item
     */
    bool visible() const;

    /**
     * @brief Set visibility of the item
     *
     * If @p visible is `true`, popup will be visible,
     * otherwise - popup won't be visible.
     *
     * @param visible visibility of the item
     */
    void setVisible( bool visible );

    /**
     * @brief Make the dialog pop up
     *
     * This has the same effect as setVisible( true ) and additionally
     * adjusts the viewport so that the dialog is fully visible.
     */
    void popup();

    /**
     * @brief Sets coordinates
     *
     * Use a geo position as the dialog base position. The dialog will be shown if
     * it is visible and if the map viewport includes the given coordinates.
     * This invalidates a screen position set with setPosition(), if any.
     *
     * Alignment of the dialog from the point of view of the coordinates. For example,
     * Qt::AlignRight | Qt::AlignVCenter shows the dialog to the right of the geo position,
     * vertically centered. An arrow points from the dialog to the geo position.
     *
     * @param coordinates geo coordinates
     * @param alignment alignment of popup when it visible
     */
    void setCoordinates( const GeoDataCoordinates &coordinates, Qt::Alignment alignment );

    /**
     * @brief Sets URL of the browser
     *
     * @see PopupItem::setUrl();
     *
     * @param url url for web brower
     */
    void setUrl( const QUrl &url );

    /**
     * @brief Sets size of popup item
     *
     * Sets the @p size of the dialog (including the arrow, if any).
     *
     * @param size popup size, arrows in count
     */
    void setSize( const QSizeF &size );

    /**
     * @brief Sets content of the browser
     *
     * @see PopupItem::setContent();
     *
     * @param html content (in html format)
     */
    void setContent( const QString &html, const QUrl & baseUrl = QUrl() );

    /**
     * @brief Sets background color of the header
     *
     * @see PopupItem::setBackgroundColor();
     *
     * @param color color to set
     */
    void setBackgroundColor( const QColor &color );

    /**
     * @brief Sets text color of the header
     *
     * @see PopupItem::setTextColor();
     *
     * @param color color to set
     */
    void setTextColor( const QColor &color );

Q_SIGNALS:
    void repaintNeeded();

private Q_SLOTS:
    void hidePopupItem();

private:
    class Private;
    Private *const d;
};

}

#endif
