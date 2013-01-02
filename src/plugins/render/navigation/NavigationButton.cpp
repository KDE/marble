//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "NavigationButton.h"

#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>

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
    if ( mouseEvent->button() == Qt::LeftButton ) {
        m_iconMode = QIcon::Selected;
        emit repaintNeeded();
    }
}

void NavigationButton::mouseReleaseEvent ( QMouseEvent * )
{
    m_iconMode = QIcon::Normal;
    emit repaintNeeded();
    emit clicked();
}

void NavigationButton::enterEvent(QEvent *)
{
    m_iconMode = QIcon::Active;
    emit repaintNeeded();
}

void NavigationButton::leaveEvent( QEvent * )
{
    m_iconMode = QIcon::Normal;
    emit repaintNeeded();
}

void NavigationButton::paintEvent( QPaintEvent * )
{
    QPainter painter( this );
    painter.drawPixmap( 0, 0, icon().pixmap( iconSize(), m_iconMode ) );
}

}

#include "NavigationButton.moc"
