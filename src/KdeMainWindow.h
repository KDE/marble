//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//


#ifndef MARBLE_MAINWINDOW_H
#define MARBLE_MAINWINDOW_H
 

#include <KXmlGuiWindow>

namespace Marble
{

class ControlView;
class MarblePart;
class MarbleWidget;

class MainWindow : public KXmlGuiWindow
{
  Q_OBJECT

 public:
    explicit MainWindow( const QString& marbleDataPath = QString(), 
                         QWidget *parent = 0 );
    ~MainWindow();

    ControlView* marbleControl() const;
    MarbleWidget* marbleWidget() const;

 public slots:
    void setMapTitle();

 private:
    MarblePart *m_part;
};

}

#endif
