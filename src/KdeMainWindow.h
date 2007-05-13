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
    void  setupActions();
    void  createStatusBar();

 private slots:
    void  exportMapScreenShot();
    void  printMapScreenShot();
    void  copyMap();

 private:
    // All the functionality is provided by this widget.
    KAtlasControl  *m_katlascontrol;

    // Actions for the GUI.
    KAction        *m_exportMapAction;
    KAction        *m_printMapAction;
    KAction        *m_copyMapAction;
};

 
#endif
