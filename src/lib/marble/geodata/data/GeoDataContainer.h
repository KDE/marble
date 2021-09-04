// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2007 Murad Tagirov <tmurad@gmail.com>
// SPDX-FileCopyrightText: 2007 Inge Wallin <inge@lysator.liu.se>
// SPDX-FileCopyrightText: 2009 Patrick Spendrin <ps_ml@gmx.de>
//


#ifndef MARBLE_GEODATACONTAINER_H
#define MARBLE_GEODATACONTAINER_H

#include <QVector>

#include "geodata_export.h"

#include "GeoDataFeature.h"

namespace Marble
{

class GeoDataContainerPrivate;

class GeoDataFolder;
class GeoDataPlacemark;
class GeoDataLatLonAltBox;

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
    /// Destruct the GeoDataContainer
    ~GeoDataContainer() override;

    GeoDataContainer& operator=(const GeoDataContainer& other);

    /**
     * @brief A convenience function that returns the LatLonAltBox of all
     * placemarks in this container.
     * @return The GeoDataLatLonAltBox
     *
     * @see GeoDataLatLonAltBox
     */
    GeoDataLatLonAltBox latLonAltBox() const;

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
     * @brief  returns the requested child item
     */
    const GeoDataFeature* child( int ) const;

    /**
     * @brief returns the position of an item in the list
     */
    int childPosition( const GeoDataFeature *child) const;

    /**
     * @brief inserts @p feature at position @p index in the container
     */
    void insert( int index, GeoDataFeature *feature );

    GEODATA_DEPRECATED void insert(GeoDataFeature *other, int index);

    /**
    * @brief add an element
    */
    void append( GeoDataFeature *other );

    void remove( int index );

    void remove(int index, int count);

    int	removeAll(GeoDataFeature* feature);

    void removeAt(int index);

    void removeFirst();

    void removeLast();

    bool removeOne( GeoDataFeature *feature );

    /**
    * @brief size of the container
    */
    int size() const;

    /**
     * @brief Returns true if the container has size 0; otherwise returns false.
     */
    bool isEmpty() const;

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
    void pack( QDataStream& stream ) const override;
    /**
     * @brief  Unserialize the container from a stream
     * @param  stream  the stream
     */
    void unpack( QDataStream& stream ) override;

 protected:
    explicit GeoDataContainer(GeoDataContainerPrivate *priv);
    GeoDataContainer(const GeoDataContainer& other, GeoDataContainerPrivate *priv);

    bool equals( const GeoDataContainer &other ) const;
    using GeoDataFeature::equals;

 private:
    Q_DECLARE_PRIVATE(GeoDataContainer)
};

}

#endif
