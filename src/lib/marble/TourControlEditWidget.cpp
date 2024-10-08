// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mihail Ivchenko <ematirov@gmail.com>
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
// SPDX-FileCopyrightText: 2014 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "TourControlEditWidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QToolButton>

#include "GeoDataTourControl.h"
#include "MarblePlacemarkModel.h"

namespace Marble
{

TourControlEditWidget::TourControlEditWidget(const QModelIndex &index, QWidget *parent)
    : QWidget(parent)
    , m_index(index)
    , m_radio_play(new QRadioButton(this))
    , m_radio_pause(new QRadioButton(this))
    , m_button(new QToolButton(this))
{
    auto layout = new QHBoxLayout(this);
    layout->setSpacing(5);

    auto iconLabel = new QLabel(this);
    iconLabel->setPixmap(QPixmap(QStringLiteral(":/marble/media-playback-pause.png")));
    layout->addWidget(iconLabel);

    layout->addWidget(m_radio_play);
    m_radio_play->setText(tr("Play"));

    layout->addWidget(m_radio_pause);
    m_radio_pause->setText(tr("Pause"));

    if (tourControlElement()->playMode() == GeoDataTourControl::Play) {
        m_radio_play->setChecked(true);
    } else {
        m_radio_pause->setChecked(true);
    }

    m_button->setIcon(QIcon(QStringLiteral(":/marble/document-save.png")));
    connect(m_button, &QAbstractButton::clicked, this, &TourControlEditWidget::save);
    layout->addWidget(m_button);
}

bool TourControlEditWidget::editable() const
{
    return m_button->isEnabled();
}

void TourControlEditWidget::setEditable(bool editable)
{
    m_button->setEnabled(editable);
}

void TourControlEditWidget::save()
{
    if (m_radio_play->isChecked()) {
        tourControlElement()->setPlayMode(GeoDataTourControl::Play);
    } else {
        tourControlElement()->setPlayMode(GeoDataTourControl::Pause);
    }
    Q_EMIT editingDone(m_index);
}

GeoDataTourControl *TourControlEditWidget::tourControlElement()
{
    auto object = qvariant_cast<GeoDataObject *>(m_index.data(MarblePlacemarkModel::ObjectPointerRole));
    Q_ASSERT(object);
    auto tourControl = geodata_cast<GeoDataTourControl>(object);
    Q_ASSERT(tourControl);
    return tourControl;
}

} // namespace Marble

#include "moc_TourControlEditWidget.cpp"
