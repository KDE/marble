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
#include "EclipsesBrowserDialog.h"

#include "ui_EclipsesConfigDialog.h"
#include "ui_EclipsesReminderDialog.h"

#include <QMenu>
#include <QPushButton>

namespace Marble
{

EclipsesPlugin::EclipsesPlugin()
    : RenderPlugin( 0 ),
      m_isInitialized( false ),
      m_marbleWidget( 0 ),
      m_model( 0 ),
      m_eclipsesActionGroup( 0 ),
      m_eclipsesMenuAction( 0 ),
      m_eclipsesListMenu( 0 ),
      m_menuYear( 0 ),
      m_configDialog( 0 ),
      m_configWidget( 0 ),
      m_browserDialog( 0 ),
      m_reminderDialog( 0 ),
      m_reminderWidget( 0 )
{
}

EclipsesPlugin::EclipsesPlugin( const MarbleModel *marbleModel )
    : RenderPlugin( marbleModel ),
     m_isInitialized( false ),
     m_marbleWidget( 0 ),
     m_model( 0 ),
     m_eclipsesActionGroup( 0 ),
     m_eclipsesMenuAction( 0 ),
     m_eclipsesListMenu( 0 ),
     m_menuYear( 0 ),
     m_configDialog( 0 ),
     m_configWidget( 0 ),
     m_browserDialog( 0 ),
     m_reminderDialog( 0 ),
     m_reminderWidget( 0 )
{
    connect( this, SIGNAL(settingsChanged(QString)),
                   SLOT(updateSettings()) );
}

EclipsesPlugin::~EclipsesPlugin()
{
    if( m_isInitialized ) {
        delete m_model;
        delete m_eclipsesActionGroup;
        delete m_eclipsesListMenu;
        delete m_configDialog;
        delete m_browserDialog;
        delete m_reminderDialog;
    }
}

QStringList EclipsesPlugin::backendTypes() const
{
    return QStringList(QStringLiteral("eclipses"));
}

QString EclipsesPlugin::renderPolicy() const
{
    return QStringLiteral("ALWAYS");
}

QStringList EclipsesPlugin::renderPosition() const
{
    return QStringList(QStringLiteral("ORBIT"));
}

QString EclipsesPlugin::name() const
{
    return tr( "Eclipses" );
}

QString EclipsesPlugin::nameId() const
{
    return QStringLiteral("eclipses");
}

QString EclipsesPlugin::guiString() const
{
    return tr( "E&clipses" );
}

QString EclipsesPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString EclipsesPlugin::description() const
{
    return tr( "This plugin visualizes solar eclipses." );
}

QString EclipsesPlugin::copyrightYears() const
{
    return QStringLiteral("2013");
}

QVector<PluginAuthor> EclipsesPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Rene Kuettner"), QStringLiteral("rene@bitkanal.net"))
            << PluginAuthor(QStringLiteral("Gerhard Holtkamp"), QString());
}

QIcon EclipsesPlugin::icon() const
{
    return QIcon(QStringLiteral(":res/eclipses.png"));
}

