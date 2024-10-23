// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008 David Roberts <dvdr18@gmail.com>
// SPDX-FileCopyrightText: 2008 Inge Wallin <inge@lysator.liu.se>
// SPDX-FileCopyrightText: 2010 Harshit Jain <hjain.itbhu@gmail.com>
// SPDX-FileCopyrightText: 2012 Mohammed Nafees <nafees.technocool@gmail.com>
//

// Own
#include "SunControlWidget.h"
#include "ui_SunControlWidget.h"

// Qt
#include <QPushButton>
#include <QShowEvent>

// Marble
#include "MarbleWidget.h"

using namespace Marble;
/* TRANSLATOR Marble::SunControlWidget */

SunControlWidget::SunControlWidget(MarbleWidget *marbleWidget, QWidget *parent)
    : QDialog(parent)
    , m_uiWidget(new Ui::SunControlWidget)
    , m_marbleWidget(marbleWidget)
    , m_shadow(QStringLiteral("shadow"))
{
    m_uiWidget->setupUi(this);

    m_uiWidget->lockWarningLabel->hide();

    connect(m_uiWidget->buttonBox->button(QDialogButtonBox::Apply), &QAbstractButton::clicked, this, &SunControlWidget::apply);
    connect(m_uiWidget->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(m_uiWidget->buttonBox->button(QDialogButtonBox::Ok), &QAbstractButton::clicked, this, &SunControlWidget::apply);
    connect(m_uiWidget->buttonBox->button(QDialogButtonBox::Ok), &QAbstractButton::clicked, this, &QDialog::accept);

    setModal(false);
}

SunControlWidget::~SunControlWidget()
{
    delete m_uiWidget;
}

void SunControlWidget::apply()
{
    if (m_uiWidget->sunShading->isChecked()) {
        if (m_uiWidget->showShadow->isChecked()) {
            Q_EMIT showSun(true);
            m_marbleWidget->setShowCityLights(false);
            m_shadow = QStringLiteral("shadow");
        } else if (m_uiWidget->showNightMap->isChecked()) {
            Q_EMIT showSun(true);
            m_marbleWidget->setShowCityLights(true);
            m_shadow = QStringLiteral("nightmap");
        }
    } else {
        Q_EMIT showSun(false);
        m_marbleWidget->setShowCityLights(false);
    }

    if (m_uiWidget->lockToSubSolarPointCheckBox->isChecked()) {
        m_marbleWidget->setLockToSubSolarPoint(true);
        Q_EMIT isLockedToSubSolarPoint(true);
    } else {
        m_marbleWidget->setLockToSubSolarPoint(false);
        Q_EMIT isLockedToSubSolarPoint(false);
    }

    if (m_uiWidget->subSolarIconCheckBox->isChecked()) {
        m_marbleWidget->setSubSolarPointIconVisible(true);
        Q_EMIT isSubSolarPointIconVisible(true);
    } else {
        m_marbleWidget->setSubSolarPointIconVisible(false);
        Q_EMIT isSubSolarPointIconVisible(false);
    }
}

void SunControlWidget::setSunShading(bool active)
{
    m_uiWidget->sunShading->setChecked(active);
}

void SunControlWidget::showEvent(QShowEvent *event)
{
    if (!event->spontaneous()) {
        // Loading all options
        if (m_marbleWidget->showSunShading()) {
            m_uiWidget->sunShading->setChecked(true);
            m_uiWidget->showShadow->setChecked(m_marbleWidget->showSunShading());
            m_uiWidget->showNightMap->setChecked(m_marbleWidget->showCityLights());
        } else {
            m_uiWidget->showShadow->setChecked(false);
            if (m_shadow == QLatin1StringView("shadow")) {
                m_uiWidget->showShadow->setChecked(true);
            } else {
                m_uiWidget->showNightMap->setChecked(true);
            }
        }
        m_uiWidget->subSolarIconCheckBox->setChecked(m_marbleWidget->isSubSolarPointIconVisible());
        m_uiWidget->lockToSubSolarPointCheckBox->setChecked(m_marbleWidget->isLockedToSubSolarPoint());
    }
}

#include "moc_SunControlWidget.cpp"
