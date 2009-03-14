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
#include <QtCore/QString>
#include <QtGui/QColor>
//#include <QtCore/QTimer>

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

    qreal  shading(qreal lon, qreal lat);
    void    shadePixel(QRgb& pixcol, qreal shade);
    void    shadePixelComposite(QRgb& pixcol, QRgb& dpixcol, qreal shade);
	
    void  setShow(bool show);
    void  setCitylights(bool show) {m_citylights = show;}
    void  setCentered(bool show);
    void  setBody(QString body);
	
    bool getShow() {return m_show;}
    bool getCitylights() {return m_citylights;}
    bool getCentered() {return m_centered;}
    qreal getLon() {return m_lon * 180.0 / M_PI;}
    qreal getLat() {return -m_lat * 180.0 / M_PI;}
	
    ExtDateTime* datetime() {return m_datetime;}

	QString body() const;
	
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
    qreal m_lon;
    qreal m_lat;

    ExtDateTime* m_datetime;
    bool m_show;
    bool m_citylights;
    bool m_centered;
    QString m_body;
//     QTimer* m_timer;
};

}

#endif
