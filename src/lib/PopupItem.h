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

#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtGui/QLabel>

#include "BillboardGraphicsItem.h"

class QWebView;
class QPainter;
class QPushButton;

namespace Marble
{

class PopupItem : public QObject, public BillboardGraphicsItem
{
    Q_OBJECT
public:
    explicit PopupItem( QObject* parent = 0 );
    ~PopupItem();

    bool isPrintButtonVisible() const;
    void setPrintButtonVisible(bool);

    void setUrl( const QUrl &url );
    void setContent( const QString &html );
    void setTextColor( const QColor &color );
    void setBackgroundColor( const QColor &color );
    QWidget* transform( QPoint &point ) const;
    void clearHistory();
    virtual bool eventFilter( QObject *, QEvent *e );

private slots:
    void printContent();
    void updateBackButton();
    void goBack();

protected:
    void paint( QPainter *painter );

Q_SIGNALS:
    void dirty();
    void hide();

private:
    QPixmap pixmap( const QString &imageid );
    void colorize( QImage &img, const QColor &col );

    QWidget *m_widget;
    QPushButton *m_printButton;
    QPushButton *m_goBackButton;
    QLabel *m_titleText;
    QWebView *m_webView;
    QString m_content;
    QColor m_textColor;
    QColor m_backColor;
    bool m_needMouseRelease;
};

}

#endif