RenderPlugin::RenderType EclipsesPlugin::renderType() const
{
    return RenderPlugin::ThemeRenderType;
    //return UnknownRenderType;
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
    connect( m_configWidget->buttonBox->button( QDialogButtonBox::Apply ),
             SIGNAL(clicked()), this, SLOT(writeSettings()) );
    connect( m_configWidget->buttonBox->button( QDialogButtonBox::Apply ),
             SIGNAL(clicked()), this, SLOT(updateEclipses()) );

    m_browserDialog = new EclipsesBrowserDialog( marbleModel() );
    connect( m_browserDialog, SIGNAL(buttonShowClicked(int,int)),
             this, SLOT(showEclipse(int,int)) );
    connect( m_browserDialog, SIGNAL(buttonSettingsClicked()),
             m_configDialog, SLOT(show()) );

    m_reminderDialog = new QDialog();
    m_reminderWidget = new Ui::EclipsesReminderDialog();
    m_reminderWidget->setupUi( m_reminderDialog );

    // initialize menu entries
    m_eclipsesActionGroup = new QActionGroup( this );
    m_actionGroups.append( m_eclipsesActionGroup );

    m_eclipsesListMenu = new QMenu();
    m_eclipsesActionGroup->addAction( m_eclipsesListMenu->menuAction() );
    connect( m_eclipsesListMenu, SIGNAL(triggered(QAction*)),
             this, SLOT(showEclipseFromMenu(QAction*)) );

    m_eclipsesMenuAction = new QAction(
            tr("Browse Ecli&pses..."), m_eclipsesActionGroup );
    m_eclipsesMenuAction->setIcon(QIcon(QStringLiteral(":res/eclipses.png")));
    m_eclipsesActionGroup->addAction( m_eclipsesMenuAction );
    connect( m_eclipsesMenuAction, SIGNAL(triggered()),
             m_browserDialog, SLOT(show()) );

    // initialize eclipses model
    m_model = new EclipsesModel( marbleModel() );

    connect( marbleModel()->clock(), SIGNAL(timeChanged()),
             this, SLOT(updateEclipses()) );

    m_isInitialized = true;

    readSettings();
    updateEclipses();
    updateMenuItemState();
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
                 this, SLOT(updateMenuItemState()) );
        m_marbleWidget = widget;
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

    if (marbleModel()->planetId() == QLatin1String("earth")) {
        foreach( EclipsesItem *item, m_model->items() ) {
            if( item->takesPlaceAt( marbleModel()->clock()->dateTime() ) ) {
                return renderItem( painter, item );
            }
        }
    }

    return true;
}

bool EclipsesPlugin::renderItem( GeoPainter *painter, EclipsesItem *item ) const
{
    int phase = item->phase();

    // Draw full penumbra shadow cone
    if( m_configWidget->checkBoxShowFullPenumbra->isChecked() ) {
        painter->setPen( Oxygen::aluminumGray1 );
        QColor sunBoundingBrush ( Oxygen::aluminumGray6 );
        sunBoundingBrush.setAlpha( 48 );
        painter->setBrush( sunBoundingBrush );
        painter->drawPolygon( item->shadowConePenumbra() );
    }

    // Draw 60% penumbra shadow cone
    if( m_configWidget->checkBoxShow60MagPenumbra->isChecked() ) {
        painter->setPen( Oxygen::aluminumGray2 );
        QColor penumbraBrush ( Oxygen::aluminumGray6 );
        penumbraBrush.setAlpha( 96 );
        painter->setBrush( penumbraBrush );
        painter->drawPolygon( item->shadowCone60MagPenumbra() );
    }

    // Draw southern boundary of the penumbra
    if( m_configWidget->checkBoxShowSouthernPenumbra->isChecked() ) {
        QColor southernBoundaryColor(Oxygen::brickRed1);
        southernBoundaryColor.setAlpha(128);
        QPen southernBoundary(southernBoundaryColor);
        southernBoundary.setWidth(3);
        painter->setPen( southernBoundary );
        painter->drawPolyline( item->southernPenumbra() );
        painter->setPen( Oxygen::brickRed5 );
        painter->drawPolyline( item->southernPenumbra() );
    }

    // Draw northern boundary of the penumbra
    if( m_configWidget->checkBoxShowNorthernPenumbra->isChecked() ) {
        QColor northernBoundaryColor(Oxygen::brickRed1);
        northernBoundaryColor.setAlpha(128);
        QPen northernBoundary(northernBoundaryColor);
        northernBoundary.setWidth(3);
        painter->setPen( northernBoundary );
        painter->drawPolyline( item->northernPenumbra() );
        painter->setPen( Oxygen::brickRed5 );
        painter->drawPolyline( item->northernPenumbra() );
    }

    // Draw Sunrise / Sunset Boundaries
    if( m_configWidget->checkBoxShowSunBoundaries->isChecked() ) {
        painter->setPen( Oxygen::hotOrange6 );
        const QList<GeoDataLinearRing> boundaries = item->sunBoundaries();
        QList<GeoDataLinearRing>::const_iterator i = boundaries.constBegin();
        QColor sunBoundingBrush ( Oxygen::hotOrange5 );
        sunBoundingBrush.setAlpha( 64 );
        painter->setBrush( sunBoundingBrush );
        for( ; i != boundaries.constEnd(); ++i ) {
            painter->drawPolygon( *i );
        }
    }

    // total or annular eclipse
    if( m_configWidget->checkBoxShowUmbra->isChecked() && phase > 3 )
    {
        painter->setPen( Oxygen::aluminumGray4 );
        QColor sunBoundingBrush ( Oxygen::aluminumGray6 );
        sunBoundingBrush.setAlpha( 128 );
        painter->setBrush( sunBoundingBrush );
        painter->drawPolygon( item->umbra() );

        // draw shadow cone
        painter->setPen( Qt::black );
        QColor shadowConeBrush ( Oxygen::aluminumGray6 );
        shadowConeBrush.setAlpha( 128 );
        painter->setBrush( shadowConeBrush );
        painter->drawPolygon( item->shadowConeUmbra() );
    }

    // plot central line
    if( m_configWidget->checkBoxShowCentralLine->isChecked() && phase > 3 ) {
        painter->setPen( Qt::black );
        painter->drawPolyline( item->centralLine() );
    }

    // mark point of maximum eclipse
    if( m_configWidget->checkBoxShowMaximum->isChecked() ) {
        painter->setPen( Qt::white );
        QColor sunBoundingBrush ( Qt::white );
        sunBoundingBrush.setAlpha( 128 );
        painter->setBrush( sunBoundingBrush );

        painter->drawEllipse( item->maxLocation(), 15, 15 );
        painter->setPen( Oxygen::brickRed4 );
        painter->drawText( item->maxLocation(), tr( "Maximum of Eclipse" ) );
    }

    return true;
}

