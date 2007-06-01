//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2005-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#include "searchcombobox.h"

#include <QtCore/QDebug>
#include <QtGui/QLineEdit>
#include <QtGui/QAbstractItemView>


SearchComboBox::SearchComboBox(QWidget* parent)
    : QComboBox( parent )
{
    // showPopup();
    // connect(this, SIGNAL(editTextChanged(QString)), SLOT(showListView(QString)));
}

void SearchComboBox::showListView(const QString& text)
{
    QAbstractItemView  *listview = view();
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


#ifndef Q_OS_MACX
#include "searchcombobox.moc"
#endif
