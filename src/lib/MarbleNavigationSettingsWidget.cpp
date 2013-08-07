//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "MarbleNavigationSettingsWidget.h"
#include "ui_MarbleNavigationSettingsWidget.h"

namespace Marble {

class MarbleNavigationSettingsWidget::Private : public Ui::MarbleNavigationSettingsWidget
{
};

MarbleNavigationSettingsWidget::MarbleNavigationSettingsWidget( QWidget *parent ) :
    QWidget( parent ),
    d( new Private )
{
    d->setupUi( this );
}

MarbleNavigationSettingsWidget::~MarbleNavigationSettingsWidget()
{
    delete d;
}

DragLocation MarbleNavigationSettingsWidget::dragLocation() const
{
    return static_cast<DragLocation>( d->kcfg_dragLocation->currentIndex() );
}

OnStartup MarbleNavigationSettingsWidget::startupLocation() const
{
    return static_cast<OnStartup>( d->kcfg_onStartup->currentIndex() );
}

bool MarbleNavigationSettingsWidget::isInertialEarthRotationEnabled() const
{
    return d->kcfg_inertialEarthRotation->isChecked();
}

bool MarbleNavigationSettingsWidget::isTargetVoyageAnimationEnabled() const
{
    return d->kcfg_animateTargetVoyage->isChecked();
}

QString MarbleNavigationSettingsWidget::externalMapEditor() const
{
    const int editorIndex = d->kcfg_externalMapEditor->currentIndex();

    if ( editorIndex == 1 )
        return "potlatch";

    if ( editorIndex == 2 )
        return "josm";

    if ( editorIndex == 3 )
        return "merkaartor";

    return QString();
}

void MarbleNavigationSettingsWidget::setDragLocation( DragLocation dragLocation )
{
    d->kcfg_dragLocation->setCurrentIndex( dragLocation );
}

void MarbleNavigationSettingsWidget::setStartupLocation( OnStartup onStartup )
{
    d->kcfg_onStartup->setCurrentIndex( onStartup );
}

void MarbleNavigationSettingsWidget::setInertialEarthRotationEnabled( bool inertialEarthRotation )
{
    d->kcfg_inertialEarthRotation->setChecked( inertialEarthRotation );
}

void MarbleNavigationSettingsWidget::setAnimateTargetVoyage( bool animateTargetVoyage )
{
    d->kcfg_animateTargetVoyage->setChecked( animateTargetVoyage );
}

void MarbleNavigationSettingsWidget::setExternalMapEditor( const QString &externalMapEditor )
{
    int editorIndex = 0;
    if ( externalMapEditor == "potlatch") {
        editorIndex = 1;
    } else if ( externalMapEditor == "josm") {
        editorIndex = 2;
    } else if ( externalMapEditor == "merkaartor") {
        editorIndex = 3;
    }
    d->kcfg_externalMapEditor->setCurrentIndex( editorIndex );
}

} // namespace Marble

#include "MarbleNavigationSettingsWidget.moc"



