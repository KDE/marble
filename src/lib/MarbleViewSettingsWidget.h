//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_MARBLEVIEWSETTINGSWIDGET_H
#define MARBLE_MARBLEVIEWSETTINGSWIDGET_H

#include <QWidget>

#include "marble_export.h"
#include "MarbleGlobal.h"

#include <QLocale>

namespace Marble
{

/** 
 * @short A public class that adds methods to the UI Cache Settings Widget.
 */
class MARBLE_EXPORT MarbleViewSettingsWidget : public QWidget
{
    Q_OBJECT

 public:
    explicit MarbleViewSettingsWidget( QWidget *parent = 0 );
    ~MarbleViewSettingsWidget();

    QLocale::MeasurementSystem measurementSystem() const;

    AngleUnit angleUnit() const;

    MapQuality stillQuality() const;

    MapQuality animationQuality() const;

    LabelLocalization labelLocalization() const;

    QFont mapFont() const;

    GraphicsSystem graphicsSystem() const;

 public Q_SLOTS:
    void setMeasurementSystem( QLocale::MeasurementSystem measurementSystem );

    void setAngleUnit( AngleUnit angleUnit );

    void setStillQuality( MapQuality quality );

    void setAnimationQuality( MapQuality quality );

    void setLabelLocalization( LabelLocalization labelLocalization );

    void setMapFont( const QFont &font );

    void setGraphicsSystem( GraphicsSystem graphicsSystem );

 private:
    class Private;
    Private *d;
};

}

#endif
