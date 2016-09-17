//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012   Torsten Rahn      <tackat@kde.org>
// Copyright 2012   Mohammed Nafees   <nafees.technocool@gmail.com>
// Copyright 2012   Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#ifndef POPUPITEM_H
#define POPUPITEM_H

#include <QObject>
#include <QUrl>

#include "BillboardGraphicsItem.h"

#ifdef MARBLE_NO_WEBKITWIDGETS
#include "ui_NullWebPopupWidget.h"
#else
#include "ui_WebPopupWidget.h"
#endif

class QPainter;

namespace Marble
{

/**
 * @brief The PopupItem Class
 *
 * This class represents graphics item for information bubble.
 * Mostly used by @see MapInfoDialog.
 *
 * It has nice API for QWebView and methods for styling it.
 *
 */
class PopupItem : public QObject, public BillboardGraphicsItem
{
    Q_OBJECT
public:
    explicit PopupItem( QObject* parent = 0 );
    ~PopupItem();

    /**
     * @brief Print button visibility indicator
     *
     * There is a button in the header of item with print icon.
     * It used to print the content of QWebView inside.
     * This method indicates visibility of this button.
     *
     * @see setPrintButtonVisible();
     *
     * @return visibility of the print button
     */
    bool isPrintButtonVisible() const;

    /**
     * @brief Sets visibility of the print button
     *
     * There is a button in the header of item with print icon.
     * It used to print the content of QWebView inside
     *
     * This method sets visibility of this button.
     *
     * If @p display is `true`, button will be displayed,
     * otherwise - button won't be displayed
     *
     * @param display visibility of the print button
     */
    void setPrintButtonVisible(bool display);

    /**
     * @brief Set URL for web window
     *
     * There is a small web browser inside.
     * It can show open websites.
     *
     * This method sets @p url for its window.
     *
     * @param url new url for web window
     */
    void setUrl( const QUrl &url );

    /**
     * @brief Set content of the popup
     *
     * There is a small web browser inside. It can show custom HTML.
     * This method sets custom @p html for its window
     *
     * @param html custom html for popup
     */
    void setContent( const QString &html, const QUrl & baseUrl = QUrl() );

    /**
     * @brief Sets text color of the header
     *
     * Frame of the web browser is called bubble. Bubble has
     * a header - part of the bubble at the top. Usually
     * it contains the name of the page which can be set via
     * TITLE html tag in HTML document loaded.
     * This method sets text @p color of the header.
     *
     * @param color text color of the header
     */
    void setTextColor( const QColor &color );

    /**
     * @brief Sets background color of the bubble
     *
     * Frame of the web browser is called bubble. This method
     * sets background @p color of this bubble.
     *
     * @param color background color of the bubble
     */
    void setBackgroundColor( const QColor &color );

    virtual bool eventFilter( QObject *, QEvent *e );

    void clearHistory();

private Q_SLOTS:
    /**
     * @brief Marks cache as dirty and tells the world its need for repainting.
     */
    void requestUpdate();

    /**
     * @brief Print content of the web browser
     *
     * Popup Item has built-in mini-browser. This function
     * executes print dialog for printing its content.
     *
     */
    void printContent() const;

    /**
     * @brief Updates Back Button (web surfing history)
     *
     * When you are browsing the site you may need to visit
     * the page, you have visited before (Go Back).
     *
     * For this action Popup Item has a button Go Back placed
     * in the left of the header.
     *
     * @note it's visible only if web surfing history is not clear or
     * you are not on its first page.
     *
     * @see goBack();
     *
     */
    void updateBackButton();

    /**
     * @brief Go Back (web surfing history)
     *
     * This method moves you one step backwards in
     * web surfing history.
     *
     */
    void goBack();

    /**
     * @brief Opens clicked URL in external browser.
     * @param url URL to be opened in external browser
     */
    void openUrl(const QUrl &url);

protected:
    void paint( QPainter *painter );

Q_SIGNALS:
    void repaintNeeded();
    void hide();

private:
    QPixmap pixmap( const QString &imageid ) const;
    static void colorize( QImage &img, const QColor &col );
    QWidget* transform( QPoint &point ) const;

    QWidget *m_widget;
    Ui::WebPopupWidget m_ui;
    QString m_content;
    QColor m_textColor;
    QColor m_backColor;
    bool m_needMouseRelease;
    QUrl m_baseUrl;
};

}

#endif
