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
#include "GeoDataPlacemark.h"


namespace Marble {

/**
 * @brief As it name says by itself, this class is used to show a couple of
 * editing options for a polygon. So far there are only a few customization
 * options, such as lines width, lines/area color, lines/area opacity.
 */
class EditPolygonDialog : public QDialog
{
    Q_OBJECT

public:
    EditPolygonDialog( GeoDataPlacemark *placemark, QWidget *parent = 0 );
    ~EditPolygonDialog();

    void setFirstTimeEditing( bool enabled );

signals:
    void polygonUpdated( GeoDataFeature *feature );
    void removeRequested();

private slots:
    void updatePolygon();
    void updateLinesDialog( const QColor &color );
    void updatePolyDialog( const QColor &color );
    void checkFields();
    void restoreInitial();

private:
    class Private;
    Private * const d;
};

}

#endif
