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


#ifndef SUNLOCATOR_H
#define SUNLOCATOR_H

#include <cmath>
// M_PI is sometimes defined in <cmath>
#ifndef M_PI 
#define M_PI 3.14159265358979323846264338327950288419717
#endif

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtGui/QColor>

#include "ExtDateTime.h"
//FIXME: This class shouldn't be exposed but is needed by the worldclock plasmoid
#include "marble_export.h"

namespace Marble
{

class SunLocatorPrivate;

class MARBLE_EXPORT SunLocator : public QObject
{
    Q_OBJECT

 public:
    /** At the moment Sunlocator does not take ownership of dateTime.
	However in the destructor there is a delete commented out.
	So strictly Q_DISABLE_COPY is not necessary in this class.
	FIXME: clearify this issue.
    */
    explicit SunLocator(ExtDateTime *dateTime);
    virtual ~SunLocator();

    qreal shading(qreal lon, qreal lat) const;
    void  shadePixel(QRgb& pixcol, qreal shade) const;
    void  shadePixelComposite(QRgb& pixcol, QRgb& dpixcol, qreal shade) const;

    void  setShow(bool show);
    void  setCitylights(bool show);
    void  setCentered(bool show);
    void  setBody(QString body);

    bool getShow() const;
    bool getCitylights() const;
    bool getCentered() const;
    qreal getLon() const;
    qreal getLat() const;

    ExtDateTime* datetime() const;

 public Q_SLOTS:
    void update();

 Q_SIGNALS:
    void updateStars();
    void updateSun();
    void centerSun();
    void reenableWidgetInput();

 private:
    void updatePosition();

    SunLocatorPrivate * const d;

 protected:
    Q_DISABLE_COPY( SunLocator )
};

}

#endif
