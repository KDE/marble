// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2007 Torsten Rahn <rahn@kde.org>
// SPDX-FileCopyrightText: 2008 Jens-Michael Hoffmann <jensmh@gmx.de>
//

#include "MarblePluginSettingsWidget.h"
#include "ui_MarblePluginSettingsWidget.h"

// Qt
#include <QDialog>
#include <QPointer>

// Marble
#include "DialogConfigurationInterface.h"
#include "PluginAboutDialog.h"
#include "PluginItemDelegate.h"
#include "RenderPluginModel.h"

namespace Marble
{

class MarblePluginSettingsWidgetPrivate : public Ui::MarblePluginSettingsWidget
{
public:
    explicit MarblePluginSettingsWidgetPrivate(Marble::MarblePluginSettingsWidget *parent)
        : q(parent)
        , m_itemDelegate(nullptr)
    {
    }

    /**
     * Shows the about dialog for the plugin with the corresponding @p nameId.
     */
    void showPluginAboutDialog(const QModelIndex &index);

    /**
     * Shows the configuration dialog for the plugin with the corresponding @p nameId.
     */
    void showPluginConfigDialog(const QModelIndex &index);

    Marble::MarblePluginSettingsWidget *const q;
    PluginItemDelegate *m_itemDelegate = nullptr;
    QPointer<RenderPluginModel> m_pluginModel;
};

void MarblePluginSettingsWidgetPrivate::showPluginAboutDialog(const QModelIndex &index)
{
    if (m_pluginModel.isNull())
        return;

    QPointer<PluginAboutDialog> aboutDialog = new PluginAboutDialog(q);

    aboutDialog->setName(m_pluginModel->data(index, RenderPluginModel::Name).toString());
    aboutDialog->setIcon(qvariant_cast<QIcon>(m_pluginModel->data(index, RenderPluginModel::Icon)));
    aboutDialog->setVersion(m_pluginModel->data(index, RenderPluginModel::Version).toString());
    aboutDialog->setDataText(m_pluginModel->data(index, RenderPluginModel::AboutDataText).toString());
    const QString copyrightText = QObject::tr("<br/>(c) %1 The Marble Project<br /><br/><a href=\"https://edu.kde.org/marble\">https://edu.kde.org/marble</a>");
    aboutDialog->setAboutText(copyrightText.arg(m_pluginModel->data(index, RenderPluginModel::CopyrightYears).toString()));
    aboutDialog->setAuthors(m_pluginModel->pluginAuthors(index));

    aboutDialog->exec();
    delete aboutDialog;
}

void MarblePluginSettingsWidgetPrivate::showPluginConfigDialog(const QModelIndex &index)
{
    if (m_pluginModel.isNull())
        return;

    DialogConfigurationInterface *configInterface = m_pluginModel->pluginDialogConfigurationInterface(index);
    QDialog *configDialog = configInterface ? configInterface->configDialog() : nullptr;
    if (configDialog) {
        configDialog->show();
    }
}

MarblePluginSettingsWidget::MarblePluginSettingsWidget(QWidget *parent)
    : QWidget(parent)
    , d(new MarblePluginSettingsWidgetPrivate(this))
{
    d->setupUi(this);

    d->m_itemDelegate = new PluginItemDelegate(d->m_pluginListView, this);
    d->m_pluginListView->setItemDelegate(d->m_itemDelegate);
    connect(d->m_itemDelegate, SIGNAL(aboutPluginClicked(QModelIndex)), this, SLOT(showPluginAboutDialog(QModelIndex)));
    connect(d->m_itemDelegate, SIGNAL(configPluginClicked(QModelIndex)), this, SLOT(showPluginConfigDialog(QModelIndex)));
}

void MarblePluginSettingsWidget::setAboutIcon(const QIcon &icon)
{
    d->m_itemDelegate->setAboutIcon(icon);
}

MarblePluginSettingsWidget::~MarblePluginSettingsWidget()
{
    delete d;
}

void MarblePluginSettingsWidget::setConfigIcon(const QIcon &icon)
{
    d->m_itemDelegate->setConfigIcon(icon);
}

void MarblePluginSettingsWidget::setModel(RenderPluginModel *pluginModel)
{
    if (!d->m_pluginModel.isNull()) {
        disconnect(d->m_pluginModel.data(), nullptr, this, nullptr);
    }

    d->m_pluginModel = pluginModel;
    d->m_pluginListView->setModel(pluginModel);

    if (!d->m_pluginModel.isNull()) {
        connect(d->m_pluginModel.data(), SIGNAL(itemChanged(QStandardItem *)), this, SIGNAL(pluginListViewClicked()));
    }
}

} // namespace Marble

#include "moc_MarblePluginSettingsWidget.cpp"
