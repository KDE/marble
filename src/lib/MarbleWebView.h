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
    Q_OBJECT
public:
    MarbleWebView(QWidget *parent = 0);

protected:
    void contextMenuEvent(QContextMenuEvent *event);
    void keyPressEvent(QKeyEvent *event);

private slots:
    void copySelectedText();

private:
    QMenu *m_contextMenu;
    QAction *m_copyAction;
};

#endif // MARBLEWEBVIEW_H
