//
// This file is part of the Marble Virtual Globe.
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

#include <QObject>
#include <QList>

namespace Marble
{

class GeoGraphicsItem;
class GeoDataFeature;
class GeoDataLatLonBox;
class GeoGraphicsScenePrivate;
class GeoDataPlacemark;

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
    explicit GeoGraphicsScene( QObject *parent = 0 );
    ~GeoGraphicsScene();

    /**
     * @brief Add an item to the GeoGraphicsScene
     * Adds the item @p item to the GeoGraphicsScene
     */
    void addItem( GeoGraphicsItem *item );

    /**
     * @brief Remove all concerned items from the GeoGraphicsScene
     * Removes all items which are associated with @p object from the GeoGraphicsScene
     */
    void removeItem( const GeoDataFeature *feature );

    /**
     * @brief Remove all items from the GeoGraphicsScene
     */
    void clear();

    /**
     * @brief Get the list of items in the specified Box
     *
     * @param box The box around the items.
     * @param maxZoomLevel The max zoom level of tiling
     * @return The list of items in the specified box in no specific order.
     */
    QList<GeoGraphicsItem *> items( const GeoDataLatLonBox &box, int maxZoomLevel ) const;

    /**
     * @brief Get the list of items which belong to a placemark
     * that has been clicked.
     * @return Returns a list of selected Items
     */
    QList<GeoGraphicsItem*> selectedItems() const;

public Q_SLOTS:
    void applyHighlight( const QVector<GeoDataPlacemark*>& );

Q_SIGNALS:
    void repaintNeeded();

private:
    GeoGraphicsScenePrivate * const d;
};
}
#endif // MARBLE_GEOGRAPHICSSCENE_H
