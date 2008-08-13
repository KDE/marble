//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
// Copyright 2007      Inge Wallin   <inge@lysator.liu.se>
//


#ifndef GEODATACONTAINER_H
#define GEODATACONTAINER_H

#include <QtCore/QVector>

#include "geodata_export.h"

#include "GeoDataFeature.h"

namespace Marble
{

class GeoDataContainerPrivate;

class GeoDataFolder;
class GeoDataPlacemark;

/**
 * @short  A base class that can hold GeoDataFeatures
 *
 * GeoDataContainer is the base class for the GeoData container
 * classes GeoDataFolder and GeoDataDocument.  It is never
 * instantiated by itself, but is always used as part of a derived
 * class.
 *
 * It is based on GeoDataFeature, and it only adds a
 * QVector<GeodataFeature *> to it, making it a Feature that can hold
 * other Features.
 *
 * @see GeoDataFolder
 * @see GeoDataDocument
 */
class GEODATA_EXPORT GeoDataContainer : public GeoDataFeature
{
 public:
    /// Destruct the GeoDataContainer
    virtual ~GeoDataContainer();

    /**
     * @brief A convenience function that returns all folders in this container.
     * @return A QVector of GeoDataFolder*
     *
     * @see GeoDataFolder
     */
    QVector<GeoDataFolder*> folders() const;

    /**
     * @brief A convenience function that returns all features in this container.
     * @return A QVector of GeoDataFeature*
     *
     * @see GeoDataFeature
     */
    QVector<GeoDataFeature*> features() const;

    /**
     * @brief A convenience function that returns all placemarks in this container.
     * @return A QVector of GeoDataPlacemark*
     *
     * @see GeoDataPlacemark
     * @see PlaceMarkContainer
     */
    QVector<GeoDataPlacemark*> placemarks() const;
    
    /**
     * @brief  Add a feature to the container
     * @param  feature  the new feature
     */
    void addFeature( GeoDataFeature* feature );

    /**
     * @brief  Serialize the container to a stream.
     * @param  stream  the stream
     */
    virtual void pack( QDataStream& stream ) const;
    /**
     * @brief  Unserialize the container from a stream
     * @param  stream  the stream
     */
    virtual void unpack( QDataStream& stream );

 protected:
    /// Default constructor
    GeoDataContainer();

 private:
    Q_DISABLE_COPY( GeoDataContainer )
    GeoDataContainerPrivate * const d;
};

}

#endif // GEODATACONTAINER_H
