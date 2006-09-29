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
#include <QDebug>
#include <QStandardItemModel>
#include "searchlistview.h"

SearchListView::SearchListView(QWidget* parent):QListView(parent){
	connect(this, SIGNAL(activated(const QModelIndex&)), this, SIGNAL(centerOn(const QModelIndex&)));
}

void SearchListView::selectItem(QString text){

	QModelIndexList resultlist;
	resultlist = model()->match(model()->index(0,0),Qt::EditRole,text,1,Qt::MatchStartsWith);

	if (resultlist.size() > 0){ 
		setCurrentIndex(resultlist[0]);
		qDebug() << "Model entry: " << text << " " << ( resultlist[0].data(Qt::EditRole) ).toString();
	}
	else qDebug("noitem!");
}
