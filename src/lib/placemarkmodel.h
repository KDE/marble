//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

//
// PlaceMarkModel resembles the model for Placemarks
//


#ifndef PLACEMARKMODEL_H
#define PLACEMARKMODEL_H


#include <QtCore/QAbstractListModel>

#include "marble_export.h"
#include "placecontainer.h"


inline bool nameSort( PlaceMark* mark1, PlaceMark* mark2 )
{
    return mark1->name() < mark2->name();
}


class MARBLE_EXPORT PlaceMarkModel : public QAbstractListModel
{
    Q_OBJECT

 public:
    PlaceMarkModel(QObject *parent = 0);
    ~PlaceMarkModel();
	
    int  rowCount(const QModelIndex &parent = QModelIndex()) const;
    int  columnCount(const QModelIndex &parent = QModelIndex()) const;

    PlaceMark* placeMark(const QModelIndex &index) const;

    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;  

    virtual QModelIndexList match ( const QModelIndex & start, int role, 
                                    const QVariant & value, int hits = 1,
                                    Qt::MatchFlags flags = Qt::MatchFlags( Qt::MatchStartsWith | Qt::MatchWrap ) ) const;

    void setContainer( PlaceContainer* );

 private:
    QVector<PlaceMark*> m_placemarkindex;
};


#endif // PLACEMARKMODEL_H
