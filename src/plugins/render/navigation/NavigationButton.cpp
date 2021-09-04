// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "NavigationButton.h"

#include <QPainter>
#include <QMouseEvent>

namespace Marble
{

NavigationButton::NavigationButton( QWidget *parent )
    : QAbstractButton( parent ),
      m_iconMode( QIcon::Normal )
{
    // nothing to do
}

void NavigationButton::mousePressEvent ( QMouseEvent *mouseEvent )
{
    if ( isEnabled() ) {
        if ( mouseEvent->button() == Qt::LeftButton ) {
            m_iconMode = QIcon::Selected;
        }
    }
    emit repaintNeeded();
}

void NavigationButton::mouseReleaseEvent ( QMouseEvent * )
{
    if ( isEnabled() ) {
        m_iconMode = QIcon::Active;
        emit clicked();
    }
    emit repaintNeeded();
}

void NavigationButton::enterEvent(QEvent *)
{
    if ( isEnabled() ) {
        m_iconMode = QIcon::Active;
    }
    emit repaintNeeded();
}

void NavigationButton::leaveEvent( QEvent * )
{
    if ( isEnabled() ) {
        m_iconMode = QIcon::Normal;
    }
    emit repaintNeeded();
}

void NavigationButton::changeEvent( QEvent *e )
{
    if ( e->type() == QEvent::EnabledChange ) {
        m_iconMode = isEnabled() ? QIcon::Normal : QIcon::Disabled;
    }
    emit repaintNeeded();
}

void NavigationButton::paintEvent( QPaintEvent * )
{
    QPainter painter( this );
    painter.drawPixmap( 0, 0, icon().pixmap( iconSize(), m_iconMode ) );
}

}

#include "moc_NavigationButton.cpp"
