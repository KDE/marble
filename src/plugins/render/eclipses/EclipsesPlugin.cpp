//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Rene Kuettner <rene@bitkanal.net>
//

#include "EclipsesPlugin.h"

#include "MarbleWidget.h"
#include "MarbleDebug.h"
#include "MarbleModel.h"
#include "MarbleClock.h"
#include "ViewportParams.h"

#include "EclipsesModel.h"
#include "EclipsesItem.h"

#include "ui_EclipsesConfigDialog.h"
#include "ui_EclipsesListDialog.h"
#include "ui_EclipsesReminderDialog.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>

namespace Marble
{

EclipsesPlugin::EclipsesPlugin()
    : RenderPlugin( 0 ),
      m_isInitialized( false ),
      m_marbleWidget( 0 ),
      m_clock( 0 ),
      m_model( 0 ),
      m_eclipsesActionGroup( 0 ),
      m_eclipsesMenuAction( 0 ),
      m_eclipsesListMenu( 0 ),
      m_configDialog( 0 ),
      m_configWidget( 0 ),
      m_listDialog( 0 ),
      m_listWidget( 0 ),
      m_reminderDialog( 0 ),
      m_reminderWidget( 0 )
{
}

EclipsesPlugin::EclipsesPlugin( const MarbleModel *marbleModel )
    : RenderPlugin( marbleModel ),
     m_isInitialized( false ),
     m_marbleWidget( 0 ),
     m_clock( 0 ),
     m_model( 0 ),
     m_eclipsesActionGroup( 0 ),
     m_eclipsesMenuAction( 0 ),
     m_eclipsesListMenu( 0 ),
     m_configDialog( 0 ),
     m_configWidget( 0 ),
     m_listDialog( 0 ),
     m_listWidget( 0 ),
     m_reminderDialog( 0 ),
     m_reminderWidget( 0 )
{
    connect( this, SIGNAL(settingsChanged(QString)),
                   SLOT(updateSettings()) );

    setSettings( QHash<QString, QVariant>() );
    setEnabled( true );
}

EclipsesPlugin::~EclipsesPlugin()
{
    if( m_isInitialized ) {
        delete m_model;
        delete m_eclipsesActionGroup;
        delete m_eclipsesListMenu;
        delete m_configDialog;
        delete m_listDialog;
        delete m_reminderDialog;
    }
}

QStringList EclipsesPlugin::backendTypes() const
{
    return QStringList( "eclipses" );
}

QString EclipsesPlugin::renderPolicy() const
{
    return QString( "ALWAYS" );
}

QStringList EclipsesPlugin::renderPosition() const
{
    return QStringList( "ORBIT" );
}

QString EclipsesPlugin::name() const
{
    return tr( "Eclipses" );
}

QString EclipsesPlugin::nameId() const
{
    return "eclipses";
}

QString EclipsesPlugin::guiString() const
{
    return tr( "E&clipses" );
}

QString EclipsesPlugin::version() const
{
    return "1.0";
}

QString EclipsesPlugin::description() const
{
    return tr( "This plugin visualizes solar eclipses." );
}

QString EclipsesPlugin::copyrightYears() const
{
    return "2013";
}

QList<PluginAuthor> EclipsesPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( "Rene Kuettner", "rene@bitkanal.net" )
            << PluginAuthor( "Gerhard Holtkamp", "" );
}

QIcon EclipsesPlugin::icon() const
{
    return QIcon( ":res/eclipses.png" );
}

RenderPlugin::RenderType EclipsesPlugin::renderType() const
{
    return Unknown;
}

QList<QActionGroup*>* EclipsesPlugin::actionGroups() const
{
    return const_cast<QList<QActionGroup*>*>( &m_actionGroups );
}

QDialog* EclipsesPlugin::configDialog()
{
    Q_ASSERT( m_isInitialized );
    return m_configDialog;
}

void EclipsesPlugin::initialize()
{
    if( isInitialized() ) {
        return;
    }

    // initialize dialogs
    m_configDialog = new QDialog();
    m_configWidget = new Ui::EclipsesConfigDialog();
    m_configWidget->setupUi( m_configDialog );

    m_listDialog = new QDialog();
    m_listWidget = new Ui::EclipsesListDialog();
    m_listWidget->setupUi( m_listDialog );
    connect( m_listWidget->buttonSettings, SIGNAL(clicked()),
             m_configDialog, SLOT(show()) );
    connect( m_listWidget->spinBoxYear, SIGNAL(valueChanged(int)),
             this, SLOT(updateListDialogForYear(int)) );
    connect( m_listWidget->treeWidget, SIGNAL(itemSelectionChanged()),
             this, SLOT(updateListDialogButtons()) );
    connect( m_configDialog, SIGNAL(rejected()),
             this, SLOT(updateEclipses()) );
    connect( m_listDialog, SIGNAL(accepted()),
             this, SLOT(showSelectedEclipse()) );

    m_reminderDialog = new QDialog();
    m_reminderWidget = new Ui::EclipsesReminderDialog();
    m_reminderWidget->setupUi( m_reminderDialog );

    // initialize menu entries
    m_eclipsesActionGroup = new QActionGroup( this );
    m_actionGroups.append( m_eclipsesActionGroup );

    m_eclipsesListMenu = new QMenu( "" );
    m_eclipsesActionGroup->addAction( m_eclipsesListMenu->menuAction() );

    m_eclipsesMenuAction = new QAction( tr("More Ecli&pses..."), m_eclipsesActionGroup );
    m_eclipsesActionGroup->addAction( m_eclipsesMenuAction );
    connect( m_eclipsesMenuAction, SIGNAL(triggered()),
             m_listDialog, SLOT(show()) );

    // initialize clock
    const MarbleClock *clock = marbleModel()->clock();
    connect( clock, SIGNAL( timeChanged() ), SLOT( updateEclipses() ) );

    // initialize model
    m_model = new EclipsesModel( clock );
    // observation point defaults to home location
    qreal lat, lon;
    int zoom;
    marbleModel()->home(lon, lat, zoom);
    GeoDataCoordinates homeLocation( lon, lat, 0, GeoDataCoordinates::Degree );
    m_model->setObservationPoint( homeLocation );

    m_isInitialized = true;

    updateEclipses();
    updateMenuItems();
    updateSettings();
}

