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
    ~NavigationSlider() override;

Q_SIGNALS:
    void repaintNeeded();

protected:
    void enterEvent( QEvent * ) override;
    void mouseMoveEvent( QMouseEvent * mouseEvent) override;
    void mousePressEvent( QMouseEvent * ) override;
    void mouseReleaseEvent( QMouseEvent * ) override;
    void leaveEvent( QEvent * ) override;
    void paintEvent( QPaintEvent * ) override;
    void repaint();

private:
    static QPixmap pixmap(const QString &id );
    QString m_handleImagePath;
};

}

#endif
