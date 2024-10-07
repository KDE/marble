// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "WikipediaPlugin.h"

// Marble
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleWidget.h"
#include "WikipediaModel.h"
#include "ui_WikipediaConfigWidget.h"

// Qt
#include <QPushButton>

using namespace Marble;
/* TRANSLATOR Marble::WikipediaPlugin */

const quint32 maximumNumberOfItems = 99;

WikipediaPlugin::WikipediaPlugin()
    : AbstractDataPlugin(nullptr)
    , ui_configWidget(nullptr)
    , m_configDialog(nullptr)
{
}

WikipediaPlugin::WikipediaPlugin(const MarbleModel *marbleModel)
    : AbstractDataPlugin(marbleModel)
    , m_icon(MarbleDirs::path(QStringLiteral("svg/wikipedia_shadow.svg")))
    , ui_configWidget(nullptr)
    , m_configDialog(nullptr)
    , m_showThumbnails(true)
{
    // Plugin is enabled by default
    setEnabled(true);
    // Plugin is not visible by default
    setVisible(false);

    connect(this, &RenderPlugin::settingsChanged, this, &WikipediaPlugin::updateSettings);
    connect(this, &AbstractDataPlugin::changedNumberOfItems, this, &WikipediaPlugin::checkNumberOfItems);

    setSettings(QHash<QString, QVariant>());
}

WikipediaPlugin::~WikipediaPlugin()
{
    delete ui_configWidget;
    delete m_configDialog;
}

void WikipediaPlugin::initialize()
{
    auto model = new WikipediaModel(marbleModel(), this);
    // Ensure that all settings get forwarded to the model.
    setModel(model);
    updateSettings();
}

QString WikipediaPlugin::name() const
{
    return tr("Wikipedia Articles");
}

QString WikipediaPlugin::guiString() const
{
    return tr("&Wikipedia");
}

QString WikipediaPlugin::nameId() const
{
    return QStringLiteral("wikipedia");
}

QString WikipediaPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString WikipediaPlugin::description() const
{
    return tr("Automatically downloads Wikipedia articles and shows them on the right position on the map");
}

QString WikipediaPlugin::copyrightYears() const
{
    return QStringLiteral("2009");
}

QList<PluginAuthor> WikipediaPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>() << PluginAuthor(QStringLiteral("Bastian Holst"), QStringLiteral("bastianholst@gmx.de"));
}

QString WikipediaPlugin::aboutDataText() const
{
    return tr("Geo positions by geonames.org\nTexts by wikipedia.org");
}

QIcon WikipediaPlugin::icon() const
{
    return m_icon;
}

QDialog *WikipediaPlugin::configDialog()
{
    if (!m_configDialog) {
        // Initializing configuration dialog
        m_configDialog = new QDialog();
        ui_configWidget = new Ui::WikipediaConfigWidget;
        ui_configWidget->setupUi(m_configDialog);
        readSettings();
        ui_configWidget->m_itemNumberSpinBox->setRange(0, maximumNumberOfItems);
        connect(ui_configWidget->m_buttonBox, &QDialogButtonBox::accepted, this, &WikipediaPlugin::writeSettings);
        connect(ui_configWidget->m_buttonBox, &QDialogButtonBox::rejected, this, &WikipediaPlugin::readSettings);
        QPushButton *applyButton = ui_configWidget->m_buttonBox->button(QDialogButtonBox::Apply);
        connect(applyButton, &QAbstractButton::clicked, this, &WikipediaPlugin::writeSettings);
    }
    return m_configDialog;
}

QHash<QString, QVariant> WikipediaPlugin::settings() const
{
    QHash<QString, QVariant> settings = AbstractDataPlugin::settings();

    settings.insert(QStringLiteral("numberOfItems"), numberOfItems());
    settings.insert(QStringLiteral("showThumbnails"), m_showThumbnails);

    return settings;
}

void WikipediaPlugin::setSettings(const QHash<QString, QVariant> &settings)
{
    AbstractDataPlugin::setSettings(settings);

    setNumberOfItems(qMin<int>(maximumNumberOfItems, settings.value(QStringLiteral("numberOfItems"), 15).toInt()));
    m_showThumbnails = settings.value(QStringLiteral("showThumbnails"), true).toBool();

    readSettings();
    Q_EMIT settingsChanged(nameId());
}

bool WikipediaPlugin::eventFilter(QObject *object, QEvent *event)
{
    if (isInitialized()) {
        auto wikipediaModel = dynamic_cast<WikipediaModel *>(model());
        Q_ASSERT(wikipediaModel);
        auto widget = dynamic_cast<MarbleWidget *>(object);
        if (widget) {
            wikipediaModel->setMarbleWidget(widget);
        }
    }

    return AbstractDataPlugin::eventFilter(object, event);
}

void WikipediaPlugin::readSettings()
{
    if (!m_configDialog)
        return;

    ui_configWidget->m_itemNumberSpinBox->setValue(numberOfItems());
    ui_configWidget->m_showThumbnailCheckBox->setChecked(m_showThumbnails);
}

void WikipediaPlugin::writeSettings()
{
    setNumberOfItems(ui_configWidget->m_itemNumberSpinBox->value());
    m_showThumbnails = ui_configWidget->m_showThumbnailCheckBox->isChecked();

    Q_EMIT settingsChanged(nameId());
}

void WikipediaPlugin::updateSettings()
{
    AbstractDataPluginModel *abstractModel = model();
    if (abstractModel != nullptr) {
        abstractModel->setItemSettings(settings());
    }

    auto wikipediaModel = qobject_cast<WikipediaModel *>(model());
    if (wikipediaModel) {
        wikipediaModel->setShowThumbnail(m_showThumbnails);
    }
}

void WikipediaPlugin::checkNumberOfItems(quint32 number)
{
    if (number > maximumNumberOfItems) {
        setNumberOfItems(maximumNumberOfItems);
    }

    readSettings();
}

#include "moc_WikipediaPlugin.cpp"
