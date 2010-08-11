// Copyright 2007-2009 David Roberts <dvdr18@gmail.com>
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public 
// License along with this library.  If not, see <http://www.gnu.org/licenses/>.

#ifndef MARBLE_SUNLOCATOR_H
#define MARBLE_SUNLOCATOR_H


#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtGui/QColor>

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
    /** At the moment Sunlocator does not take ownership of dateTime.
	However in the destructor there is a delete commented out.
	So strictly Q_DISABLE_COPY is not necessary in this class.
	FIXME: clarify this issue.
    */
    SunLocator(MarbleClock *clock, Planet *planet);
    virtual ~SunLocator();

    qreal shading(qreal lon, qreal a, qreal c) const;
    void  shadePixel(QRgb& pixcol, qreal shade) const;
    void  shadePixelComposite(QRgb& pixcol, QRgb& dpixcol, qreal shade) const;

    void  setShow(bool show);
    void  setCitylights(bool show);
    void  setCentered(bool show);
    void  setPlanet(Planet *planet);

    bool getShow() const;
    bool getCitylights() const;
    bool getCentered() const;
    qreal getLon() const;
    qreal getLat() const;

    MarbleClock* clock() const;

    Planet* planet() const;

 public Q_SLOTS:
    void update();

 Q_SIGNALS:
    void updateStars();
    void updateSun();
    void centerSun();
    void enableWidgetInput( bool );

 private:
    void updatePosition();

    SunLocatorPrivate * const d;

    Q_DISABLE_COPY( SunLocator )
};

}

#endif
