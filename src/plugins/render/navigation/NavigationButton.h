//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Mohammed Nafees <nafees.technocool@gmail.com>
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
    explicit NavigationButton( QWidget *parent = 0 );

Q_SIGNALS:
    void repaintNeeded();

protected:
    void mousePressEvent ( QMouseEvent *mouseEvent );
    void mouseReleaseEvent ( QMouseEvent *mouseEvent );
    void enterEvent( QEvent * e);
    void leaveEvent( QEvent * e);
    void changeEvent( QEvent *e );
    void paintEvent( QPaintEvent * );

private:
    QIcon::Mode m_iconMode;
};

}

#endif
