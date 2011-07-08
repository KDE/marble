// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>

#include "MainWindow.h"

MainWindow::MainWindow( QWidget *parent ) : QDeclarativeView(parent)
{
}

MainWindow::~MainWindow()
{
}

#include "MainWindow.moc"