bool EclipsesPlugin::isInitialized() const
{
    return m_isInitialized;
}

bool EclipsesPlugin::eventFilter( QObject *object, QEvent *e )
{
    // delayed initialization of pointer to marble widget
    MarbleWidget *widget = dynamic_cast<MarbleWidget*> (object);
    if ( widget && m_marbleWidget != widget ) {
        connect( widget, SIGNAL(themeChanged(QString)),
                 this, SLOT(updateMenuItems()) );
        m_marbleWidget = widget;
        m_clock = m_marbleWidget->model()->clock();
    }

    return RenderPlugin::eventFilter(object, e);
}

bool EclipsesPlugin::render( GeoPainter *painter,
                             ViewportParams *viewport,
                             const QString &renderPos,
                             GeoSceneLayer *layer )
{
    Q_UNUSED( viewport );
    Q_UNUSED( renderPos );
    Q_UNUSED( layer );

    if( marbleModel()->planetId() == "earth" ) {
        m_model->paint( painter );
    }

    return true;
}

QHash<QString, QVariant> EclipsesPlugin::settings() const
{
    return m_settings;
}

void EclipsesPlugin::setSettings( const QHash<QString, QVariant> &settings )
{
    m_settings = settings;

    readSettings();
    emit settingsChanged( nameId() );
}

void EclipsesPlugin::readSettings()
{
}

void EclipsesPlugin::writeSettings()
{
    emit settingsChanged( nameId() );
}

void EclipsesPlugin::updateSettings()
{
    if (!isInitialized()) {
        return;
    }
}

void EclipsesPlugin::updateEclipses()
{
    mDebug() << "Updating eclipses....";
    const int year = marbleModel()->clock()->dateTime().date().year();

    if( m_model->year() != year ) {
        // remove old menus
        foreach( QAction *action, m_eclipsesListMenu->actions() ) {
            m_eclipsesListMenu->removeAction( action );
            delete action;
        }

        // update year and create menus for this year's eclipse events
        m_model->setYear( year );
        m_listWidget->spinBoxYear->setValue( year );

        m_eclipsesListMenu->setTitle( tr("Eclipses in %1").arg( year ) );

        foreach( EclipsesItem *item, m_model->items() ) {
            m_eclipsesListMenu->addAction( item->dateTime().date().toString() );
        }

        emit actionGroupsChanged();
    }

    m_model->synchronize( marbleModel()->clock() );
}

void EclipsesPlugin::updateMenuItems()
{
    if( !isInitialized() ) {
        return;
    }

    // eclipses are only supported for earth based obervers at the moment
    // so we disable the menu items for other celestial bodies

    bool active = ( marbleModel()->planetId() == "earth" );

    m_eclipsesListMenu->setEnabled( active );
    m_eclipsesMenuAction->setEnabled( active );
}

void EclipsesPlugin::updateListDialogForYear( int year )
{
    m_model->setYear( year );

    QTreeWidget *tree = m_listWidget->treeWidget;
    tree->clear();
    foreach( EclipsesItem *item, m_model->items() ) {
        QTreeWidgetItem *treeItem = new QTreeWidgetItem( tree );
        treeItem->setText( 0, item->dateTime().date().toString() );
        treeItem->setText( 1, item->dateTime().time().toString() );
        treeItem->setText( 2, QString( "%1" ).arg( item->magnitude() ) );
        treeItem->setText( 3, item->phaseText() );
        treeItem->setData( 0, Qt::UserRole, QVariant( item->index() ) );
    }
}

void EclipsesPlugin::updateListDialogButtons()
{
    QTreeWidget *tree = m_listWidget->treeWidget;
    m_listWidget->buttonShow->setEnabled( ( tree->selectedItems().count() > 0 ) );
}

void EclipsesPlugin::showSelectedEclipse()
{
    QTreeWidgetItem *item = m_listWidget->treeWidget->currentItem();
    if( item ) {
        showEclipse( item->data( 0, Qt::UserRole ).toInt() );
    }
}

void EclipsesPlugin::showEclipse( int index )
{
    EclipsesItem *item = m_model->eclipseWithIndex( index );
    Q_ASSERT( item );

    if( item ) {
        Q_ASSERT( m_clock );
        //m_clock->stop();
        m_clock->setDateTime( item->dateTime() );
    }
}

}

Q_EXPORT_PLUGIN2( EclipsesPlugin, Marble::EclipsesPlugin )

#include "EclipsesPlugin.moc"

