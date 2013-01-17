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
#include "GeoPainter.h"

#include "EclipsesModel.h"
#include "EclipsesItem.h"
#include "EclipsesListDialog.h"

#include "ui_EclipsesConfigDialog.h"
#include "ui_EclipsesReminderDialog.h"

#include <QPushButton>
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
      m_menuYear( 0 ),
      m_configDialog( 0 ),
      m_configWidget( 0 ),
      m_listDialog( 0 ),
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
     m_menuYear( 0 ),
     m_configDialog( 0 ),
     m_configWidget( 0 ),
     m_listDialog( 0 ),
     m_reminderDialog( 0 ),
     m_reminderWidget( 0 )
{
    connect( this, SIGNAL(settingsChanged(QString)),
                   SLOT(updateSettings()) );

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

    connect( m_configDialog, SIGNAL(accepted()),
             this, SLOT(writeSettings()) );
    connect( m_configDialog, SIGNAL(rejected()),
             this, SLOT(readSettings()) );
    connect( m_configWidget->buttonBox->button( QDialogButtonBox::Reset ),
             SIGNAL(clicked()), this, SLOT(readSettings()) );

    m_listDialog = new EclipsesListDialog( marbleModel() );
    connect( m_listDialog, SIGNAL(buttonShowClicked(int, int)),
             this, SLOT(showEclipse(int,int)) );

    m_reminderDialog = new QDialog();
    m_reminderWidget = new Ui::EclipsesReminderDialog();
    m_reminderWidget->setupUi( m_reminderDialog );

    // initialize menu entries
    m_eclipsesActionGroup = new QActionGroup( this );
    m_actionGroups.append( m_eclipsesActionGroup );

    m_eclipsesListMenu = new QMenu( "" );
    m_eclipsesActionGroup->addAction( m_eclipsesListMenu->menuAction() );

    m_eclipsesMenuAction = new QAction( tr("Browse Ecli&pses..."), m_eclipsesActionGroup );
    m_eclipsesActionGroup->addAction( m_eclipsesMenuAction );
    connect( m_eclipsesMenuAction, SIGNAL(triggered()),
             m_listDialog, SLOT(show()) );

    // initialize eclipses model
    m_model = new EclipsesModel( marbleModel() );

    m_isInitialized = true;

    readSettings();
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
        connect( m_clock, SIGNAL(timeChanged()),
                 this, SLOT(updateEclipses()) );
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
        foreach( EclipsesItem *item, m_model->items() ) {
            if( item->takesPlaceAt( marbleModel()->clock()->dateTime() ) ) {
                return renderItem( painter, item );
            }
        }
    }

    return true;
}

