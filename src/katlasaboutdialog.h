//
// C++ Interface: katlasaboutdialog
//
// Description: KAtlasAboutDialog 

// The about dialog 
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution

#ifndef KATLASABOUTDIALOG_H
#define KATLASABOUTDIALOG_H

#include "ui_katlasaboutdialog.h"

/**
@author Torsten Rahn
*/


class KAtlasAboutDialog : public QDialog, private Ui::katlasAboutDialog {

Q_OBJECT

public:
	KAtlasAboutDialog(QWidget *parent = 0);
};

#endif // KATLASABOUTDIALOG_H
