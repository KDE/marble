//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Calin Cruceru <crucerucalincristian@gmail.com>
//

#ifndef MARBLE_EDITPOLYLINEDIALOG_H
#define MARBLE_EDITPOLYLINEDIALOG_H

#include <QDialog>


namespace Marble {

class GeoDataPlacemark;
class GeoDataFeature;
class GeoDataCoordinates;
class OsmPlacemarkData;

class EditPolylineDialog : public QDialog
{
    Q_OBJECT
public:
    EditPolylineDialog( GeoDataPlacemark *placemark,
                        const QHash<qint64, OsmPlacemarkData> *relations = 0,
                        QWidget *parent = 0 );
    ~EditPolylineDialog();

public Q_SLOTS:
    void handleAddingNode( const GeoDataCoordinates &node );
    void handleItemMoving( GeoDataPlacemark *item );
    void handleChangingStyle();

Q_SIGNALS:
    void polylineUpdated( GeoDataFeature *feature );

    /**
     * @brief relationCreated signals the annotate plugin that a new relation has been
     * created( or modified ) within the relation editor
     * @param relation the relation's osmData
     */
    void relationCreated( const OsmPlacemarkData &relation );

private Q_SLOTS:
    void updatePolyline();
    void updateLinesDialog( const QColor &color );
    void restoreInitial( int result );
    void checkFields();

private:
    class Private;
    Private * const d;
};

}

#endif
