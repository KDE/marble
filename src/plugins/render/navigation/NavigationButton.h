//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mohammed Nafees <nafees.technocool@gmail.com>
//

#ifndef NAVIGATIONBUTTON_H
#define NAVIGATIONBUTTON_H

#include <QAbstractButton>

namespace Marble
{

class NavigationButton : public QAbstractButton
{
    Q_OBJECT
public:
    explicit NavigationButton( QWidget *parent = nullptr );

Q_SIGNALS:
    void repaintNeeded();

protected:
    void mousePressEvent ( QMouseEvent *mouseEvent ) override;
    void mouseReleaseEvent ( QMouseEvent *mouseEvent ) override;
    void enterEvent( QEvent * e) override;
    void leaveEvent( QEvent * e) override;
    void changeEvent( QEvent *e ) override;
    void paintEvent( QPaintEvent * ) override;

private:
    QIcon::Mode m_iconMode;
};

}

#endif
