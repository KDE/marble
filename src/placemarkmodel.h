//
// C++ Interface: gpmapscale
//
// Description: PlaceMarkModel 

// PlaceMarkMode resembles the model for Placemarks
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution

#ifndef PLACEMARKMODEL_H
#define PLACEMARKMODEL_H

#include <QAbstractListModel>
#include "placecontainer.h"

/**
@author Torsten Rahn
*/

inline bool nameSort( PlaceMark* mark1, PlaceMark* mark2 ){ return mark1->name() < mark2->name(); }

class PlaceMarkModel : public QAbstractListModel {

Q_OBJECT

public:
	PlaceMarkModel(QObject *parent = 0);
	~PlaceMarkModel();
	
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;

	PlaceMark* placeMark(const QModelIndex &index) const;

	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation,

	int role = Qt::DisplayRole) const;	
	void setContainer( PlaceContainer* );
private:
	QVector<PlaceMark*> m_placemarkindex;
};

#endif // PLACEMARKMODEL_H
