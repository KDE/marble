//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Illya Kovalevskyy   <illya.kovalevskyy@gmail.com>
//

#include <QApplication>
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
    connect(m_copyAction, SIGNAL(triggered()),
            this, SLOT(copySelectedText()));
    m_contextMenu->addAction(m_copyAction);
    setContextMenuPolicy(Qt::DefaultContextMenu);
}

void MarbleWebView::contextMenuEvent(QContextMenuEvent *event)
{
    m_copyAction->setEnabled(!selectedText().isEmpty());
    m_contextMenu->exec(event->globalPos());
}

void MarbleWebView::copySelectedText()
{
    const QString text = selectedText();
    if (!text.isEmpty()) {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(text);
    }
}

void MarbleWebView::keyPressEvent(QKeyEvent *event)
{
    if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_C) {
        copySelectedText();
        return;
    }

    QWebView::keyPressEvent(event);
}

#include "MarbleWebView.moc"
