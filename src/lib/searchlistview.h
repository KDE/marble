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


#ifndef SEARCHLISTVIEW_H
#define SEARCHLISTVIEW_H


#include <QtGui/QListView>

#include "marble_export.h"

class MARBLE_EXPORT SearchListView : public QListView
{
    Q_OBJECT

 public:
    SearchListView(QWidget*);

 signals:
    void centerOn(const QModelIndex&);

 public slots:
    void  selectItem(const QString&);
    void  activate() {
        if ( selectedIndexes().size() > 0 )
            emit activated( currentIndex() );
    }
};

#endif // SEARCHLISTVIEW_H
