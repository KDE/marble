//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Torsten Rahn   <rahn@kde.org>
// Copyright 2008      Inge Wallin    <inge@lysator.liu.se>
// Copyright 2011,2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// Copyright 2012      Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

// Self
#include "RenderPlugin.h"

// Marble
#include "DialogConfigurationInterface.h"
#include "MarbleModel.h"
#include "MarbleDebug.h"
#include "RenderPluginModel.h"
#include "RenderState.h"

// Qt
#include <QAction>
#include <QStandardItem>


namespace Marble
{

class Q_DECL_HIDDEN RenderPlugin::Private
{
  public:
    Private( const MarbleModel *marbleModel )
        : m_marbleModel( marbleModel ),
          m_action(0),
          m_item(),
          m_enabled(true),
          m_visible(true),
          m_userCheckable(true)
    {
    }

    ~Private()
    {
    }

    // const: RenderPlugins should only read the model, not modify it
    const MarbleModel  *const m_marbleModel;

    QAction            m_action;
    QStandardItem      m_item;

    bool                m_enabled;
    bool                m_visible;
    bool                m_userCheckable;
};


RenderPlugin::RenderPlugin( const MarbleModel *marbleModel )
    : d( new Private( marbleModel ) )
{
    connect( &d->m_action, SIGNAL(toggled(bool)),
             this,         SLOT(setVisible(bool)) );
    connect( this,         SIGNAL(visibilityChanged(bool,QString)),
             &d->m_action, SLOT(setChecked(bool)) );
    connect( this,         SIGNAL(enabledChanged(bool)),
             &d->m_action, SLOT(setVisible(bool)) );
    connect( this,         SIGNAL(enabledChanged(bool)),
                           SIGNAL(actionGroupsChanged()) );

    connect( this, SIGNAL(visibilityChanged(bool,QString)),
             this, SIGNAL(repaintNeeded()) );
    connect( this, SIGNAL(settingsChanged(QString)),
             this, SIGNAL(repaintNeeded()) );
}

RenderPlugin::~RenderPlugin()
{
    delete d;
}

const MarbleModel* RenderPlugin::marbleModel() const
{
    return d->m_marbleModel;
}

QAction* RenderPlugin::action() const
{
    d->m_action.setCheckable( true );
    d->m_action.setChecked( visible() );
    d->m_action.setIcon( icon() );
    d->m_action.setText( guiString() );
    d->m_action.setToolTip( description() );
    return &d->m_action;
}

const QList<QActionGroup*>* RenderPlugin::actionGroups() const
{
    return 0;
}

const QList<QActionGroup*>* RenderPlugin::toolbarActionGroups() const
{
    return 0;
}

QStandardItem* RenderPlugin::item()
{
    d->m_item.setIcon( icon() );
    d->m_item.setText( name() );
    d->m_item.setEditable( false );
    d->m_item.setCheckable( true );
    d->m_item.setCheckState( enabled() ?  Qt::Checked : Qt::Unchecked  );
    d->m_item.setToolTip( description() );
    d->m_item.setFlags( d->m_item.flags() & ~Qt::ItemIsSelectable );

    // Custom data
    d->m_item.setData( nameId(), RenderPluginModel::NameId );
    d->m_item.setData( (bool) qobject_cast<DialogConfigurationInterface *>( this ), RenderPluginModel::ConfigurationDialogAvailable );
    d->m_item.setData( backendTypes(), RenderPluginModel::BackendTypes );
    d->m_item.setData( version(), RenderPluginModel::Version );
    d->m_item.setData( aboutDataText(), RenderPluginModel::AboutDataText );
    d->m_item.setData( copyrightYears(), RenderPluginModel::CopyrightYears );

    return &d->m_item;
}

void RenderPlugin::applyItemState()
{
    setEnabled( d->m_item.checkState() == Qt::Checked );
}

void RenderPlugin::retrieveItemState()
{
    d->m_item.setCheckState( enabled() ? Qt::Checked : Qt::Unchecked  );
}

void RenderPlugin::setEnabled( bool enabled )
{
    if ( enabled == d->m_enabled )
        return;

    d->m_enabled = enabled;

    d->m_item.setCheckState( enabled ? Qt::Checked : Qt::Unchecked  );

    emit enabledChanged( enabled );
}

void RenderPlugin::setVisible( bool visible )
{
    if ( visible == d->m_visible )
        return;

    d->m_visible = visible;

    emit visibilityChanged( visible, nameId() );
}

void RenderPlugin::setUserCheckable( bool checkable )
{
    if ( checkable != d->m_userCheckable ) {
        d->m_action.setEnabled( checkable );
        d->m_userCheckable = checkable;
        emit userCheckableChanged( checkable );
    }
}

bool RenderPlugin::enabled() const
{
    return d->m_enabled;
}

bool RenderPlugin::visible() const
{
    return d->m_visible;
}

bool RenderPlugin::isUserCheckable() const
{
    return d->m_userCheckable;
}

QHash<QString,QVariant> RenderPlugin::settings() const
{
    QHash<QString,QVariant> result;

    result.insert(QStringLiteral("enabled"), enabled());
    result.insert(QStringLiteral("visible"), visible());

    return result;
}

void RenderPlugin::setSettings( const QHash<QString,QVariant> &settings )
{
    setEnabled(settings.value(QStringLiteral("enabled"), enabled()).toBool());
    setVisible(settings.value(QStringLiteral("visible"), visible()).toBool());
}

RenderPlugin::RenderType RenderPlugin::renderType() const
{
    return UnknownRenderType;
}

RenderState RenderPlugin::renderState() const
{
    return RenderState( name() );
}

QString RenderPlugin::runtimeTrace() const
{
    return name();
}

bool RenderPlugin::eventFilter( QObject *, QEvent * )
{
    return false;
}

void RenderPlugin::restoreDefaultSettings()
{
    setSettings( QHash<QString,QVariant>() );
}

QStringList RenderPlugin::settingKeys() const
{
    return settings().keys();
}

bool RenderPlugin::setSetting( const QString & key, const QVariant & value )
{
    QHash< QString, QVariant> settings = this->settings();
    if( settings.contains( key ) )
    {
        settings [ key ] = value;
        setSettings( settings );
        return true;
    } else {
        return false;
    }
}

QVariant RenderPlugin::setting( const QString & name ) const
{
    return settings().value( name, QVariant() );
}

} // namespace Marble

#include "moc_RenderPlugin.cpp"
