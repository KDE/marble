// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Mihail Ivchenko <ematirov@gmail.com>
//

#include "RemoveItemEditWidget.h"

#include <QHBoxLayout>
#include <QLabel>

#include "GeoDataDelete.h"
#include "GeoDataUpdate.h"
#include "MarblePlacemarkModel.h"
#include "MarbleWidget.h"
#include "geodata/data/GeoDataAnimatedUpdate.h"

namespace Marble
{

RemoveItemEditWidget::RemoveItemEditWidget(const QModelIndex &index, QWidget *parent)
    : QWidget(parent)
    , m_index(index)
    , m_button(new QToolButton)
    , m_comboBox(new QComboBox)
{
    auto layout = new QHBoxLayout(this);
    layout->setSpacing(5);

    auto iconLabel = new QLabel;
    iconLabel->setPixmap(QPixmap(QStringLiteral(":/icons/remove.png")));
    layout->addWidget(iconLabel);

    auto comboBoxLabel = new QLabel;
    comboBoxLabel->setText(tr("Choose item:"));
    layout->addWidget(comboBoxLabel);

    layout->addWidget(m_comboBox);

    m_button->setIcon(QIcon(QStringLiteral(":/marble/document-save.png")));
    connect(m_button, &QAbstractButton::clicked, this, &RemoveItemEditWidget::save);
    layout->addWidget(m_button);
}

bool RemoveItemEditWidget::editable() const
{
    return m_button->isEnabled();
}

void RemoveItemEditWidget::setFeatureIds(const QStringList &ids)
{
    QString id = animatedUpdateElement()->update()->getDelete()->first().targetId();
    QString current = m_comboBox->currentIndex() == -1 ? id : m_comboBox->currentText();
    m_comboBox->clear();
    m_comboBox->addItems(ids);
    m_comboBox->setCurrentIndex(m_comboBox->findText(current));
}

void RemoveItemEditWidget::setDefaultFeatureId(const QString &featureId)
{
    if (m_comboBox->currentIndex() == -1) {
        m_comboBox->setCurrentIndex(m_comboBox->findText(featureId));
    }
}

void RemoveItemEditWidget::setEditable(bool editable)
{
    m_button->setEnabled(editable);
}

void RemoveItemEditWidget::save()
{
    animatedUpdateElement()->update()->getDelete()->child(0)->setTargetId(m_comboBox->currentText());
    Q_EMIT editingDone(m_index);
}

GeoDataAnimatedUpdate *RemoveItemEditWidget::animatedUpdateElement()
{
    auto object = qvariant_cast<GeoDataObject *>(m_index.data(MarblePlacemarkModel::ObjectPointerRole));
    Q_ASSERT(object);
    auto animatedUpdate = geodata_cast<GeoDataAnimatedUpdate>(object);
    Q_ASSERT(animatedUpdate);
    return animatedUpdate;
}

} // namespace Marble

#include "moc_RemoveItemEditWidget.cpp"
