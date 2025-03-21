// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
// SPDX-FileCopyrightText: 2012 Mohammed Nafees <nafees.technocool@gmail.com>
//

// Self
#include "PhotoPlugin.h"

#include "PhotoPluginModel.h"

// Marble
#include "MarbleDebug.h"
#include "MarbleModel.h"
#include "MarbleWidget.h"
#include "ui_PhotoConfigWidget.h"

// Qt
#include <QPushButton>
#include <QStringList>

using namespace Marble;
/* TRANSLATOR Marble::PhotoPlugin */

const quint32 maximumNumberOfItems = 99;

PhotoPlugin::PhotoPlugin()
    : AbstractDataPlugin(nullptr)
{
}

PhotoPlugin::PhotoPlugin(const MarbleModel *marbleModel)
    : AbstractDataPlugin(marbleModel)
{
    // Plugin is enabled by default
    setEnabled(true);
    // Plugin is not visible by default
    setVisible(false);

    connect(this, &RenderPlugin::settingsChanged, this, &PhotoPlugin::updateSettings);
    connect(this, &AbstractDataPlugin::changedNumberOfItems, this, &PhotoPlugin::checkNumberOfItems);

    setSettings(QHash<QString, QVariant>());
}

PhotoPlugin::~PhotoPlugin()
{
    delete ui_configWidget;
    delete m_configDialog;
}

void PhotoPlugin::initialize()
{
    mDebug() << "PhotoPlugin: Initialize";
    auto model = new PhotoPluginModel(marbleModel(), this);
    setModel(model);
    updateSettings();
}

QString PhotoPlugin::name() const
{
    return tr("Photos");
}

QString PhotoPlugin::guiString() const
{
    return tr("&Photos");
}

QString PhotoPlugin::nameId() const
{
    return QStringLiteral("photo");
}

QString PhotoPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString PhotoPlugin::description() const
{
    return tr("Automatically downloads images from around the world in preference to their popularity");
}

QString PhotoPlugin::copyrightYears() const
{
    return QStringLiteral("2009, 2012");
}

QList<PluginAuthor> PhotoPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>() << PluginAuthor(QStringLiteral("Bastian Holst"), QStringLiteral("bastianholst@gmx.de"))
                                 << PluginAuthor(QStringLiteral("Mohammed Nafees"), QStringLiteral("nafees.technocool@gmail.com"));
}

QIcon PhotoPlugin::icon() const
{
    return QIcon(QStringLiteral(":/icons/photo.png"));
}

QDialog *PhotoPlugin::configDialog()
{
    if (!m_configDialog) {
        // Initializing configuration dialog
        m_configDialog = new QDialog();
        ui_configWidget = new Ui::PhotoConfigWidget;
        ui_configWidget->setupUi(m_configDialog);

        // add licenses to the list widget
        auto ccByNcSa2 = new QListWidgetItem(ui_configWidget->m_licenseListWidget);
        ccByNcSa2->setText(tr("Attribution-NonCommercial-ShareAlike License"));
        ccByNcSa2->setIcon(QIcon());
        ccByNcSa2->setCheckState(Qt::Checked);
        ccByNcSa2->setData(Qt::UserRole + 1, 1);
        ccByNcSa2->setData(Qt::UserRole + 2, QStringLiteral("http://creativecommons.org/licenses/by-nc-sa/2.0/"));
        ui_configWidget->m_licenseListWidget->addItem(ccByNcSa2);
        auto ccByNc2 = new QListWidgetItem(ui_configWidget->m_licenseListWidget);
        ccByNc2->setText(tr("Attribution-NonCommercial License"));
        ccByNc2->setIcon(QIcon());
        ccByNc2->setCheckState(Qt::Checked);
        ccByNc2->setData(Qt::UserRole + 1, 2);
        ccByNc2->setData(Qt::UserRole + 2, QStringLiteral("http://creativecommons.org/licenses/by-nc/2.0/"));
        ui_configWidget->m_licenseListWidget->addItem(ccByNc2);
        auto ccByNcNd2 = new QListWidgetItem(ui_configWidget->m_licenseListWidget);
        ccByNcNd2->setText(tr("Attribution-NonCommercial-NoDerivs License"));
        ccByNcNd2->setIcon(QIcon());
        ccByNcNd2->setCheckState(Qt::Checked);
        ccByNcNd2->setData(Qt::UserRole + 1, 3);
        ccByNcNd2->setData(Qt::UserRole + 2, QStringLiteral("http://creativecommons.org/licenses/by-nc-nd/2.0/"));
        ui_configWidget->m_licenseListWidget->addItem(ccByNcNd2);
        auto ccBy2 = new QListWidgetItem(ui_configWidget->m_licenseListWidget);
        ccBy2->setText(tr("Attribution License"));
        ccBy2->setIcon(QIcon());
        ccBy2->setCheckState(Qt::Checked);
        ccBy2->setData(Qt::UserRole + 1, 4);
        ccBy2->setData(Qt::UserRole + 2, QStringLiteral("http://creativecommons.org/licenses/by/2.0/"));
        ui_configWidget->m_licenseListWidget->addItem(ccBy2);
        auto ccBySa2 = new QListWidgetItem(ui_configWidget->m_licenseListWidget);
        ccBySa2->setText(tr("Attribution-ShareAlike License"));
        ccBySa2->setIcon(QIcon());
        ccBySa2->setCheckState(Qt::Checked);
        ccBySa2->setData(Qt::UserRole + 1, 5);
        ccBySa2->setData(Qt::UserRole + 2, QStringLiteral("http://creativecommons.org/licenses/by-sa/2.0/"));
        ui_configWidget->m_licenseListWidget->addItem(ccBySa2);
        auto ccByNd2 = new QListWidgetItem(ui_configWidget->m_licenseListWidget);
        ccByNd2->setText(tr("Attribution-NoDerivs License"));
        ccByNd2->setIcon(QIcon());
        ccByNd2->setCheckState(Qt::Checked);
        ccByNd2->setData(Qt::UserRole + 1, 6);
        ccByNd2->setData(Qt::UserRole + 2, QStringLiteral("http://creativecommons.org/licenses/by-nd/2.0/"));
        ui_configWidget->m_licenseListWidget->addItem(ccByNd2);
        auto noLicense = new QListWidgetItem(ui_configWidget->m_licenseListWidget);
        noLicense->setText(tr("No known copyright restrictions"));
        noLicense->setIcon(QIcon());
        noLicense->setCheckState(Qt::Checked);
        noLicense->setData(Qt::UserRole + 1, 7);
        noLicense->setData(Qt::UserRole + 2, QStringLiteral("http://flickr.com/commons/usage/"));
        ui_configWidget->m_licenseListWidget->addItem(noLicense);

        readSettings();
        ui_configWidget->m_itemNumberSpinBox->setRange(0, maximumNumberOfItems);
        connect(ui_configWidget->m_buttonBox, &QDialogButtonBox::accepted, this, &PhotoPlugin::writeSettings);
        connect(ui_configWidget->m_buttonBox, &QDialogButtonBox::rejected, this, &PhotoPlugin::readSettings);
        QPushButton *applyButton = ui_configWidget->m_buttonBox->button(QDialogButtonBox::Apply);
        connect(applyButton, SIGNAL(clicked()), this, SLOT(writeSettings()));
    }
    return m_configDialog;
}

