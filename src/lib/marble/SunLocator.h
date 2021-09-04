// SPDX-FileCopyrightText: 2007-2009 David Roberts <dvdr18@gmail.com>
// 
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef MARBLE_SUNLOCATOR_H
#define MARBLE_SUNLOCATOR_H


#include <QObject>
#include <QColor>

//FIXME: This class shouldn't be exposed but is needed by the worldclock plasmoid
#include "marble_export.h"

namespace Marble
{
class MarbleClock;
class SunLocatorPrivate;
class Planet;

class MARBLE_EXPORT SunLocator : public QObject
{
    Q_OBJECT

 public:
    SunLocator( const MarbleClock *clock, const Planet *planet );
    ~SunLocator() override;

    qreal shading(qreal lon, qreal a, qreal c) const;
    static void shadePixel(QRgb &pixcol, qreal shade);
    static void shadePixelComposite(QRgb &pixcol, const QRgb &dpixcol, qreal shade);

    void  setPlanet( const Planet *planet );

    qreal getLon() const;
    qreal getLat() const;

 public Q_SLOTS:
    void update();

 Q_SIGNALS:
    void positionChanged( qreal lon, qreal lat );

 private:
    SunLocatorPrivate * const d;

    Q_DISABLE_COPY( SunLocator )
};

}

#endif
