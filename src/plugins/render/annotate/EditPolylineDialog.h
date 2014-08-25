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

class EditPolylineDialog : public QDialog
{
    Q_OBJECT

public:
    EditPolylineDialog( GeoDataPlacemark *placemark, QWidget *parent = 0 );
    ~EditPolylineDialog();

    void setFirstTimeEditing( bool enabled );

signals:
    void polylineUpdated( GeoDataFeature *feature );

private slots:
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
