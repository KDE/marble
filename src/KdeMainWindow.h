#ifndef MAINWINDOW_H
#define MAINWINDOW_H
 
#include <KXmlGuiWindow>
#include "katlascontrol.h"
 
class MainWindow : public KXmlGuiWindow
{
    public:
        MainWindow(QWidget *parent=0);

    private:
        KAtlasControl *m_katlascontrol;
	void setupActions();

        void createStatusBar();
};
 
#endif