QHash<QString, QVariant> EclipsesPlugin::settings() const
{
    return RenderPlugin::settings();
}

void EclipsesPlugin::setSettings( const QHash<QString, QVariant> &settings )
{
    RenderPlugin::setSettings( settings );
    m_settings = settings;
    emit settingsChanged( nameId() );
}

void EclipsesPlugin::readSettings()
{
    m_configWidget->checkBoxEnableLunarEclipses->setChecked(
            m_settings.value(QStringLiteral("enableLunarEclipses"), false).toBool());
    m_configWidget->checkBoxShowMaximum->setChecked(
            m_settings.value(QStringLiteral("showMaximum"), true).toBool());
    m_configWidget->checkBoxShowUmbra->setChecked(
            m_settings.value(QStringLiteral("showUmbra"), true).toBool());
    m_configWidget->checkBoxShowSouthernPenumbra->setChecked(
            m_settings.value(QStringLiteral("showSouthernPenumbra"), true).toBool());
    m_configWidget->checkBoxShowNorthernPenumbra->setChecked(
            m_settings.value(QStringLiteral("showNorthernPenumbra"), true).toBool());
    m_configWidget->checkBoxShowCentralLine->setChecked(
            m_settings.value(QStringLiteral("showCentralLine"), true).toBool());
    m_configWidget->checkBoxShowFullPenumbra->setChecked(
            m_settings.value(QStringLiteral("showFullPenumbra"), true).toBool());
    m_configWidget->checkBoxShow60MagPenumbra->setChecked(
            m_settings.value(QStringLiteral("show60MagPenumbra"), false).toBool());
    m_configWidget->checkBoxShowSunBoundaries->setChecked(
            m_settings.value(QStringLiteral("showSunBoundaries"), true).toBool());
}

