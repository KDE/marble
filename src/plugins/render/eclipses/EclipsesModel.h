//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Rene Kuettner <rene@bitkanal.net>
//

#ifndef MARBLE_ECLIPSESMODEL_H
#define MARBLE_ECLIPSESMODEL_H

#include <QAbstractItemModel>

#include "GeoDataCoordinates.h"
#include "MarbleModel.h"

class EclSolar;

namespace Marble
{

class EclipsesItem;

/**
 * @brief The model for eclipses
 *
 * EclipsesModel provides an interface to the eclsolar backend. Instances
 * of this class hold EclipseItem objects for every eclipse event of a given
 * year. Furthermore, it implements QTs AbstractItemModel interface and can
 * be used with QTs view classes.
 */
class EclipsesModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit EclipsesModel( const MarbleModel *model, QObject *parent = 0 );

    ~EclipsesModel();

    /**
     * @brief Return the current observation point
     *
     * Returns the current observation point on which location specific
     * eclipse calculations are based.
     *
     * @return GeoDataCoordinates of the current observation point
     * @see setObservationPoint
     */
    const GeoDataCoordinates& observationPoint() const;

    /**
     * @brief Set the current observation point
     *
     * @param coords
     *
     * Set the current observation point to @p coords. This will mark all
     * items for recalculation of eclipses details according to the new
     * observation point.
     *
     * @see observationPoint
     */
    void setObservationPoint( const GeoDataCoordinates &coords );

    /**
     * @brief Set the year
     *
     * @param year The year
     *
     * Sets the year to @p year. This clears all items in the model and
     * fills it with all eclipse items for the given year.
     *
     * @see year
     */
    void setYear( int year );

    /**
     * @brief Return the year
     *
     * Returns the year of all eclipse items in this model.
     *
     * @return the year of eclipse items in this model
     * @see setYear
     */
    int year() const;

    /**
     * @brief Set if lunar eclipses are enbled
     * @param enable Indicates whether or not to allow lunar eclipses
     *
     * Allows to enable or disable inclusion of lunar eclipses.
     *
     * @see withLunarEclipses
     */
    void setWithLunarEclipses( const bool enable );

    /**
     * @brief Return whether or not lunar eclipses are enabled
     *
     * Returns whether or not lunar eclipses are included in the eclipse
     * calculation.
     *
     * @return True if lunar eclipses are enabled or false otherwise
     * @see setWithLunarEclipses
     */
    bool withLunarEclipses() const;

    /**
     * @brief Get eclipse item of a given year
     *
     * @param index
     *
     * This returns the eclipse item with @p index for the year set. If
     * there is no eclipse with @p index in the set year, NULL will be
     * returned.
     *
     * @return the requested eclipse item or NULL if there is no eclipse
     * @see setYear
     */
    EclipsesItem* eclipseWithIndex( int index );

    /**
     * @brief Return the items in this model
     *
     * Returns a list of items currently in the model.
     *
     * @return list of items in the model
     */
    QList<EclipsesItem*> items() const;

    // QT abstract item model interface
    QModelIndex index( int row, int column,
                       const QModelIndex &parent = QModelIndex() ) const;
    QModelIndex parent( const QModelIndex &index ) const;
    int rowCount( const QModelIndex &parent = QModelIndex() ) const;
    int columnCount( const QModelIndex &parent = QModelIndex() ) const;
    QVariant data( const QModelIndex &index,
                   int role = Qt::DisplayRole ) const;
    QVariant headerData( int section, Qt::Orientation orientation,
                         int role = Qt::DisplayRole ) const;

public Q_SLOTS:
    /**
     * @brief Update the list of eclipse items
     *
     * This forces an update of the current list of eclipse items by
     * calculating all eclipse events for the currently set year and
     * adding them to the model. All previously added items are
     * cleared before.
     *
     * @see clear
     */
    void update();

private:
    /**
     * @brief Add an item to the model
     * @param item the item to add
     *
     * Adds @p item to the model.
     *
     * @see clear
     */
    void addItem( EclipsesItem *item );

    /**
     * @brief Clears all items
     *
     * Clear the model by removing all items.
     *
     * @see addItem
     */
    void clear();

    const MarbleModel *m_marbleModel;
    EclSolar *m_ecl;
    QList<EclipsesItem*> m_items;
    int m_currentYear;
    bool m_withLunarEclipses;
    GeoDataCoordinates m_observationPoint;
};

}

#endif // MARBLE_ECLIPSESMODEL_H
