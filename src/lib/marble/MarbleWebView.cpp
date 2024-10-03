// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "MarbleWebView.h"
#include <QApplication>
#include <QClipboard>
#include <QContextMenuEvent>
#include <QKeyEvent>
#include <QMenu>

MarbleWebView::MarbleWebView(QWidget *parent)
    : QWebEngineView(parent)
    , m_contextMenu(new QMenu(this))
    , m_copyAction(new QAction(this))
{
    m_copyAction->setText(tr("Copy"));
    m_copyAction->setIcon(QIcon(QStringLiteral(":/icons/edit-copy.png")));
    m_copyAction->setIconVisibleInMenu(true);
    m_copyAction->setToolTip(tr("Copy selected content"));
    connect(m_copyAction, &QAction::triggered, this, &MarbleWebView::copySelectedText);
    m_contextMenu->addAction(m_copyAction);
    setContextMenuPolicy(Qt::DefaultContextMenu);
}

void MarbleWebView::contextMenuEvent(QContextMenuEvent *)
{
    m_copyAction->setEnabled(!selectedText().isEmpty());
    m_contextMenu->exec(QCursor::pos());
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
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_C) {
        copySelectedText();
        return;
    }

    QWebEngineView::keyPressEvent(event);
}

#include "moc_MarbleWebView.cpp"