QHash<QString, QVariant> PhotoPlugin::settings() const
{
    QHash<QString, QVariant> settings = AbstractDataPlugin::settings();

    settings.insert(QStringLiteral("numberOfItems"), numberOfItems());
    settings.insert(QStringLiteral("checkState"), m_checkStateList.join(QLatin1Char(',')));

    return settings;
}

void PhotoPlugin::setSettings(const QHash<QString, QVariant> &settings)
{
    AbstractDataPlugin::setSettings(settings);

    setNumberOfItems(qMin<int>(maximumNumberOfItems, settings.value(QStringLiteral("numberOfItems"), 15).toInt()));
    m_checkStateList = settings.value(QStringLiteral("checkState"), QStringLiteral("1,2,3,4,5,6,7")).toString().split(QLatin1Char(','), Qt::SkipEmptyParts);

    updateSettings();
    readSettings();
}

bool PhotoPlugin::eventFilter(QObject *object, QEvent *event)
{
    if (isInitialized()) {
        auto photoPluginModel = dynamic_cast<PhotoPluginModel *>(model());
        Q_ASSERT(photoPluginModel);
        auto widget = dynamic_cast<MarbleWidget *>(object);
        if (widget) {
            photoPluginModel->setMarbleWidget(widget);
        }
    }

    return AbstractDataPlugin::eventFilter(object, event);
}

void PhotoPlugin::readSettings()
{
    if (!m_configDialog)
        return;

    ui_configWidget->m_itemNumberSpinBox->setValue(numberOfItems());
    for (int i = 0; i < ui_configWidget->m_licenseListWidget->count(); ++i) {
        const QString licenseId = QString::number(ui_configWidget->m_licenseListWidget->item(i)->data(Qt::UserRole + 1).toInt());
        ui_configWidget->m_licenseListWidget->item(i)->setCheckState(m_checkStateList.contains(licenseId) ? Qt::Checked : Qt::Unchecked);
    }
}

void PhotoPlugin::writeSettings()
{
    setNumberOfItems(ui_configWidget->m_itemNumberSpinBox->value());

    QStringList licenseCheckStateList;
    for (int i = 0; i < ui_configWidget->m_licenseListWidget->count(); ++i) {
        if (ui_configWidget->m_licenseListWidget->item(i)->checkState() == Qt::Checked) {
            licenseCheckStateList << ui_configWidget->m_licenseListWidget->item(i)->data(Qt::UserRole + 1).toString();
        }
    }
    m_checkStateList = licenseCheckStateList;

    Q_EMIT settingsChanged(nameId());
}

void PhotoPlugin::updateSettings()
{
    AbstractDataPluginModel *abstractModel = model();
    if (abstractModel != nullptr) {
        abstractModel->setItemSettings(settings());
    }

    if (model()) {
        qobject_cast<PhotoPluginModel *>(model())->setLicenseValues(m_checkStateList.join(QLatin1Char(',')));
    }
}

void PhotoPlugin::checkNumberOfItems(quint32 number)
{
    if (number > maximumNumberOfItems) {
        setNumberOfItems(maximumNumberOfItems);
    }

    readSettings();
}

#include "moc_PhotoPlugin.cpp"
