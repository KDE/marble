// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Utku Aydın <utkuaydin34@gmail.com>
// SPDX-FileCopyrightText: 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "OpenDesktopPlugin.h"
#include "MarbleWidget.h"
#include "OpenDesktopModel.h"

#include "ui_OpenDesktopConfigWidget.h"

#include <QPushButton>

using namespace Marble;

OpenDesktopPlugin::OpenDesktopPlugin()
    : AbstractDataPlugin(nullptr)
    , m_configDialog(nullptr)
    , m_uiConfigWidget(nullptr)
{
}

OpenDesktopPlugin::OpenDesktopPlugin(const MarbleModel *marbleModel)
    : AbstractDataPlugin(marbleModel)
    , m_configDialog(nullptr)
    , m_uiConfigWidget(nullptr)
{
    setEnabled(true); // Plugin is enabled by default
    setVisible(false); // Plugin is invisible by default
}

OpenDesktopPlugin::~OpenDesktopPlugin()
{
    delete m_uiConfigWidget;
}

void OpenDesktopPlugin::initialize()
{
    setModel(new OpenDesktopModel(marbleModel(), this));
    setNumberOfItems(defaultItemsOnScreen); // Setting the number of items on the screen.
}

QString OpenDesktopPlugin::name() const
{
    return tr("OpenDesktop Items");
}

QString OpenDesktopPlugin::guiString() const
{
    return tr("&OpenDesktop Community");
}

QString OpenDesktopPlugin::nameId() const
{
    return QStringLiteral("opendesktop");
}

QString OpenDesktopPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString OpenDesktopPlugin::description() const
{
    return tr("Shows OpenDesktop users' avatars and some extra information about them on the map.");
}

QString OpenDesktopPlugin::copyrightYears() const
{
    return QStringLiteral("2010");
}

QList<PluginAuthor> OpenDesktopPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>() << PluginAuthor(QStringLiteral("Utku Aydin"), QStringLiteral("utkuaydin34@gmail.com"));
}

QIcon OpenDesktopPlugin::icon() const
{
    return QIcon(QStringLiteral(":/icons/social.png"));
}

QDialog *OpenDesktopPlugin::configDialog()
{
    if (!m_configDialog) {
        m_configDialog = new QDialog();
        m_uiConfigWidget = new Ui::OpenDesktopConfigWidget;
        m_uiConfigWidget->setupUi(m_configDialog);
        readSettings();

        connect(m_uiConfigWidget->m_buttonBox, &QDialogButtonBox::accepted, this, &OpenDesktopPlugin::writeSettings);
        connect(m_uiConfigWidget->m_buttonBox, &QDialogButtonBox::rejected, this, &OpenDesktopPlugin::readSettings);
        QPushButton *applyButton = m_uiConfigWidget->m_buttonBox->button(QDialogButtonBox::Apply);
        connect(applyButton, &QAbstractButton::clicked, this, &OpenDesktopPlugin::writeSettings);
    }

    return m_configDialog;
}

QHash<QString, QVariant> OpenDesktopPlugin::settings() const
{
    QHash<QString, QVariant> settings = AbstractDataPlugin::settings();

    settings.insert(QStringLiteral("itemsOnScreen"), numberOfItems());

    return settings;
}

bool OpenDesktopPlugin::eventFilter(QObject *object, QEvent *event)
{
    if (isInitialized()) {
        auto odModel = qobject_cast<OpenDesktopModel *>(model());
        Q_ASSERT(odModel);
        auto widget = qobject_cast<MarbleWidget *>(object);
        if (widget) {
            odModel->setMarbleWidget(widget);
        }
    }

    return AbstractDataPlugin::eventFilter(object, event);
}

void OpenDesktopPlugin::setSettings(const QHash<QString, QVariant> &settings)
{
    AbstractDataPlugin::setSettings(settings);

    setNumberOfItems(settings.value(QStringLiteral("itemsOnScreen"), defaultItemsOnScreen).toInt());

    Q_EMIT settingsChanged(nameId());
}

void OpenDesktopPlugin::readSettings()
{
    if (m_uiConfigWidget) {
        m_uiConfigWidget->m_itemsOnScreenSpin->setValue(numberOfItems());
    }
}

void OpenDesktopPlugin::writeSettings()
{
    if (m_uiConfigWidget) {
        setNumberOfItems(m_uiConfigWidget->m_itemsOnScreenSpin->value());
    }

    Q_EMIT settingsChanged(nameId());
}

#include "moc_OpenDesktopPlugin.cpp"
