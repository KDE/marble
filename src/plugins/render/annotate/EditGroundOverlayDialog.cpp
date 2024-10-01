// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Adrian Draghici <draghici.adrian.b@gmail.com>
//

// Self
#include "EditGroundOverlayDialog.h"
#include "ui_EditGroundOverlayDialog.h"

// Qt
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>

// Marble
#include "FormattedTextWidget.h"
#include "GeoDataGroundOverlay.h"
#include "TextureLayer.h"

namespace Marble
{

class Q_DECL_HIDDEN EditGroundOverlayDialog::Private : public Ui::UiEditGroundOverlayDialog
{
public:
    GeoDataGroundOverlay *m_overlay;
    TextureLayer *m_textureLayer;

    Private(GeoDataGroundOverlay *overlay, TextureLayer *textureLayer);
    ~Private();

    void updateCoordinates();
};

EditGroundOverlayDialog::Private::Private(GeoDataGroundOverlay *overlay, TextureLayer *textureLayer)
    : Ui::UiEditGroundOverlayDialog()
    , m_overlay(overlay)
    , m_textureLayer(textureLayer)
{
    // nothing to do
}

EditGroundOverlayDialog::Private::~Private()
{
    // nothing to do
}

EditGroundOverlayDialog::EditGroundOverlayDialog(GeoDataGroundOverlay *overlay, TextureLayer *textureLayer, QWidget *parent)
    : QDialog(parent)
    , d(new Private(overlay, textureLayer))
{
    d->setupUi(this);

    d->m_header->setName(overlay->name());
    d->m_header->setIconLink(overlay->absoluteIconFile());
    d->m_header->setPositionVisible(false);
    d->m_formattedTextWidget->setText(overlay->description());

    d->m_north->setRange(-90, 90);
    d->m_south->setRange(-90, 90);
    d->m_west->setRange(-180, 180);
    d->m_east->setRange(-180, 180);
    d->m_rotation->setRange(-360, 360);

    GeoDataLatLonBox latLonBox = overlay->latLonBox();
    d->m_north->setValue(latLonBox.north(GeoDataCoordinates::Degree));
    d->m_south->setValue(latLonBox.south(GeoDataCoordinates::Degree));
    d->m_west->setValue(latLonBox.west(GeoDataCoordinates::Degree));
    d->m_east->setValue(latLonBox.east(GeoDataCoordinates::Degree));
    d->m_rotation->setValue(latLonBox.rotation(GeoDataCoordinates::Degree));

    connect(d->buttonBox->button(QDialogButtonBox::Ok), SIGNAL(pressed()), this, SLOT(checkFields()));
}

EditGroundOverlayDialog::~EditGroundOverlayDialog()
{
    delete d;
}

void EditGroundOverlayDialog::updateGroundOverlay()
{
    d->m_overlay->setName(d->m_header->name());
    d->m_overlay->setIconFile(d->m_header->iconLink());
    d->m_overlay->setDescription(d->m_formattedTextWidget->text());

    d->m_overlay->latLonBox().setBoundaries(d->m_north->value(), d->m_south->value(), d->m_east->value(), d->m_west->value(), GeoDataCoordinates::Degree);

    d->m_overlay->latLonBox().setRotation(d->m_rotation->value(), GeoDataCoordinates::Degree);
}

void EditGroundOverlayDialog::setGroundOverlayUpdated()
{
    Q_EMIT groundOverlayUpdated(d->m_overlay);
}

void EditGroundOverlayDialog::checkFields()
{
    if (d->m_header->name().isEmpty()) {
        QMessageBox::warning(this, tr("No name specified"), tr("Please specify a name for this ground overlay."));
    } else if (d->m_header->iconLink().isEmpty()) {
        QMessageBox::warning(this, tr("No image specified"), tr("Please specify an image file."));
    } else if (!QFileInfo::exists(d->m_header->iconLink())) {
        QMessageBox::warning(this, tr("Invalid image path"), tr("Please specify a valid path for the image file."));
    } else {
        this->updateGroundOverlay();
        this->setGroundOverlayUpdated();
        d->m_textureLayer->reset();
        accept();
    }
}

}

#include "moc_EditGroundOverlayDialog.cpp"
