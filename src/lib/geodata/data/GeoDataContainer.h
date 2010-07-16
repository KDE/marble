//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
// Copyright 2007      Inge Wallin   <inge@lysator.liu.se>
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//


#ifndef MARBLE_GEODATACONTAINER_H
#define MARBLE_GEODATACONTAINER_H

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
    /// Default constructor
    GeoDataContainer();
    GeoDataContainer( const GeoDataContainer& other );
    GeoDataContainer( const GeoDataFeature& other );
    GeoDataContainerPrivate* p() const;
    /// Destruct the GeoDataContainer
    virtual ~GeoDataContainer();

    /// Provides type information for downcasting a GeoData
    virtual QString nodeType() const;

    /**
     * @brief A convenience function that returns all folders in this container.
     * @return A QVector of GeoDataFolder
     *
     * @see GeoDataFolder
     */
    QVector<GeoDataFolder*> folderList() const;

    /**
     * @brief A convenience function that returns all features in this container.
     * @return A QVector of GeoDataFeature
     *
     * @see GeoDataFeature
     */
    QVector<GeoDataFeature*> featureList() const;

    /**
     * @brief A convenience function that returns all placemarks in this container.
     * @return A QVector of GeoDataPlacemark
     *
     * @see GeoDataPlacemark
     */
    QVector<GeoDataPlacemark*> placemarkList() const;
    
    /**
     * @brief  returns the requested child item
     */
    GeoDataFeature* child( int );
    
    /**
     * @brief returns the position of an item in the list
     */
    int childPosition( GeoDataFeature *child);
    
    /**
    * @brief add an element
    */
    void append( GeoDataFeature *other );

    void remove( int index );

    /**
    * @brief add an element
    */
    void append( const GeoDataFeature& other );

    /**
    * @brief size of the container
    */
    int size() const;

    /**
    * @brief return the reference of the element at a specific position
    */
    GeoDataFeature& at( int pos );
    const GeoDataFeature& at( int pos ) const;

    /**
    * @brief return the reference of the last element for convenience
    */
    GeoDataFeature& last();
    const GeoDataFeature& last() const;
    /**
    * @brief return the reference of the last element for convenience
    */
    GeoDataFeature& first();
    const GeoDataFeature& first() const;

    QVector<GeoDataFeature*>::Iterator begin();
    QVector<GeoDataFeature*>::Iterator end();
    QVector<GeoDataFeature*>::ConstIterator constBegin() const;
    QVector<GeoDataFeature*>::ConstIterator constEnd() const;
    void clear();

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
private:
    friend class GeoDataDocument;
    friend class GeoDataFolder;
    GeoDataContainer( GeoDataContainerPrivate *priv );
};

}

#endif
