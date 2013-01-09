//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Illya Kovalevskyy   <illya.kovalevskyy@gmail.com>
//

#ifndef MARBLEWEBVIEW_H
#define MARBLEWEBVIEW_H

#include <QtWebKit/QWebView>

class MarbleWebView : public QWebView
{
public:
    MarbleWebView(QWidget *parent = 0);

protected:
    void contextMenuEvent(QContextMenuEvent *event);

#if (QT_VERSION >= 0x040700 && QT_VERSION < 0x040800)
    void keyPressEvent(QKeyEvent *event);
#endif

private:
    QMenu *m_contextMenu;
    QAction *m_copyAction;
};

#endif // MARBLEWEBVIEW_H
