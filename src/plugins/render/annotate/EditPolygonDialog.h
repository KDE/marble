//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Calin Cruceru <crucerucalincristian@gmail.com>
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
                       const QHash<qint64, OsmPlacemarkData> *relations = nullptr,
                       QWidget *parent = nullptr );
    ~EditPolygonDialog() override;

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
