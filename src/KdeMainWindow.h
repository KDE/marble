#ifndef MAINWINDOW_H
#define MAINWINDOW_H
 
#include <KXmlGuiWindow>

#include <KAction>
#include "katlascontrol.h"
 
class MainWindow : public KXmlGuiWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent=0);

    private:
        KAtlasControl *m_katlascontrol;
	void setupActions();

        void createStatusBar();

        KAction *m_exportMapAction;
        KAction *m_printMapAction;
        KAction *m_copyMapAction;

    private slots:
        void exportMapScreenShot();
        void printMapScreenShot();
        void copyMap();
};
 
#endif
