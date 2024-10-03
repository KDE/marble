// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mihail Ivchenko <ematirov@gmail.com>
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
// SPDX-FileCopyrightText: 2014 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "FlyToEditWidget.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>

#include "GeoDataCamera.h"
#include "GeoDataLookAt.h"
#include "MarblePlacemarkModel.h"
#include "MarbleWidget.h"
#include "geodata/data/GeoDataFlyTo.h"

namespace Marble
{

FlyToEditWidget::FlyToEditWidget(const QModelIndex &index, MarbleWidget *widget, QWidget *parent)
    : QWidget(parent)
    , m_widget(widget)
    , m_index(index)
    , m_button(new QToolButton)
{
    auto layout = new QHBoxLayout(this);
    layout->setSpacing(5);

    auto iconLabel = new QLabel;
    iconLabel->setPixmap(QPixmap(QStringLiteral(":/marble/flag.png")));
    layout->addWidget(iconLabel);

    auto pairLayout = new QHBoxLayout;
    pairLayout->setSpacing(10);

    auto durationLayout = new QHBoxLayout;
    durationLayout->setSpacing(5);

    auto durationLabel = new QLabel;
    durationLabel->setText(tr("Duration:"));
    durationLayout->addWidget(durationLabel);

    m_durationSpin = new QDoubleSpinBox;
    durationLayout->addWidget(m_durationSpin);
    m_durationSpin->setValue(flyToElement()->duration());
    m_durationSpin->setSuffix(tr(" s", "seconds"));

    auto modeLayout = new QHBoxLayout;
    modeLayout->addSpacing(5);

    auto modeLabel = new QLabel;
    modeLabel->setText(tr("Mode:"));
    modeLayout->addWidget(modeLabel);

    m_modeCombo = new QComboBox;
    modeLayout->addWidget(m_modeCombo);
    m_modeCombo->addItem(tr("Smooth"));
    m_modeCombo->addItem(tr("Bounce"));

    if (flyToElement()->flyToMode() == GeoDataFlyTo::Smooth) {
        m_modeCombo->setCurrentIndex(0);
    } else if (flyToElement()->flyToMode() == GeoDataFlyTo::Bounce) {
        m_modeCombo->setCurrentIndex(1);
    } else {
        m_modeCombo->setCurrentIndex(-1);
    }

    pairLayout->addLayout(durationLayout);
    pairLayout->addLayout(modeLayout);

    layout->addLayout(pairLayout);

    auto flyToPinCenter = new QToolButton;
    flyToPinCenter->setIcon(QIcon(QStringLiteral(":/marble/places.png")));
    flyToPinCenter->setToolTip(tr("Current map center"));
    connect(flyToPinCenter, &QAbstractButton::clicked, this, &FlyToEditWidget::updateCoordinates);
    layout->addWidget(flyToPinCenter);

    m_button->setIcon(QIcon(QStringLiteral(":/marble/document-save.png")));
    connect(m_button, &QAbstractButton::clicked, this, &FlyToEditWidget::save);
    layout->addWidget(m_button);
}

bool FlyToEditWidget::editable() const
{
    return m_button->isEnabled();
}

void FlyToEditWidget::setEditable(bool editable)
{
    m_button->setEnabled(editable);
}

void FlyToEditWidget::setFirstFlyTo(const QPersistentModelIndex &index)
{
    if (m_index.internalPointer() == index.internalPointer()) {
        m_durationSpin->setValue(0);
    }
}

void FlyToEditWidget::updateCoordinates()
{
    m_coord = m_widget->focusPoint();
    m_coord.setAltitude(m_widget->lookAt().range());
}

void FlyToEditWidget::save()
{
    if (flyToElement()->view() != nullptr && m_coord != GeoDataCoordinates()) {
        GeoDataCoordinates coords = m_coord;
        if (auto camera = geodata_cast<GeoDataCamera>(flyToElement()->view())) {
            camera->setCoordinates(coords);
        } else if (auto lookAt = geodata_cast<GeoDataLookAt>(flyToElement()->view())) {
            lookAt->setCoordinates(coords);
        } else {
            lookAt = new GeoDataLookAt;
            lookAt->setCoordinates(coords);
            flyToElement()->setView(lookAt);
        }
    }

    flyToElement()->setDuration(m_durationSpin->value());

    if (m_modeCombo->currentIndex() == 0) {
        flyToElement()->setFlyToMode(GeoDataFlyTo::Smooth);
    } else if (m_modeCombo->currentIndex() == 1) {
        flyToElement()->setFlyToMode(GeoDataFlyTo::Bounce);
    }

    Q_EMIT editingDone(m_index);
}

GeoDataFlyTo *FlyToEditWidget::flyToElement()
{
    auto object = qvariant_cast<GeoDataObject *>(m_index.data(MarblePlacemarkModel::ObjectPointerRole));
    Q_ASSERT(object);
    auto flyTo = geodata_cast<GeoDataFlyTo>(object);
    Q_ASSERT(flyTo);
    return flyTo;
}

} // namespace Marble

#include "moc_FlyToEditWidget.cpp"
