//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Mohammed Nafees <nafees.technocool@gmail.com>
//

#ifndef NAVIGATIONSLIDER_H
#define NAVIGATIONSLIDER_H

#include <QAbstractSlider>
#include <QPixmap>

namespace Marble
{

class NavigationSlider : public QAbstractSlider
{
    Q_OBJECT
public:
    explicit NavigationSlider( QWidget *parent = 0 );
    ~NavigationSlider();

Q_SIGNALS:
    void repaintNeeded();

protected:
    void enterEvent( QEvent * );
    void mouseMoveEvent( QMouseEvent * mouseEvent);
    void mousePressEvent( QMouseEvent * );
    void mouseReleaseEvent( QMouseEvent * );
    void leaveEvent( QEvent * );
    void paintEvent( QPaintEvent * );
    void repaint();

private:
    static QPixmap pixmap(const QString &id );
    QString m_handleImagePath;
};

}

#endif
