#ifndef MAINWINDOW_H
#define MAINWINDOW_H
 
#include <KMainWindow>
#include "katlascontrol.h"
 
class MainWindow : public KMainWindow
{
    public:
        MainWindow(QWidget *parent=0);

    private:
        KAtlasControl *m_katlascontrol;
	void setupActions();

        void createStatusBar();
};
 
#endif
