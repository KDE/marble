//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Bastian Holst <bastianholst@gmx.de>
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#ifndef MARBLE_GEOGRAPHICSSCENE_H
#define MARBLE_GEOGRAPHICSSCENE_H

#include "marble_export.h"
#include "MarbleGlobal.h"

#include <QtCore/QObject>
#include <QtCore/QList>

namespace Marble
{

class GeoGraphicsItem;
class GeoDataLatLonAltBox;
class GeoGraphicsScenePrivate;

/**
 * @short This is the home of all GeoGraphicsItems to be shown on the map.
 */
class MARBLE_EXPORT GeoGraphicsScene : public QObject
{
    Q_OBJECT

public:
    /**
     * Creates a new instance of GeoGraphicsScene
     * @param parent the QObject parent of the Scene
     */
    GeoGraphicsScene( QObject *parent = 0 );
    ~GeoGraphicsScene();

    /**
     * @brief Add an item to the GeoGraphicsScene
     * Adds the item @p item to the GeoGraphicsScene
     */
    void addItem( GeoGraphicsItem *item );

    /**
     * @brief Remove an item from the GeoGraphicsScene
     * Removes the item @p item from the GeoGraphicsScene
     */
    void removeItem( GeoGraphicsItem *item );

    /**
     * @brief Remove all items from the GeoGraphicsScene
     * Removes all items from the GeoGraphicsScene
     */
    void clear();

    /**
     * @brief Get all items in the GeoGraphicsScene
     * Returns all items in the GeoGraphicsScene.
     * The items will be returned in no specific order.
     *
     * @return The list of all GeoGraphicsItems
     */
    QList<GeoGraphicsItem *> items() const;

    /**
     * @brief Get the list of items in the specified Box
     *
     * @param box The box around the items.
     * @param maxZoomLevel The max zoom level of tiling
     * @return The list of items in the specified box in no specific order.
     */
    QList<GeoGraphicsItem *> items( const GeoDataLatLonAltBox& box, int maxZoomLevel ) const;
    
    /**
     * @brief default zoom level used for tiling
     */
    static int s_tileZoomLevel;

private:
    GeoGraphicsScenePrivate * const d;
};
}
#endif // MARBLE_GEOGRAPHICSSCENE_H
