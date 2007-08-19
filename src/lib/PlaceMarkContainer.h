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


#ifndef PLACEMARKCONTAINER_H
#define PLACEMARKCONTAINER_H


#include <QtCore/QVector>
#include <QtCore/QDebug>

#include "PlaceMark.h"


class PlaceMarkContainer : public QVector<PlaceMark*>
{
 public:
    PlaceMarkContainer();
    PlaceMarkContainer( const QString& name );

    void setName( const QString& name );
    QString name() const;

    void deleteAll();
    void clearTextPixmaps();
    void clearSelected();
    void sort();

 private:	
    QString  m_name;
};

#endif // PLACEMARKCONTAINER_H