bool EclipsesPlugin::renderItem( GeoPainter *painter, EclipsesItem *item )
{
    QList<GeoDataCoordinates>::const_iterator ci;
    int phase = item->phase();

    // plot central line for central eclipses
    if( m_configWidget->checkBoxShowCentralLine->isChecked() ) {
        painter->setPen( Qt::black );
        painter->drawPolyline( item->centralLine() );
    }

    // total or annular eclipse
    if( m_configWidget->checkBoxShowUmbra->isChecked() && phase > 3 )
    {
        painter->setPen( Oxygen::aluminumGray4 );
        QColor sunBoundingBrush ( Oxygen::aluminumGray4 );
        sunBoundingBrush.setAlpha( 128 );
        painter->setBrush( sunBoundingBrush );
        painter->drawPolygon( item->umbra() );

        // draw shadow cone
        painter->setPen( Qt::black );
        ci = item->shadowConeUmbra().constBegin();
        for ( ; ci != item->shadowConeUmbra().constEnd(); ++ci ) {
            painter->drawEllipse( *ci, 2, 2 );
        }
    }

    // penumbra shadow cones

    if( m_configWidget->checkBoxShowFullPenumbra->isChecked() ) {
        painter->setPen( Qt::blue );
        ci = item->shadowConePenUmbra().constBegin();
        for ( ; ci != item->shadowConePenUmbra().constEnd(); ++ci ) {
            painter->drawEllipse( *ci, 2, 2 );
        }
    }

    if( m_configWidget->checkBoxShow60MagPenumbra->isChecked() ) {
        painter->setPen( Qt::magenta );
        ci = item->shadowCone60MagPenUmbra().constBegin();
        for ( ; ci != item->shadowCone60MagPenUmbra().constEnd(); ++ci ) {
            painter->drawEllipse( *ci, 3, 3 );
        }
    }

    if( m_configWidget->checkBoxShowMaximum->isChecked() ) {
        // mark point of maximum eclipse
        painter->setPen( Qt::white );
        QColor sunBoundingBrush ( Qt::white );
        sunBoundingBrush.setAlpha( 128 );
        painter->setBrush( sunBoundingBrush );

        painter->drawEllipse( item->maxLocation(), 15, 15 );
        painter->setPen( Oxygen::brickRed4 );
        painter->drawText( item->maxLocation(), tr( "Maximum of Eclipse" ) );
    }

    if( m_configWidget->checkBoxShowSouthernPenumbra->isChecked() ) {
        // southern boundary
        painter->setPen( Oxygen::brickRed4 );
        painter->drawPolyline( item->southernPenUmbra() );
    }

    if( m_configWidget->checkBoxShowNorthernPenumbra->isChecked() ) {
        // northern boundary
        painter->setPen( Oxygen::brickRed4 );
        painter->drawPolyline( item->northernPenUmbra() );
    }

    if( m_configWidget->checkBoxShowSunBoundaries->isChecked() ) {
        // Sunrise / Sunset Boundaries
        painter->setPen( Oxygen::hotOrange5 );
        const QList<GeoDataLinearRing> boundaries = item->sunBoundaries();
        QList<GeoDataLinearRing>::const_iterator i = boundaries.constBegin();
        for( ; i != boundaries.constEnd(); ++i ) {
            QColor sunBoundingBrush ( Oxygen::hotOrange5 );
            sunBoundingBrush.setAlpha( 64 );
            painter->setBrush( sunBoundingBrush );
            painter->drawPolygon( *i );
        }
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
    emit settingsChanged( nameId() );
}

void EclipsesPlugin::readSettings()
{
    m_configWidget->checkBoxEnableLunarEclipses->setChecked(
            m_settings.value( "enableLunarEclipses", false ).toBool() );
    m_configWidget->checkBoxShowMaximum->setChecked(
            m_settings.value( "showMaximum", true ).toBool() );
    m_configWidget->checkBoxShowUmbra->setChecked(
            m_settings.value( "showUmbra", true ).toBool() );
    m_configWidget->checkBoxShowSouthernPenumbra->setChecked(
            m_settings.value( "showSouthernPenumbra", true ).toBool() );
    m_configWidget->checkBoxShowNorthernPenumbra->setChecked(
            m_settings.value( "showNorthernPenumbra", true ).toBool() );
    m_configWidget->checkBoxShowCentralLine->setChecked(
            m_settings.value( "showCentralLine", true ).toBool() );
    m_configWidget->checkBoxShowFullPenumbra->setChecked(
            m_settings.value( "showFullPenumbra", true ).toBool() );
    m_configWidget->checkBoxShow60MagPenumbra->setChecked(
            m_settings.value( "show60MagPenumbra", false ).toBool() );
    m_configWidget->checkBoxShowSunBoundaries->setChecked(
            m_settings.value( "showSunBoundaries", true ).toBool() );
}

void EclipsesPlugin::writeSettings()
{
    m_settings.insert( "enableLunarEclipses",
            m_configWidget->checkBoxEnableLunarEclipses->isChecked() );
    m_settings.insert( "showMaximum",
            m_configWidget->checkBoxShowMaximum->isChecked() );
    m_settings.insert( "showUmbra",
            m_configWidget->checkBoxShowUmbra->isChecked() );
    m_settings.insert( "showSouthernPenumbra",
            m_configWidget->checkBoxShowSouthernPenumbra->isChecked() );
    m_settings.insert( "showNorthernPenumbra",
            m_configWidget->checkBoxShowNorthernPenumbra->isChecked() );
    m_settings.insert( "showCentralLine",
            m_configWidget->checkBoxShowCentralLine->isChecked() );
    m_settings.insert( "showFullPenumbra",
            m_configWidget->checkBoxShowFullPenumbra->isChecked() );
    m_settings.insert( "show60MagPenumbra",
            m_configWidget->checkBoxShow60MagPenumbra->isChecked() );
    m_settings.insert( "showSunBoundaries",
            m_configWidget->checkBoxShowSunBoundaries->isChecked() );

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

    if( m_menuYear != year ) {

        // remove old menus
        foreach( QAction *action, m_eclipsesListMenu->actions() ) {
            m_eclipsesListMenu->removeAction( action );
            delete action;
        }

        // update year and create menus for this year's eclipse events
        if( m_model->year() != year ) {
            m_model->setYear( year );
        }
        m_menuYear = year;

        m_eclipsesListMenu->setTitle( tr("Eclipses in %1").arg( year ) );

        foreach( EclipsesItem *item, m_model->items() ) {
            QAction *action = m_eclipsesListMenu->addAction(
                        item->dateMaximum().date().toString() );
            action->setData( QVariant( item->index() ) );
            connect( m_eclipsesListMenu, SIGNAL(triggered(QAction*)),
                     this, SLOT(showEclipseFromMenu(QAction*)) );
        }

        emit actionGroupsChanged();
    }
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

void EclipsesPlugin::showEclipse( int year, int index )
{
    EclipsesItem *item = m_model->eclipseWithIndex( year, index );
    Q_ASSERT( item );

    if( item ) {
        Q_ASSERT( m_clock );
        m_clock->setDateTime( item->dateMaximum() );
        m_marbleWidget->centerOn( item->maxLocation() );
    }
}

void EclipsesPlugin::showEclipseFromMenu( QAction *action )
{
    QDate date = QDate::fromString( action->text() );
    Q_ASSERT( date.isValid() );
    Q_ASSERT( action->data().isValid() );
    int index = action->data().toInt();

    mDebug() << "Eclipse from menu selected: year=" <<
        date.year() << ", index=" << index;
    showEclipse( date.year(), index );
}

}

Q_EXPORT_PLUGIN2( EclipsesPlugin, Marble::EclipsesPlugin )

#include "EclipsesPlugin.moc"

