// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtDeclarative/QDeclarativeView>

class MainWindow : public QDeclarativeView
{
    Q_OBJECT

public:
    MainWindow( QWidget *parent = 0 );
    ~MainWindow();
};

#endif //MAINWINDOW_H