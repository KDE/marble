//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Illya Kovalevskyy   <illya.kovalevskyy@gmail.com>
//

#include <QMenu>
#include <QContextMenuEvent>
#include <QKeyEvent>
#include <QClipboard>
#include "MarbleWebView.h"

MarbleWebView::MarbleWebView(QWidget *parent) :
    QWebView(parent),
    m_contextMenu(new QMenu(this)),
    m_copyAction(new QAction(this))
{
    m_copyAction->setText(tr("Copy"));
    m_copyAction->setIcon(QIcon(":/icons/edit-copy.png"));
    m_copyAction->setIconVisibleInMenu(true);
    m_copyAction->setToolTip(tr("Copy selected content"));
    m_contextMenu->addAction(m_copyAction);
    setContextMenuPolicy(Qt::DefaultContextMenu);

#if !(QT_VERSION >= 0x040700 && QT_VERSION < 0x040800)
    QAction *hotCopyAction = pageAction(QWebPage::Copy);
    hotCopyAction->setShortcut(QKeySequence::Copy);
#endif
}

void MarbleWebView::contextMenuEvent(QContextMenuEvent *event)
{
    m_copyAction->setEnabled(!selectedHtml().isEmpty());
    m_contextMenu->exec(event->globalPos());
}

#if (QT_VERSION >= 0x040700 && QT_VERSION < 0x040800)
void MarbleWebView::keyPressEvent(QKeyEvent *event)
{
    if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_C) {
        const QString text = selectedText();
        if (!text.isEmpty()) {
            QClipboard *clipboard = qApp->clipboard();
            clipboard->setText(text);
        } else {
            QWebView::keyPressEvent(event);
        }
    }
}
#endif