void EclipsesPlugin::writeSettings()
{
    m_settings.insert(QStringLiteral("enableLunarEclipses"),
            m_configWidget->checkBoxEnableLunarEclipses->isChecked() );
    m_settings.insert(QStringLiteral("showMaximum"),
            m_configWidget->checkBoxShowMaximum->isChecked() );
    m_settings.insert(QStringLiteral("showUmbra"),
            m_configWidget->checkBoxShowUmbra->isChecked() );
    m_settings.insert(QStringLiteral("showSouthernPenumbra"),
            m_configWidget->checkBoxShowSouthernPenumbra->isChecked() );
    m_settings.insert(QStringLiteral("showNorthernPenumbra"),
            m_configWidget->checkBoxShowNorthernPenumbra->isChecked() );
    m_settings.insert(QStringLiteral("showCentralLine"),
            m_configWidget->checkBoxShowCentralLine->isChecked() );
    m_settings.insert(QStringLiteral("showFullPenumbra"),
            m_configWidget->checkBoxShowFullPenumbra->isChecked() );
    m_settings.insert(QStringLiteral("show60MagPenumbra"),
            m_configWidget->checkBoxShow60MagPenumbra->isChecked() );
    m_settings.insert(QStringLiteral("showSunBoundaries"),
            m_configWidget->checkBoxShowSunBoundaries->isChecked() );

    emit settingsChanged( nameId() );
}

void EclipsesPlugin::updateSettings()
{
    if (!isInitialized()) {
        return;
    }

    m_browserDialog->setWithLunarEclipses(
            m_settings.value(QStringLiteral("enableLunarEclipses")).toBool());
    if( m_model->withLunarEclipses() !=
            m_settings.value(QStringLiteral("enableLunarEclipses")).toBool()) {
        updateEclipses();
    }
}

void EclipsesPlugin::updateEclipses()
{
    // mDebug() << "Updating eclipses....";
    const int year = marbleModel()->clock()->dateTime().date().year();
    const bool lun = m_settings.value(QStringLiteral("enableLunarEclipses")).toBool();

    if( ( m_menuYear != year ) || ( m_model->withLunarEclipses() != lun ) ) {

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

        // enable/disable lunar eclipses if necessary
        if( m_model->withLunarEclipses() != lun ) {
            m_model->setWithLunarEclipses( lun );
        }

        m_eclipsesListMenu->setTitle( tr("Eclipses in %1").arg( year ) );

        foreach( EclipsesItem *item, m_model->items() ) {
            QAction *action = m_eclipsesListMenu->addAction(
                        item->dateMaximum().date().toString() );
            action->setData( QVariant( 1000 * item->dateMaximum().date().year() +  item->index() ) );
            action->setIcon( item->icon() );
        }

        emit actionGroupsChanged();
    }
}

void EclipsesPlugin::updateMenuItemState()
{
    if( !isInitialized() ) {
        return;
    }

    // eclipses are only supported for earth based obervers at the moment
    // so we disable the menu items for other celestial bodies

    const bool active = (marbleModel()->planetId() == QLatin1String("earth"));

    m_eclipsesListMenu->setEnabled( active );
    m_eclipsesMenuAction->setEnabled( active );
}

void EclipsesPlugin::showEclipse( int year, int index )
{
    if( m_model->year() != year ) {
        m_model->setYear( year );
    }

    EclipsesItem *item = m_model->eclipseWithIndex( index );
    Q_ASSERT( item );

    if( item ) {
        m_marbleWidget->model()->clock()->setDateTime( item->dateMaximum() );
        m_marbleWidget->centerOn( item->maxLocation() );
    }
}

void EclipsesPlugin::showEclipseFromMenu( QAction *action )
{
    Q_ASSERT( action->data().isValid() );
    int year = action->data().toInt() / 1000;
    int index = action->data().toInt() - 1000 * year;

    showEclipse( year, index );
}

} // namespace Marble

#include "moc_EclipsesPlugin.cpp"

