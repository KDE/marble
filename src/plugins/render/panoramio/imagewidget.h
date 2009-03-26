//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Shashank Singh <shashank.personal@gmail.com>"
// C++ Interface: imagewidget
//
#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QWidget>
#include <QDebug>
/**
This class implements a Widget that shows image in thumbnail format and then it will also implement sutiable events so that if clicked it scales the image to show it in it's original size

	@author Shashank Singh <shashank.singh@geodesic.com>
*/
class imageWidget : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(imageWidget) //i am trying to save this class from deepcopy
public:
    imageWidget();

    ~imageWidget();

    void addImage(QPixmap);
    QPixmap* returnPointerToImage();
protected:
    void paintEvent(QPaintEvent * event );
    void mousePressEvent ( QMouseEvent * event ) ;
private:
    QString imageLink;
    QPixmap image;


};

#endif
