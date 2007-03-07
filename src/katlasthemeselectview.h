//
// C++ Interface: gpthemeselectview
//
// Description: PlaceMarkModel 

// KAtlasThemeSelectView lets the user choose a map theme
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution

#ifndef KATLASTHEMESELECTVIEW_H
#define KATLASTHEMESELECTVIEW_H

#include <QtCore/QDebug>
#include <QtGui/QStandardItemModel>
#include <QtGui/QListView>
/**
@author Torsten Rahn
*/

class KAtlasThemeSelectView : public QListView {

Q_OBJECT

public:
	KAtlasThemeSelectView(QWidget *parent = 0);
//	void setModel( QAbstractItemModel * model );

protected:
	void resizeEvent(QResizeEvent* event);

private slots:
	void selectedMapTheme( QModelIndex index ){ 
		const QAbstractItemModel* model = index.model();
		QModelIndex colindex = model->index( index.row(),2,QModelIndex());

		QString currentmaptheme = (model->data(colindex)).toString();
		emit selectMapTheme( currentmaptheme ); 
//		qDebug() << currentmaptheme;
	}

signals:
	void selectMapTheme( const QString& );
};

#endif // KATLASTHEMESELECTVIEW_H
