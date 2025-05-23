// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Rene Kuettner <rene@bitkanal.net>
//

#include "SatellitesConfigDialog.h"

#include <QDateTime>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QUrl>

#include "MarbleDebug.h"
#include "SatellitesConfigLeafItem.h"
#include "SatellitesConfigModel.h"
#include "SatellitesConfigNodeItem.h"

#include "ui_SatellitesConfigDialog.h"

namespace Marble
{

SatellitesConfigDialog::SatellitesConfigDialog(QWidget *parent)
    : QDialog(parent)
{
    initialize();

    // allow translation for catalog items
    // + categories
    m_translations[QStringLiteral("Comets")] = tr("Comets");
    m_translations[QStringLiteral("Moons")] = tr("Moons");
    m_translations[QStringLiteral("Other")] = tr("Other");
    m_translations[QStringLiteral("Spacecrafts")] = tr("Spacecrafts");
    m_translations[QStringLiteral("Spaceprobes")] = tr("Spaceprobes");
    // + bodies
    m_translations[QStringLiteral("Moon")] = tr("Moon");
    m_translations[QStringLiteral("Sun")] = tr("Sun");
    m_translations[QStringLiteral("Mercury")] = tr("Mercury");
    m_translations[QStringLiteral("Venus")] = tr("Venus");
    m_translations[QStringLiteral("Earth")] = tr("Earth");
    m_translations[QStringLiteral("Mars")] = tr("Mars");
    m_translations[QStringLiteral("Jupiter")] = tr("Jupiter");
    m_translations[QStringLiteral("Saturn")] = tr("Saturn");
    m_translations[QStringLiteral("Uranus")] = tr("Uranus");
    m_translations[QStringLiteral("Neptune")] = tr("Neptune");
}

SatellitesConfigDialog::~SatellitesConfigDialog()
{
    delete m_configWidget;
}

void SatellitesConfigDialog::setUserDataSources(const QStringList &sources)
{
    m_userDataSources = sources;

    // keep the first item
    for (int i = m_configWidget->listDataSources->count(); i > 1; --i) {
        delete m_configWidget->listDataSources->takeItem(i - 1);
    }

    m_configWidget->listDataSources->addItems(m_userDataSources);
}

QStringList SatellitesConfigDialog::userDataSources() const
{
    return m_userDataSources;
}

void SatellitesConfigDialog::setUserDataSourceLoaded(const QString &source, bool loaded)
{
    QList<QListWidgetItem *> list;
    list = m_configWidget->listDataSources->findItems(source, Qt::MatchFixedString);
    if (list.count() > 0) {
        list[0]->setData(IsLoadedRole, QVariant(loaded));
    }

    QString date(QDateTime::currentDateTime().toString());
    m_configWidget->labelLastUpdated->setText(date);
}

void SatellitesConfigDialog::update()
{
    expandTreeView();

    QDialog::update();
}

SatellitesConfigAbstractItem *
SatellitesConfigDialog::addSatelliteItem(const QString &body, const QString &category, const QString &title, const QString &id, const QString &url)
{
    QString theTitle = translation(title);

    SatellitesConfigNodeItem *categoryItem;
    categoryItem = getSatellitesCategoryItem(body, category, true);

    // exists?
    for (int i = 0; i < categoryItem->childrenCount(); ++i) {
        SatellitesConfigAbstractItem *absItem = categoryItem->childAt(i);
        if ((absItem->data(0, SatellitesConfigAbstractItem::IdListRole) == id)) {
            return absItem;
        }
    }

    // add it
    SatellitesConfigLeafItem *newItem;
    newItem = new SatellitesConfigLeafItem(theTitle, id);
    if (!url.isNull() && !url.isEmpty()) {
        newItem->setData(0, SatellitesConfigAbstractItem::UrlListRole, url);
    }
    categoryItem->appendChild(newItem);
    return newItem;
}

SatellitesConfigAbstractItem *SatellitesConfigDialog::addTLESatelliteItem(const QString &category, const QString &title, const QString &url)
{
    // TLE items always have their id set to their url and
    // are always related to the earth
    return addSatelliteItem(QStringLiteral("Earth"), category, title, url, url);
}

void SatellitesConfigDialog::setDialogActive(bool active)
{
    m_configWidget->tabWidget->clear();

    if (active) {
        m_configWidget->tabWidget->addTab(m_configWidget->tabSatellites, tr("&Satellites"));
        m_configWidget->tabWidget->addTab(m_configWidget->tabDataSources, tr("&Data Sources"));
    } else {
        m_configWidget->tabWidget->addTab(m_configWidget->tabDisabled, tr("&Activate Plugin"));
    }

    QDialogButtonBox *bBox = m_configWidget->buttonBox;
    bBox->button(QDialogButtonBox::Ok)->setEnabled(active);
    bBox->button(QDialogButtonBox::Reset)->setEnabled(active);
}

Ui::SatellitesConfigDialog *SatellitesConfigDialog::configWidget()
{
    return m_configWidget;
}

void SatellitesConfigDialog::initialize()
{
    m_configWidget = new Ui::SatellitesConfigDialog();
    m_configWidget->setupUi(this);

    setupDataSourcesTab();

    setDialogActive(false);
    connect(m_configWidget->buttonDisabled, SIGNAL(clicked()), this, SIGNAL(activatePluginClicked()));

    update();
}

void SatellitesConfigDialog::setupDataSourcesTab()
{
    connect(m_configWidget->buttonAddDataSource, SIGNAL(clicked()), SLOT(addDataSource()));
    connect(m_configWidget->buttonOpenDataSource, SIGNAL(clicked()), SLOT(openDataSource()));
    connect(m_configWidget->buttonRemoveDataSource, SIGNAL(clicked()), SLOT(removeSelectedDataSource()));
    connect(m_configWidget->buttonReloadDataSources, SIGNAL(clicked()), SLOT(reloadDataSources()));

    connect(m_configWidget->listDataSources, SIGNAL(itemSelectionChanged()), SLOT(updateButtonState()));
}

SatellitesConfigNodeItem *SatellitesConfigDialog::getSatellitesCategoryItem(const QString &body, const QString &category, bool create)
{
    QString theCategory = translation(category);

    SatellitesConfigNodeItem *catalogItem;
    catalogItem = getSatellitesBodyItem(body, create);

    if (catalogItem == nullptr) {
        return nullptr;
    }

    // find category
    for (int i = 0; i < catalogItem->childrenCount(); ++i) {
        if (catalogItem->childAt(i)->name() == theCategory) {
            return dynamic_cast<SatellitesConfigNodeItem *>(catalogItem->childAt(i));
        }
    }

    // not found, create?
    if (create) {
        SatellitesConfigNodeItem *newItem;
        newItem = new SatellitesConfigNodeItem(theCategory);
        catalogItem->appendChild(newItem);
        return newItem;
    }

    return nullptr; // not found, not created
}

SatellitesConfigNodeItem *SatellitesConfigDialog::getSatellitesBodyItem(const QString &body, bool create)
{
    QString theBody = translation(body);

    auto model = dynamic_cast<SatellitesConfigModel *>(m_configWidget->treeView->model());
    SatellitesConfigNodeItem *rootItem = model->rootItem();

    // try to find it
    for (int i = 0; i < rootItem->childrenCount(); ++i) {
        if (rootItem->childAt(i)->name() == theBody) {
            return dynamic_cast<SatellitesConfigNodeItem *>(rootItem->childAt(i));
        }
    }

    // not found, create?
    if (create) {
        SatellitesConfigNodeItem *newItem;
        newItem = new SatellitesConfigNodeItem(theBody);
        rootItem->appendChild(newItem);
        return newItem;
    }

    return nullptr; // not found, not created
}

void SatellitesConfigDialog::reloadDataSources()
{
    Q_EMIT dataSourcesReloadRequested();
}

void SatellitesConfigDialog::addDataSource()
{
    QListWidget *list = m_configWidget->listDataSources;

    bool ok;
    QString text = QInputDialog::getText(this, tr("Add Data Source"), tr("URL or File path:"), QLineEdit::Normal, QString(), &ok);

    if (ok && !text.isEmpty()) {
        QUrl url = QUrl::fromUserInput(text);
        if (!url.isValid()) {
            mDebug() << "Invalid data source input:" << text;
            QMessageBox::critical(this, tr("Invalid data source input"), tr("Please enter a valid URL or file path!"), QMessageBox::Cancel);
            return;
        }

        // add item
        auto item = new QListWidgetItem(url.toString(), list);
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        item->setData(IsLoadedRole, QVariant(false));

        mDebug() << "Added satellite data source:" << item->text();
        m_userDataSources << item->text();

        Q_EMIT userDataSourceAdded(item->text());
        Q_EMIT userDataSourcesChanged();
    }
}

void SatellitesConfigDialog::openDataSource()
{
    QListWidget *list = m_configWidget->listDataSources;

    const QString filter =
        QStringLiteral("%1;;%2;;%3")
            .arg(tr("All Supported Files (*.txt *.msc)"), tr("Marble Satellite Catalog (*.msc)"), tr("Two Line Element Set (*.txt)"), tr("All Files (*.*)"));

    QString filename = QFileDialog::getOpenFileName(this, tr("Open Satellite Data File"), QString(), filter);

    if (!filename.isNull()) {
        QString url = QUrl::fromLocalFile(filename).toString();

        if (!m_configWidget->listDataSources->findItems(url, Qt::MatchFixedString).isEmpty()) {
            mDebug() << "Satellite data source exists:" << url;
            return; // already in list
        }

        auto item = new QListWidgetItem(url, list);
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        item->setData(IsLoadedRole, QVariant(false));

        mDebug() << "Added satellite data source:" << url;
        m_userDataSources << url;

        Q_EMIT userDataSourceAdded(url);
        Q_EMIT userDataSourcesChanged();
    }
}

void SatellitesConfigDialog::removeSelectedDataSource()
{
    int row = m_configWidget->listDataSources->currentRow();
    if (row >= 0
        && QMessageBox::question(this,
                                 tr("Delete selected data source"),
                                 tr("Do you really want to delete the selected data source?"),
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::No)
            == QMessageBox::Yes) {
        QListWidgetItem *item;
        item = m_configWidget->listDataSources->takeItem(row);
        QString source = item->text();

        mDebug() << "Removing satellite data source:" << source;
        m_userDataSources.removeAll(source);
        Q_EMIT userDataSourceRemoved(source);

        delete item;

        Q_EMIT userDataSourcesChanged();
    }
}

void SatellitesConfigDialog::updateButtonState()
{
    m_configWidget->buttonRemoveDataSource->setEnabled((m_configWidget->listDataSources->currentIndex().row() >= 0));
}

void SatellitesConfigDialog::expandTreeView()
{
    QTreeView *treeView = m_configWidget->treeView;

    if (!treeView->model()) {
        return;
    }

    // expand only branches with selected items
    treeView->expandAll();

    for (int i = 0; i < treeView->model()->columnCount(); ++i) {
        treeView->resizeColumnToContents(i);
    }
}

QString SatellitesConfigDialog::translation(const QString &from) const
{
    if (m_translations.contains(from)) {
        return m_translations.value(from);
    }

    return from;
}

} // namespace Marble

#include "moc_SatellitesConfigDialog.cpp"
