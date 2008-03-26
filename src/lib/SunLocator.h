// (c) 2007--2008 David Roberts

#ifndef SUNLOCATOR_H
#define SUNLOCATOR_H

#include <cmath>

#include <QtCore/QObject>
#include <QtGui/QColor>
#include <QTimer>

#include "ExtDateTime.h"


class SunLocator : public QObject
{
    Q_OBJECT
	
 public:
    explicit SunLocator();
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
    void updateSun();
    void centerSun();
    void reenableWidgetInput();
	
 private:
    void updatePosition();
 protected:
    double m_lon;
    double m_lat;

    ExtDateTime* m_datetime;
    bool m_show;
    bool m_citylights;
    bool m_centered;
    QTimer* m_timer;
};

#endif
