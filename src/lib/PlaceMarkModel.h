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

//
// PlaceMarkModel resembles the model for Placemarks
//


#ifndef PLACEMARKMODEL_H
#define PLACEMARKMODEL_H


#include <QtCore/QAbstractListModel>

#include "marble_export.h"

class PlaceMarkContainer;
class PlaceMarkManager;

/**
 * This class represents a model of all place marks which
 * are currently available through a given PlaceMarkManager.
 */
class MARBLE_EXPORT PlaceMarkModel : public QAbstractListModel
{
    friend class PlaceMarkManager;

    Q_OBJECT

 public:
    /**
     * The roles of the place marks.
     */
    enum Roles
    {
      GeoTypeRole = Qt::UserRole + 1,  ///< The geo type (e.g. city or mountain)
      DescriptionRole,                 ///< The description
      CoordinateRole,                  ///< The GeoPoint coordinate
      CountryCodeRole,                 ///< The country code
      VisualCategoryRole,              ///< The category
      StyleRole,                       ///< The style
      PopularityIndexRole,             ///< The popularity index
      PopularityRole                   ///< The popularity
    };

    /**
     * Creates a new place mark model.
     *
     * @param manager The place mark manager on which the model shall work.
     * @param parent The parent object.
     */
    PlaceMarkModel( PlaceMarkManager *manager, QObject *parent = 0 );

    /**
     * Destroys the place mark model.
     */
    ~PlaceMarkModel();
	
    int rowCount( const QModelIndex &parent = QModelIndex() ) const;
    int columnCount( const QModelIndex &parent = QModelIndex() ) const;

    QVariant data( const QModelIndex &index, int role ) const;

    virtual QModelIndexList match( const QModelIndex &start, int role, 
                                   const QVariant &value, int hits = 1,
                                   Qt::MatchFlags flags = Qt::MatchFlags( Qt::MatchStartsWith | Qt::MatchWrap ) ) const;

 private:
    /**
     * This method is used by the PlaceMarkManager to add new
     * place marks to the model.
     *
     * Note: The model takes ownership of the place marks!
     */
    void addPlaceMarks( const PlaceMarkContainer &placeMarks, bool clearPrevious = false );

    /**
     * This method is used by the PlaceMarkManager to clear
     * the model.
     */
    void clearPlaceMarks();

    class Private;
    Private* const d;
};


#endif // PLACEMARKMODEL_H
