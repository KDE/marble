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

#include "geodata/data/GeoDataPlacemark.h"

/**
 * A helper class which contains a couple of place mark
 * objects.
 *
 * The ownership of the place mark objects is transfered to the container!
 */
class PlaceMarkContainer : public QVector<GeoDataPlacemark*>
{
 public:
    /// Create a new PlaceMarkContainer.
    PlaceMarkContainer();
    /**
     * @brief Create a new PlaceMarkContainer
     * param  name the name of the container
     */
    PlaceMarkContainer( const QString& name );

    /// Return the name of the container
    QString name() const;
    /**
     * @brief Set a new name for the PlaceMarkContainer
     * param  name the new name of the container
     */
    void setName( const QString& name );

    /// @brief Sort the placemarks in descending order by population.
    void sort();

 private:	
    QString  m_name;
};

#endif // PLACEMARKCONTAINER_H
