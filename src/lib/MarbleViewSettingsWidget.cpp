//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "MarbleViewSettingsWidget.h"
#include "ui_MarbleViewSettingsWidget.h"

#include "MarbleGlobal.h"

namespace Marble {

class MarbleViewSettingsWidget::Private : public Ui::MarbleViewSettingsWidget
{
};

MarbleViewSettingsWidget::MarbleViewSettingsWidget( QWidget *parent ) :
    QWidget( parent ),
    d( new Private )
{
    d->setupUi( this );

    // It's experimental -- so we remove it for now.
    // FIXME: Delete the following  line once OpenGL support is officially supported.
    d->kcfg_graphicsSystem->removeItem( Marble::OpenGLGraphics );

#ifdef Q_WS_X11
    const QString nativeString = tr( "Native (X11)" );
#elif defined Q_WS_MAC
    const QString nativeString = tr( "Native (Mac OS X Core Graphics)" );
#else
    const QString nativeString = tr( "Native" );
#endif

    d->kcfg_graphicsSystem->setItemText( Marble::NativeGraphics, nativeString );
}

MarbleViewSettingsWidget::~MarbleViewSettingsWidget()
{
    delete d;
}

QLocale::MeasurementSystem MarbleViewSettingsWidget::measurementSystem() const
{
    return static_cast<QLocale::MeasurementSystem>( d->kcfg_distanceUnit->currentIndex() );
}

AngleUnit MarbleViewSettingsWidget::angleUnit() const
{
    return static_cast<AngleUnit>( d->kcfg_angleUnit->currentIndex() );
}

MapQuality MarbleViewSettingsWidget::stillQuality() const
{
    return static_cast<MapQuality>( d->kcfg_stillQuality->currentIndex() );
}

MapQuality MarbleViewSettingsWidget::animationQuality() const
{
    return static_cast<MapQuality>( d->kcfg_animationQuality->currentIndex() );
}

LabelLocalization MarbleViewSettingsWidget::labelLocalization() const
{
    return static_cast<LabelLocalization>( d->kcfg_labelLocalization->currentIndex() );
}

QFont MarbleViewSettingsWidget::mapFont() const
{
    return d->kcfg_mapFont->currentFont();
}

GraphicsSystem MarbleViewSettingsWidget::graphicsSystem() const
{
    return static_cast<GraphicsSystem>( d->kcfg_graphicsSystem->currentIndex() );
}

void MarbleViewSettingsWidget::setMeasurementSystem( QLocale::MeasurementSystem measurementSystem )
{
    d->kcfg_distanceUnit->setCurrentIndex( measurementSystem );
}

void MarbleViewSettingsWidget::setAngleUnit( AngleUnit angleUnit )
{
    d->kcfg_angleUnit->setCurrentIndex( angleUnit );
}

void MarbleViewSettingsWidget::setStillQuality( MapQuality quality )
{
    d->kcfg_stillQuality->setCurrentIndex( quality );
}

void MarbleViewSettingsWidget::setAnimationQuality( MapQuality quality )
{
    d->kcfg_animationQuality->setCurrentIndex( quality );
}

void MarbleViewSettingsWidget::setLabelLocalization( LabelLocalization labelLocalization )
{
    d->kcfg_labelLocalization->setCurrentIndex( labelLocalization );
}

void MarbleViewSettingsWidget::setMapFont( const QFont &font )
{
    d->kcfg_mapFont->setCurrentFont( font );
}

void MarbleViewSettingsWidget::setGraphicsSystem( GraphicsSystem graphicsSystem )
{
    d->kcfg_graphicsSystem->setCurrentIndex( graphicsSystem );
}

} // namespace Marble

#include "MarbleViewSettingsWidget.moc"
