#ifndef MAINWINDOW_H
#define MAINWINDOW_H
 
#include <QtGui/QMainWindow>

#include "katlascontrol.h"

class QAction;
class QMenu;
 
class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent=0);

    private:
        void createActions();
        void createMenus();
        void createStatusBar();


        KAtlasControl *m_katlascontrol;

        QMenu *fileMenu;
        QMenu *helpMenu;

        QAction *exportMapAct;
        QAction *printAct;
        QAction *quitAct;
        QAction *copyMapAct;
        QAction *aboutMarbleAct;
        QAction *aboutQtAct;

    private slots:
        void exportMapScreenShot();
        void printMapScreenShot();
        void copyMap();
        void aboutMarble();
};
 
#endif
