// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "NavigationButton.h"

#include <QMouseEvent>
#include <QPainter>

namespace Marble
{

NavigationButton::NavigationButton(QWidget *parent)
    : QAbstractButton(parent)
    , m_iconMode(QIcon::Normal)
{
    // nothing to do
}

void NavigationButton::mousePressEvent(QMouseEvent *mouseEvent)
{
    if (isEnabled()) {
        if (mouseEvent->button() == Qt::LeftButton) {
            m_iconMode = QIcon::Selected;
        }
    }
    Q_EMIT repaintNeeded();
}

void NavigationButton::mouseReleaseEvent(QMouseEvent *)
{
    if (isEnabled()) {
        m_iconMode = QIcon::Active;
        Q_EMIT clicked();
    }
    Q_EMIT repaintNeeded();
}

void NavigationButton::enterEvent(QEnterEvent *)
{
    if (isEnabled()) {
        m_iconMode = QIcon::Active;
    }
    Q_EMIT repaintNeeded();
}

void NavigationButton::leaveEvent(QEvent *)
{
    if (isEnabled()) {
        m_iconMode = QIcon::Normal;
    }
    Q_EMIT repaintNeeded();
}

void NavigationButton::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::EnabledChange) {
        m_iconMode = isEnabled() ? QIcon::Normal : QIcon::Disabled;
    }
    Q_EMIT repaintNeeded();
}

void NavigationButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, icon().pixmap(iconSize(), m_iconMode));
}

}

#include "moc_NavigationButton.cpp"
