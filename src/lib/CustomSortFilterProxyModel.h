//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Claudiu Covaci <claudiu.covaci@gmail.com>"
//

#ifndef CUSTOMSORTFILTERPROXYMODEL_H
#define CUSTOMSORTFILTERPROXYMODEL_H

#include <QtGui/QSortFilterProxyModel>

class CustomSortFilterProxyModel : public QSortFilterProxyModel {
     Q_OBJECT

    public:
    CustomSortFilterProxyModel(QObject *parent = 0);
    protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

};

#endif //CUSTOMSORTFILTERPROXYMODEL
