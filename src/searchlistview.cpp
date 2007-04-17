//
// C++ Implementation: searchlistview
//
// Description: 
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <QtCore/QDebug>
#include <QtGui/QStandardItemModel>
#include "searchlistview.h"

SearchListView::SearchListView(QWidget* parent):QListView(parent){
	connect(this, SIGNAL(activated(const QModelIndex&)), this, SIGNAL(centerOn(const QModelIndex&)));
}

void SearchListView::selectItem(QString text){
	if (text == ""){
		clearSelection();
		scrollToTop();
		emit activated(QModelIndex());
		return;
	}

	QModelIndexList resultlist;

	resultlist = model()->match(model()->index(0,0),Qt::DisplayRole,text,1,Qt::MatchStartsWith);

	if (resultlist.size() > 0){
        scrollToBottom(); 
		setCurrentIndex(resultlist[0]);
        scrollTo(resultlist[0]);
	}
	else qDebug("noitem!");
}

#ifndef Q_OS_MACX
#include "searchlistview.moc"
#endif
