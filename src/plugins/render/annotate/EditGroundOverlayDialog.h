//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Adrian Draghici <draghici.adrian.b@gmail.com>
//

#ifndef MARBLE_EDITGROUNDOVERLAYDIALOG_H
#define MARBLE_EDITGROUNDOVERLAYDIALOG_H

#include <QDialog>

namespace Marble
{
class TextureLayer;

class GeoDataGroundOverlay;

class EditGroundOverlayDialog : public QDialog
{
    Q_OBJECT

public:
    EditGroundOverlayDialog( GeoDataGroundOverlay *overlay, TextureLayer *textureLayer, QWidget *parent = nullptr );
    ~EditGroundOverlayDialog() override;

private Q_SLOTS:
    void updateGroundOverlay();
    void setGroundOverlayUpdated();
    void checkFields();

Q_SIGNALS:
    void groundOverlayUpdated( GeoDataGroundOverlay* );

private:
    class Private;
    Private * const d;
};

}

#endif
