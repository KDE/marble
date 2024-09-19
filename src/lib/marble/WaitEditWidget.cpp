// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mihail Ivchenko <ematirov@gmail.com>
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
// SPDX-FileCopyrightText: 2014 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "WaitEditWidget.h"

#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>

#include "GeoDataTourControl.h"
#include "MarblePlacemarkModel.h"
#include "geodata/data/GeoDataWait.h"

namespace Marble
{

WaitEditWidget::WaitEditWidget(const QModelIndex &index, QWidget *parent)
    : QWidget(parent)
    , m_index(index)
    , m_spinBox(new QDoubleSpinBox)
    , m_button(new QToolButton)
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(5);

    QLabel *iconLabel = new QLabel;
    iconLabel->setPixmap(QPixmap(QStringLiteral(":/marble/player-time.png")));
    layout->addWidget(iconLabel);

    QLabel *waitLabel = new QLabel;
    waitLabel->setText(tr("Wait duration:"));
    layout->addWidget(waitLabel);

    layout->addWidget(m_spinBox);
    m_spinBox->setValue(waitElement()->duration());
    m_spinBox->setSuffix(tr(" s", "seconds"));

    m_button->setIcon(QIcon(QStringLiteral(":/marble/document-save.png")));
    connect(m_button, SIGNAL(clicked()), this, SLOT(save()));
    layout->addWidget(m_button);

    setLayout(layout);
}

bool WaitEditWidget::editable() const
{
    return m_button->isEnabled();
}

void WaitEditWidget::setEditable(bool editable)
{
    m_button->setEnabled(editable);
}

void WaitEditWidget::save()
{
    waitElement()->setDuration(m_spinBox->value());
    emit editingDone(m_index);
}

GeoDataWait *WaitEditWidget::waitElement()
{
    GeoDataObject *object = qvariant_cast<GeoDataObject *>(m_index.data(MarblePlacemarkModel::ObjectPointerRole));
    Q_ASSERT(object);
    auto wait = geodata_cast<GeoDataWait>(object);
    Q_ASSERT(wait);
    return wait;
}

} // namespace Marble

#include "moc_WaitEditWidget.cpp"
