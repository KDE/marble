//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Mohammed Nafees <nafees.technocool@gmail.com>
//

#ifndef ARROWDISCWIDGET_H
#define ARROWDISCWIDGET_H

#include <QtGui/QWidget>
#include <QtGui/QPixmap>
#include <QtCore/QTimer>

namespace Marble
{

class MarbleWidget;

class ArrowDiscWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ArrowDiscWidget( QWidget *parent = 0 );
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
    void leaveEvent( QEvent *event );
    void repaint();

private slots:
    void startPressRepeat();
    void repeatPress();

private:
    Qt::ArrowType arrowUnderMouse( const QPoint &position ) const;
    QTimer m_initialPressTimer;
    QTimer m_repeatPressTimer;
    Qt::ArrowType m_arrowPressed;
    int m_repetitions;

    MarbleWidget *m_marbleWidget;
    QString m_imagePath;
};

}

#endif
