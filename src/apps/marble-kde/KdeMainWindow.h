// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2006-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
//


#ifndef MARBLE_KDEMAINWINDOW_H
#define MARBLE_KDEMAINWINDOW_H
 

#include <KXmlGuiWindow>

class QActionGroup;
class QAction;

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
                         QWidget *parent = nullptr );
    ~MainWindow() override;

    ControlView* marbleControl() const;
    MarbleWidget* marbleWidget() const;

 public Q_SLOTS:
    void updateWindowTitle();
    void changeViewSize( QAction* );
    void updateCenterFromTheme();

 protected:
    void closeEvent( QCloseEvent *event ) override;

 private:
    QSize m_savedSize;
    MarblePart *m_part;
    QActionGroup *m_viewSizeActsGroup;
};

}

#endif
