//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Rene Kuettner <rene@bitkanal.net>
//

#include "SatellitesConfigDialog.h"

#include <QPushButton>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QDateTime>
#include <QUrl>

#include "MarbleGlobal.h"
#include "MarbleDebug.h"
#include "SatellitesConfigNodeItem.h"
#include "SatellitesConfigLeafItem.h"
#include "SatellitesConfigModel.h"

#include "ui_SatellitesConfigDialog.h"

namespace Marble {

SatellitesConfigDialog::SatellitesConfigDialog( QWidget *parent )
    : QDialog( parent )
{
    initialize();

    // allow translation for catalog items
    // + categories
    const char *descCat = "An object category";
    m_translations["Comets"]        = tr( "Comets", descCat );
    m_translations["Moons"]         = tr( "Moons", descCat );
    m_translations["Other"]         = tr( "Other", descCat );
    m_translations["Spacecrafts"]   = tr( "Spacecrafts", descCat );
    m_translations["Space probes"]  = tr( "Space probes", descCat );
    // + bodies
    const char *descBodies = "A planet or space body";
    m_translations["Moon"]          = tr( "Moon", descBodies );
    m_translations["Sun"]           = tr( "Sun", descBodies );
    m_translations["Mercury"]       = tr( "Mercury", descBodies );
    m_translations["Venus"]         = tr( "Venus", descBodies );
    m_translations["Earth"]         = tr( "Earth", descBodies );
    m_translations["Mars"]          = tr( "Mars", descBodies );
    m_translations["Jupiter"]       = tr( "Jupiter", descBodies );
    m_translations["Saturn"]        = tr( "Saturn", descBodies );
    m_translations["Uranus"]        = tr( "Uranus", descBodies );
    m_translations["Neptune"]       = tr( "Neptune", descBodies );
}

SatellitesConfigDialog::~SatellitesConfigDialog()
{
}

void SatellitesConfigDialog::setUserDataSources( const QStringList &sources )
{
    m_userDataSources = sources;

    // keep the first item
    for( int i = m_configWidget->listDataSources->count(); i > 1; --i ) {
        delete m_configWidget->listDataSources->takeItem( i-1 );
    }

    m_configWidget->listDataSources->addItems( m_userDataSources );
}

QStringList SatellitesConfigDialog::userDataSources() const
{
    return m_userDataSources;
}

void SatellitesConfigDialog::setUserDataSourceLoaded( const QString &source,
                                                      bool loaded )
{
    QList<QListWidgetItem*> list;
    list = m_configWidget->listDataSources->findItems( source,
                                                       Qt::MatchFixedString );
    if( list.count() > 0 ) {
        list[0]->setData( IsLoadedRole, QVariant( loaded ) );
    }

    QString date( QDateTime::currentDateTime().toString() );
    m_configWidget->labelLastUpdated->setText( date );
}

void SatellitesConfigDialog::update()
{
    expandTreeView();

    QDialog::update();
}

SatellitesConfigAbstractItem*
SatellitesConfigDialog::addSatelliteItem( const QString &body,
                                          const QString &category,
                                          const QString &title,
                                          const QString &id,
                                          const QString &url )
{
    QString theTitle = translation( title );

    SatellitesConfigNodeItem *categoryItem;
    categoryItem = getSatellitesCategoryItem( body, category, true );

    // exists?
    for( int i = 0; i < categoryItem->childrenCount(); ++i ) {
        SatellitesConfigAbstractItem *absItem = categoryItem->childAt( i );
        if( ( absItem->data( 0, SatellitesConfigAbstractItem::IdListRole ) == id ) ) {
            return absItem;
        }
    }

    // add it
    SatellitesConfigLeafItem *newItem;
    newItem = new SatellitesConfigLeafItem( theTitle, id );
    if( !url.isNull() && !url.isEmpty() ) {
        newItem->setData( 0, SatellitesConfigAbstractItem::UrlListRole, url );
    }
    categoryItem->appendChild( newItem );
    return newItem;
}

SatellitesConfigAbstractItem*
SatellitesConfigDialog::addTLESatelliteItem( const QString &category,
                                             const QString &title,
                                             const QString &url )
{
    // TLE items always have their id set to their url and
    // are always related to the earth
    return addSatelliteItem( "Earth", category, title, url, url );
}

Ui::SatellitesConfigDialog* SatellitesConfigDialog::configWidget()
{
    return m_configWidget;
}

void SatellitesConfigDialog::initialize()
{
    m_configWidget = new Ui::SatellitesConfigDialog();
    m_configWidget->setupUi( this );

    setupSatellitesTab();
    setupDataSourcesTab();

    update();
}

void SatellitesConfigDialog::setupSatellitesTab()
{
}

void SatellitesConfigDialog::setupDataSourcesTab()
{
    connect( m_configWidget->buttonAddDataSource,
        SIGNAL( clicked() ), SLOT( addDataSource() ) );
    connect( m_configWidget->buttonOpenDataSource,
        SIGNAL( clicked() ), SLOT( openDataSource() ) );
    connect( m_configWidget->buttonRemoveDataSource,
        SIGNAL( clicked() ), SLOT( removeSelectedDataSource() ) );
    connect( m_configWidget->buttonReloadDataSources,
        SIGNAL( clicked() ), SLOT( reloadDataSources() ) );

    connect( m_configWidget->listDataSources,
        SIGNAL( itemSelectionChanged() ), SLOT( updateButtonState() ) );
}

SatellitesConfigNodeItem* SatellitesConfigDialog::getSatellitesCategoryItem(
    const QString &body,
    const QString &category,
    bool create )
{
    QString theCategory = translation( category );

    SatellitesConfigNodeItem *catalogItem;
    catalogItem = getSatellitesBodyItem( body, create );

    if( catalogItem == NULL ) {
        return NULL;
    }

    // find category
    for( int i = 0; i < catalogItem->childrenCount(); ++i ) {
        if( catalogItem->childAt( i )->name() == theCategory ) {
            return (SatellitesConfigNodeItem*)catalogItem->childAt( i );
        }
    }

    // not found, create?
    if( create ) {
        SatellitesConfigNodeItem *newItem;
        newItem = new SatellitesConfigNodeItem( theCategory );
        catalogItem->appendChild( newItem );
        return newItem;
    }

    return NULL; // not found, not created
}

SatellitesConfigNodeItem* SatellitesConfigDialog::getSatellitesBodyItem(
    const QString &body,
    bool create )
{
    QString theBody = translation( body );

    SatellitesConfigModel *model;
    model = (SatellitesConfigModel*)m_configWidget->treeView->model();
    SatellitesConfigNodeItem *rootItem = model->rootItem();

    // try to find it
    for( int i = 0; i < rootItem->childrenCount(); ++i ) {
        if( rootItem->childAt( i )->name() == theBody ) {
            return (SatellitesConfigNodeItem*)rootItem->childAt( i ); 
        }
    }

    // not found, create?
    if( create ) {
        SatellitesConfigNodeItem *newItem;
        newItem = new SatellitesConfigNodeItem( theBody );
        rootItem->appendChild( newItem );
        return newItem;
    }

    return NULL; // not found, not created
}

void SatellitesConfigDialog::reloadDataSources()
{
    emit dataSourcesReloadRequested();
}

void SatellitesConfigDialog::addDataSource()
{
    QListWidget *list = m_configWidget->listDataSources;

    bool ok;
    QString text = QInputDialog::getText( this,
                                          tr("Add Data Source"),
                                          tr("URL or File path:"),
                                          QLineEdit::Normal,
                                          "", &ok);

    if( ok && !text.isEmpty() ) {
        QUrl url = QUrl::fromUserInput( text );
        if( !url.isValid() ) {
            mDebug() << "Invalid data source input:" << text;
            QMessageBox::critical( this,
                                   tr( "Invalid data source input" ),
                                   tr( "Please enter a valid URL or file path!" ),
                                   QMessageBox::Cancel );
            return;
        }

        // add item
        QListWidgetItem *item = new QListWidgetItem( url.toString(), list );
        item->setFlags( Qt::ItemIsSelectable |
                        Qt::ItemIsEnabled );
        item->setData( IsLoadedRole, QVariant( false ) );

        mDebug() << "Added satellite data source:" << item->text();
        m_userDataSources << item->text();

        emit userDataSourceAdded( item->text() );
        emit userDataSourcesChanged();
    }
}

void SatellitesConfigDialog::openDataSource()
{
    QListWidget *list = m_configWidget->listDataSources;

    const QString filter = QString("%1;;%2;;%3").arg(
        tr( "Marble Satellite Catalogue (*.msc)" ),
        tr( "Two Line Element Set (*.txt)" ),
        tr( "All Files (*.*)" ) );

    QString filename = QFileDialog::getOpenFileName( this,
        tr( "Open Satellite Data File" ), "", filter );

    if( !filename.isNull() ) {
        QString url = QUrl::fromLocalFile( filename ).toString();

        if( m_configWidget->listDataSources->findItems(
                url, Qt::MatchFixedString ).size() > 0 ) {
            mDebug() << "Satellite data source exists:" << url;
            return; // already in list
        }

        QListWidgetItem *item = new QListWidgetItem( url, list );
        item->setFlags( Qt::ItemIsSelectable |
                        Qt::ItemIsEnabled );
        item->setData( IsLoadedRole, QVariant( false ) );

        mDebug() << "Added satellite data source:" << url;
        m_userDataSources << url;

        emit userDataSourceAdded( url );
        emit userDataSourcesChanged();
    }

}

void SatellitesConfigDialog::removeSelectedDataSource()
{
    int row = m_configWidget->listDataSources->currentRow();
    if( row >= 0 && 
        QMessageBox::question( this,
            tr( "Delete selected data source" ),
            tr( "Do you really want to delete the selected data source?" ),
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No ) == QMessageBox::Yes ) {

        QListWidgetItem *item;
        item = m_configWidget->listDataSources->takeItem( row );
        QString source = item->text();

        mDebug() << "Removing satellite data source:" << source;
        m_userDataSources.removeAll( source );
        emit userDataSourceRemoved( source );

        delete item;

        emit userDataSourcesChanged();
    }
}

void SatellitesConfigDialog::updateButtonState()
{
    m_configWidget->buttonRemoveDataSource->setEnabled(
        ( m_configWidget->listDataSources->currentIndex().row() >= 0 ) );
}

void SatellitesConfigDialog::expandTreeView()
{
    QTreeView *treeView = m_configWidget->treeView;

    if( !treeView->model() ) {
        return;
    }

    // expand only branches with selected items
    treeView->expandAll();

    for ( int i = 0; i < treeView->model()->columnCount(); ++i ) {
        treeView->resizeColumnToContents( i );
    }
}

QString SatellitesConfigDialog::translation( const QString &from ) const
{
    if( m_translations.contains( from ) ) {
        return m_translations.value( from );
    }

    return from;
}

} // namespace Marble

#include "SatellitesConfigDialog.moc"

