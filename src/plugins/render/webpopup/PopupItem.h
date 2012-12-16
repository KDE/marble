//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012   Mohammed Nafees   <nafees.technocool@gmail.com>
//

#ifndef POPUPITEM_H
#define POPUPITEM_H

#include <QtCore/QObject>
#include <QtCore/QUrl>

#include "BillboardGraphicsItem.h"

class QWebView;
class QPainter;

namespace Marble
{

class PopupItem : public QObject, public BillboardGraphicsItem
{
    Q_OBJECT
public:
    explicit PopupItem( QObject* parent = 0 );
    ~PopupItem();

    void setUrl( const QUrl &url );
    void setContent( const QString &html );
    QPoint transform( const QPoint &point ) const;
    virtual bool eventFilter( QObject *, QEvent *e );

protected:
    void paint( QPainter *painter );

Q_SIGNALS:
    void dirty();

private:
    QWebView *m_webView;

    QString m_content;

    bool m_needMouseRelease;
};

}

#endif
