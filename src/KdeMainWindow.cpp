#include "KdeMainWindow.h"
 
MainWindow::MainWindow(QWidget *parent) : KMainWindow(parent)
{
    m_katlascontrol = new KAtlasControl(0);
    setCentralWidget(m_katlascontrol);
    setupGUI();
}