//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Calin Cruceru <crucerucalincristian@gmail.com>
//

#ifndef MARBLE_EDITPOLYGONDIALOG_H
#define MARBLE_EDITPOLYGONDIALOG_H

#include <QDialog>
#include <QColor>

#include "MarbleGlobal.h"


namespace Marble {

class GeoDataPlacemark;
class GeoDataCoordinates;
class GeoDataFeature;
class OsmPlacemarkData;

/**
 * @brief As it name says by itself, this class is used to show a couple of
 * editing options for a polygon. So far there are only a few customization
 * options, such as lines width, lines/area color, lines/area opacity.
 */
class EditPolygonDialog : public QDialog
{
    Q_OBJECT

public:
    EditPolygonDialog( GeoDataPlacemark *placemark,
                       const QHash<qint64, OsmPlacemarkData> *relations = 0,
                       QWidget *parent = 0 );
    ~EditPolygonDialog();

public Q_SLOTS:
    void handleAddingNode( const GeoDataCoordinates &node );
    void handleItemMoving( GeoDataPlacemark *item );
    void handleChangingStyle();
    void updatePolygon();

Q_SIGNALS:
    void polygonUpdated( GeoDataFeature *feature );

    /**
     * @brief relationCreated signals the annotate plugin that a new relation has been
     * created( or modified ) within the relation editor
     * @param relation the relation's osmData
     */
    void relationCreated( const OsmPlacemarkData &relation );

private Q_SLOTS:
    void updateLinesDialog( const QColor &color );
    void updatePolyDialog( const QColor &color );
    void checkFields();
    void restoreInitial( int result );

private:
    class Private;
    Private * const d;
};

}

#endif
