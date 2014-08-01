//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Adrian Draghici <draghici.adrian.b@gmail.com>
//

#ifndef MARBLE_EDITGROUNDOVERLAYDIALOG_H
#define MARBLE_EDITGROUNDOVERLAYDIALOG_H

#include <QDialog>

#include "GeoDataGroundOverlay.h"
#include "TextureLayer.h"

namespace Marble
{

class EditGroundOverlayDialog : public QDialog
{
    Q_OBJECT

public:
    EditGroundOverlayDialog( GeoDataGroundOverlay *overlay, TextureLayer *textureLayer, QWidget *parent = 0 );
    ~EditGroundOverlayDialog();

private slots:
    void loadPicture();
    void updateGroundOverlay();
    void setGroundOverlayUpdated();
    void checkFields();

signals:
    void groundOverlayUpdated( GeoDataGroundOverlay* );

private:
    class Private;
    Private *d;
};

}

#endif
