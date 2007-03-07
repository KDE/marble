//
// C++ Implementation: searchcombobox
//
// Description: 
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <QtCore/QDebug>
#include <QtGui/QLineEdit>
#include <QtGui/QAbstractItemView>
#include "searchcombobox.h"

#ifdef KDEBUILD
#include "searchcombobox.moc"
#endif

SearchComboBox::SearchComboBox(QWidget* parent):QComboBox(parent){
//	showPopup();
//	connect(this, SIGNAL(editTextChanged(QString)), SLOT(showListView(QString)));
}

void SearchComboBox::showListView(QString text){
//	qDebug("Ha");

	QAbstractItemView* listview = view();
	listview->show();
	listview->raise();

//	QLineEdit* lineedit = lineEdit();
//	if (text == lineedit->text()) return;
//	showPopup();

//	setFocusProxy(lineedit);
//	lineedit->setFocus();
//	lineedit->setFocusProxy(this);
//	setEditText(text);
	qDebug() << text;
// 4955

}
