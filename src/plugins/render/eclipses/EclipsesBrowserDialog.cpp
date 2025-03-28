// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Rene Kuettner <rene@bitkanal.net>
//

#include "EclipsesBrowserDialog.h"

#include "MarbleClock.h"
#include "MarbleDebug.h"
#include "MarbleModel.h"

#include "EclipsesItem.h"
#include "EclipsesModel.h"

#include "ui_EclipsesBrowserDialog.h"

namespace Marble
{

EclipsesBrowserDialog::EclipsesBrowserDialog(const MarbleModel *model, QWidget *parent)
    : QDialog(parent)
    , m_marbleModel(model)
{
    initialize();
}

EclipsesBrowserDialog::~EclipsesBrowserDialog() = default;

void EclipsesBrowserDialog::setYear(int year)
{
    m_browserWidget->spinBoxYear->setValue(year);
}

int EclipsesBrowserDialog::year() const
{
    return m_browserWidget->spinBoxYear->value();
}

void EclipsesBrowserDialog::setWithLunarEclipses(const bool enable)
{
    if (m_eclModel->withLunarEclipses() != enable) {
        m_eclModel->setWithLunarEclipses(enable);
        m_eclModel->update();
    }
}

bool EclipsesBrowserDialog::withLunarEclipses() const
{
    return m_eclModel->withLunarEclipses();
}

void EclipsesBrowserDialog::accept()
{
    QItemSelectionModel *s = m_browserWidget->treeView->selectionModel();
    QModelIndex selected = s->currentIndex();

    if (selected.isValid()) {
        auto item = static_cast<EclipsesItem *>(selected.internalPointer());
        Q_EMIT buttonShowClicked(m_eclModel->year(), item->index());
    }

    QDialog::accept();
}

void EclipsesBrowserDialog::updateEclipsesForYear(int year)
{
    m_eclModel->setYear(year);
    updateButtonStates();
}

void EclipsesBrowserDialog::updateButtonStates()
{
    QItemSelectionModel *s = m_browserWidget->treeView->selectionModel();
    m_browserWidget->buttonShow->setEnabled(s->hasSelection());
}

void EclipsesBrowserDialog::initialize()
{
    m_browserWidget = std::make_unique<Ui::EclipsesBrowserDialog>();
    m_browserWidget->setupUi(this);

    m_browserWidget->treeView->setExpandsOnDoubleClick(false);

    if (!m_eclModel) {
        m_eclModel = new EclipsesModel(m_marbleModel);
    }
    m_browserWidget->treeView->setModel(m_eclModel);

    connect(m_browserWidget->buttonShow, &QAbstractButton::clicked, this, &EclipsesBrowserDialog::accept);
    connect(m_browserWidget->buttonClose, &QAbstractButton::clicked, this, &QDialog::reject);
    connect(m_browserWidget->spinBoxYear, &QSpinBox::valueChanged, this, &EclipsesBrowserDialog::updateEclipsesForYear);
    connect(m_browserWidget->treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &EclipsesBrowserDialog::updateButtonStates);
    connect(m_browserWidget->buttonSettings, &QAbstractButton::clicked, this, &EclipsesBrowserDialog::buttonSettingsClicked);

    setYear(m_marbleModel->clock()->dateTime().date().year());

    update();

    m_browserWidget->treeView->resizeColumnToContents(2);
    m_browserWidget->treeView->resizeColumnToContents(3);
}

} // namespace Marble

#include "moc_EclipsesBrowserDialog.cpp"
