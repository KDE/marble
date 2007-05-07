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

        KAction *m_copyMapAction;

    private slots:
        void copyMap();
};
 
#endif
