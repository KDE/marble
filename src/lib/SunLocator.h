// Copyright 2007-2008 David Roberts <dvdr18@gmail.com>
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
#include <QtGui/QColor>
#include <QtCore/QTimer>

#include "ExtDateTime.h"


class SunLocator : public QObject
{
    Q_OBJECT
	
 public:
    /** At the moment Sunlocator does not take ownership of dateTime.
	However in the destructor there is a delete commented out.
	So strictly Q_DISABLE_COPY is not neccessary in this class.
	FIXME: clearify this issue.
    */
    explicit SunLocator(ExtDateTime *dateTime);
    virtual ~SunLocator();

    double  shading(double lon, double lat);
    void    shadePixel(QRgb& pixcol, double shade);
    void    shadePixelComposite(QRgb& pixcol, QRgb& dpixcol, double shade);
	
    void  setShow(bool show);
    void  setCitylights(bool show) {m_citylights = show;}
    void  setCentered(bool show);
	
    bool getShow() {return m_show;}
    bool getCitylights() {return m_citylights;}
    bool getCentered() {return m_centered;}
    double getLon() {return m_lon * 180.0 / M_PI;}
    double getLat() {return -m_lat * 180.0 / M_PI;}
	
    ExtDateTime* datetime() {return m_datetime;}
	
 public Q_SLOTS:
    void update();
	
 Q_SIGNALS:
    void updateStars();
    void updateSun();
    void centerSun();
    void reenableWidgetInput();
	
 private:
    void updatePosition();

 protected:
    Q_DISABLE_COPY( SunLocator )
    double m_lon;
    double m_lat;

    ExtDateTime* m_datetime;
    bool m_show;
    bool m_citylights;
    bool m_centered;
//     QTimer* m_timer;
};

#endif
