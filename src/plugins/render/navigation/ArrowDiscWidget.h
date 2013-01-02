//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Mohammed Nafees <nafees.technocool@gmail.com>
//

#ifndef ARROWDISCWIDGET_H
#define ARROWDISCWIDGET_H

#include <QtGui/QWidget>
#include <QtGui/QPixmap>

namespace Marble
{

class MarbleWidget;

class ArrowDiscWidget : public QWidget
{
    Q_OBJECT
public:
    ArrowDiscWidget( QWidget *parent = 0 );
    ~ArrowDiscWidget();

    QPixmap pixmap(const QString &id );
    void setMarbleWidget( MarbleWidget *marbleWidget );

Q_SIGNALS:
    void repaintNeeded();

protected:
    void paintEvent( QPaintEvent * );
    void mouseMoveEvent( QMouseEvent *mouseEvent );
    void mousePressEvent( QMouseEvent *mouseEvent );
    void mouseReleaseEvent( QMouseEvent *mouseEvent );
    void repaint();

private:
    Qt::ArrowType arrowUnderMouse( const QPoint &position ) const;

    MarbleWidget *m_marbleWidget;
    QString m_imagePath;
};

}

#endif
