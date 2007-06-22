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

#include "placemark.h"


inline bool populationLessThan( PlaceMark* mark1, PlaceMark* mark2 )
{ 

    return  ( mark1->selected() == mark2->selected()
              ? mark1->population() > mark2->population() 
              : mark1->selected() == 1 ? true : false );

    /* 
     *  If compared items don't differ in terms of being selected
     *  compare them based on population numbers.
     *  If compared items do differ in terms of being selected
     *  then let that be the final criteria.
     */
}


class PlaceMarkContainer : public QVector<PlaceMark*>
{
 public:
    PlaceMarkContainer();
    PlaceMarkContainer( const QString& name );

    void setName( const QString& name ) { m_name = name; }
    QString name() const         { return m_name; }

    inline void deleteAll()
    {
        foreach ( PlaceMark* mark, *this ) {
            if ( mark != 0 )
                delete mark;
        }
    }

    inline void clearSelected()
    {
        foreach ( PlaceMark* mark, *this ) {
            if ( mark != 0 ) 
                mark->setSelected( 0 ); 
        } 
    }

    inline void sort()
    {
        qStableSort( begin(), end(), populationLessThan );
    }

 private:	
    QString  m_name;
};

#endif // PLACEMARKCONTAINER_H
