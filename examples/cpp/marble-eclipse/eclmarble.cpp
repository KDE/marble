//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Gerhard HOLTKAMP 
//

// Prototype of Solar Eclipse Marble Widget

#include <QtGui/QApplication>
#include <QtCore/QDebug>

#include <QtGui/QLayout>
#include <QtGui/QSlider>
#include <QtGui/QLabel>
#include <QtGui/QInputDialog>

#include <marble/MarbleWidget.h>
#include <marble/GeoPainter.h>
#include <marble/GeoDataLineString.h>
#include <marble/GeoDataLinearRing.h>
#include "eclsolar.h"

#include <cstdio>
#include <cstdlib>

using namespace Marble;

bool pline=true;
EclSolar ecps;

class MyMarbleWidget : public MarbleWidget
{
public:
    virtual void customPaint(GeoPainter* painter);

};
 
void MyMarbleWidget::customPaint(GeoPainter* painter)
{
    int np, kp, j;
    double lat1, lng1, lat2, lng2, lat3, lng3, lat4, lng4;
    double ltf[60], lnf[60];

    // plot central line for central eclipses

    np = ecps.eclPltCentral(true, lat1, lng1);
    kp = np;

    GeoDataLineString  centralLine ( Tessellate );
    centralLine << GeoDataCoordinates (lng1, lat1, 0.0, GeoDataCoordinates::Degree);

    if (np > 3)  // we have a central eclipse
    {
        while (np > 3)
         {
            np = ecps.eclPltCentral(false, lat1, lng1);
            if (np > 3)
            {
               centralLine << GeoDataCoordinates (lng1, lat1, 0.0, GeoDataCoordinates::Degree);
            };
        };
    }

    painter->setPen(Qt::black);
    painter->drawPolyline(centralLine);

    np = kp;
    if (np > 3)  // total or annular eclipse
    {
        // North/South Boundaries (umbra)

        np = ecps.centralBound(true, lat1, lng1, lat2, lng2);

        GeoDataLinearRing  lowerUmbra ( Tessellate );
        lowerUmbra << GeoDataCoordinates (lng1, lat1, 0.0, GeoDataCoordinates::Degree);
        GeoDataLinearRing  upperUmbra ( Tessellate );
        upperUmbra << GeoDataCoordinates (lng2, lat2, 0.0, GeoDataCoordinates::Degree);

        while (np > 0)
        {
            np = ecps.centralBound(false, lat1, lng1, lat2, lng2);

            if (lat1 <= 90.0) lowerUmbra << GeoDataCoordinates(lng1, lat1, 0.0, GeoDataCoordinates::Degree);
            if (lat2 <= 90.0) upperUmbra << GeoDataCoordinates(lng2, lat2, 0.0, GeoDataCoordinates::Degree);
        };

        // Invert upperUmbra linear ring
        GeoDataLinearRing invertedUpperUmbra (Tessellate);

        QVector<GeoDataCoordinates>::const_iterator iter = upperUmbra.constEnd();
        --iter;
        for( ; iter != upperUmbra.constBegin(); --iter ) {
            invertedUpperUmbra << *iter;
        }
        invertedUpperUmbra << upperUmbra.first();
        upperUmbra = invertedUpperUmbra;

        GeoDataLinearRing umbra ( Tessellate );

        umbra << lowerUmbra << upperUmbra;

        painter->setPen(oxygenAluminumGray4);

        QColor sunBoundingBrush ( oxygenAluminumGray4 );
        sunBoundingBrush.setAlpha( 128 );
        painter->setBrush( sunBoundingBrush );

        painter->drawPolygon( umbra );

    }
    
       //  draw shadow cones at maximum eclipse time

        ecps.getMaxPos(lat1, lng1);
        ecps.setLocalPos(lat1, lng1,0);
        ecps.getLocalMax(lat2, lat3, lat4);
       
        ecps.getShadowCone(lat2, true, 40, ltf, lnf);  // umbra
        for (j=0; j<40; j++)
        {
         if(ltf[j] < 100.0)
             painter->drawEllipse(GeoDataCoordinates(lnf[j], ltf[j], 0.0, GeoDataCoordinates::Degree),2,2);
        }

        ecps.setPenumbraAngle(1.0, 0);  // full penumbra
        ecps.getShadowCone(lat2, false, 60, ltf, lnf);
        painter->setPen(Qt::blue);
        for (j=0; j<60; j++)
        {
            if(ltf[j] < 100.0)
             painter->drawEllipse(GeoDataCoordinates(lnf[j], ltf[j], 0.0, GeoDataCoordinates::Degree),2,2);
        }

        ecps.setPenumbraAngle(0.6, 1);  // 60% magnitude penumbra
        ecps.getShadowCone(lat2, false, 60, ltf, lnf);
        painter->setPen(Qt::magenta);
        for (j=0; j<60; j++)
        {
         if(ltf[j] < 100.0)
             painter->drawEllipse(GeoDataCoordinates(lnf[j], ltf[j], 0.0, GeoDataCoordinates::Degree),3,3);
        }
        ecps.setPenumbraAngle(1.0, 0);
   

    // mark point of maximum eclipse
    ecps.getMaxPos(lat1, lng1);

    GeoDataCoordinates home2(lng1, lat1, 0.0, GeoDataCoordinates::Degree);
    painter->setPen(Qt::white);
    QColor sunBoundingBrush ( Qt::white );
    sunBoundingBrush.setAlpha( 128 );
    painter->setBrush( sunBoundingBrush );

    painter->drawEllipse(home2, 15, 15);
    painter->setPen(oxygenBrickRed4);
    painter->drawText(home2, "MaxEcl");

    // plot eclipse boundaries

    // North/South Boundaries (penumbra)

    painter->setPen(Qt::magenta);

    // southern boundary
    np = ecps.GNSBound(true, true, lat1, lng2);

    GeoDataLineString southernPenUmbra(Tessellate);
    while (np > 0)
    {
        np = ecps.GNSBound(false, true, lat1, lng1);
        if ((np > 0) && (lat1 <= 90.0)) southernPenUmbra  << GeoDataCoordinates (lng1, lat1, 0.0, GeoDataCoordinates::Degree);
    };

    painter->setPen(oxygenBrickRed4);
    painter->drawPolyline( southernPenUmbra );

    // northern boundary
    np = ecps.GNSBound(true, false, lat1, lng1);

    GeoDataLineString northernPenUmbra(Tessellate);
    while (np > 0)
    {
        np = ecps.GNSBound(false, false, lat1, lng1);
        if ((np > 0) && (lat1 <= 90.0)) northernPenUmbra  << GeoDataCoordinates (lng1, lat1, 0.0, GeoDataCoordinates::Degree);
    };

    painter->setPen(oxygenBrickRed4);
    painter->drawPolyline( northernPenUmbra );

    // Sunrise / Sunset Boundaries

    QList<GeoDataLinearRing*> sunBoundaries;

    np = ecps.GRSBound(true, lat1, lng1, lat3, lng3);
    GeoDataLinearRing * lowerBoundary = new GeoDataLinearRing( Tessellate );

    *lowerBoundary << GeoDataCoordinates (lng1, lat1, 0.0, GeoDataCoordinates::Degree);

    GeoDataLinearRing * upperBoundary = new GeoDataLinearRing( Tessellate );
    *upperBoundary << GeoDataCoordinates (lng3, lat3, 0.0, GeoDataCoordinates::Degree);

    while (np > 0)
    {
        np = ecps.GRSBound(false, lat2, lng2, lat4, lng4);
        pline = fabs(lng1 - lng2) < 10.0; // during partial eclipses, the Rise/Set lines switch at one stage.
                                          // This will prevent an ugly line between the switch points.
        // If there is a longitude jump then add the current section to our sun boundaries collection and start a new section
        if (!pline && !lowerBoundary->isEmpty()) {
            sunBoundaries.prepend(lowerBoundary);
            lowerBoundary = new GeoDataLinearRing( Tessellate );
        }
        if ((np > 0) && (lat2 <= 90.0) && (lat1 <= 90.0)) {
            *lowerBoundary << GeoDataCoordinates(lng2, lat2, 0.0, GeoDataCoordinates::Degree);
        }
        pline = fabs(lng3 - lng4) < 10.0; // during partial eclipses, the Rise/Set lines switch at one stage.
                                          // This will prevent an ugly line between the switch points.
        // If there is a longitude jump then add the current section to our sun boundaries collection and start a new section
        if (!pline && !upperBoundary->isEmpty()) {
            sunBoundaries.prepend(upperBoundary);
            upperBoundary = new GeoDataLinearRing( Tessellate );
        }
        if (pline && (np > 0) && (lat4 <= 90.0) && (lat3 <= 90.0)) {
            *upperBoundary << GeoDataCoordinates(lng4, lat4, 0.0, GeoDataCoordinates::Degree);
        }

        lng1 = lng2;
        lat1 = lat2;
        lng3 = lng4;
        lat3 = lat4;
    };

    if ( !lowerBoundary->isEmpty() ) {
        sunBoundaries.prepend(lowerBoundary);
    }
    if ( !upperBoundary->isEmpty() ) {
        sunBoundaries.prepend(upperBoundary);
    }


    for ( int result = 0; result < 2; ++result ) {
        GeoDataLinearRing sunBoundary( Tessellate );

        sunBoundary = *sunBoundaries.last();
        sunBoundaries.pop_back();

        int closestSection;

        while ( sunBoundaries.size() > 0) {

            closestSection = -1;
            // TODO: Now that MableMath is not public anymore we need a GeoDataCoordinates::distance() method in Marble.
            GeoDataLineString ruler;
            ruler << sunBoundary.last() << sunBoundary.first();
            qreal closestDistance = ruler.length(1);
            int closestEnd = 0;  // 0 = start of section, 1 = end of section

            // Look for the closest section that is closest to our sunBoundary section.
            for ( int it = 0; it < sunBoundaries.size(); ++it ) {
                GeoDataLineString distanceStartSection;
                distanceStartSection << sunBoundary.last() << sunBoundaries.at(it)->first();
                GeoDataLineString distanceEndSection;
                distanceEndSection << sunBoundary.last() << sunBoundaries.at(it)->last();
                if ( distanceStartSection.length(1) < closestDistance ) {
                    closestDistance = distanceStartSection.length(1);
                    closestSection = it;
                    closestEnd = 0;
                }
                if ( distanceEndSection.length(1) < closestDistance ) {
                    closestDistance = distanceEndSection.length(1);
                    closestSection = it;
                    closestEnd = 1;
                }
            }

            if (closestSection == -1) {
                // There is no other section that is closer to the end of our sunBoundary section than the startpoint of our sunBoundary itself
                break;
            }
            else {
                // We now concatenate the closest section to the sunBoundary.
                // First we might have to invert it so that we concatenate the right end
                if (closestEnd == 1) {
                    // TODO: replace this with a GeoDataLinearRing::invert() method that needs to be added to Marble ...
                    GeoDataLinearRing * invertedBoundary = new GeoDataLinearRing( Tessellate );

                    QVector<GeoDataCoordinates>::const_iterator iter = sunBoundaries.at(closestSection)->constEnd();
                    --iter;
                    for( ; iter != sunBoundaries.at(closestSection)->constBegin(); --iter ) {
                        *invertedBoundary << *iter;
                    }
                    *invertedBoundary << sunBoundaries.at(closestSection)->first();
                    delete sunBoundaries[closestSection];
                    sunBoundaries[closestSection] = invertedBoundary;
                }
                sunBoundary << *sunBoundaries[closestSection];

                // Now remove the section that we've just added from the list
                delete sunBoundaries[closestSection];
                sunBoundaries.removeAt(closestSection);
            }
        }

        // Now paint the processed Sunrise / Sunset area:
        painter->setPen( oxygenBrownOrange4 );

        QColor sunBoundingBrush ( oxygenBrownOrange4 );
        sunBoundingBrush.setAlpha( 64 );
        painter->setBrush( sunBoundingBrush );

        painter->drawPolygon(sunBoundary);

        // Stay only in the loop if there are two Sunrise / Sunset areas
        if (sunBoundaries.size() == 0) break;
    }

}

int main(int argc, char** argv)
{
  int yr, hc;
  double lat, lng;
  bool ok;
  char wbuf[700];
  
    QApplication app(argc,argv);
    QWidget *window = new QWidget;
 
    // initialize the solar eclipse class and get the year of the eclipse

    ecps.setLunarEcl(false);  // only solar eclipses
 //   ecps.setLocalPos(49.5, 175.5,0);  // test data for local positions. we would get that via mouse clicks normally
//    ecps.setLocalPos(33.525, 135.525,0);
 //   ecps.setLocalPos(36.0, 165.0,0);
    ecps.setTimezone(1.0);

    yr = QInputDialog::getInt(0, "Enter Year", "Year:", ecps.getYear(), -4500, 10000, 1,&ok);
    ecps.putYear(yr);

    ecps.getEclYearInfo(wbuf);  // find the eclipses for the selected year

    hc = QInputDialog::getInt(0, "Select Eclipse", wbuf, 1, 1, 7, 1,&ok);
    ecps.putEclSelect(hc);

    // Create a Marble QWidget without a parent
    MarbleWidget *mapWidget = new MyMarbleWidget();
 
    hc = 0;
    hc = QInputDialog::getInt(0, "MapTheme",
             "Set Map Theme\n 0: Plain \n 1: openstreetmap \n 2: clouds",
               0, 0, 2, 1,&ok);
			      
    if (hc == 1) mapWidget->setMapThemeId("earth/openstreetmap/openstreetmap.dgml");
    else
    {
        if (hc == 2) mapWidget->setMapThemeId("earth/bluemarble/bluemarble.dgml");
        else mapWidget->setMapThemeId("earth/plain/plain.dgml");
    };

    // Hide the FloatItems: OverviewMap, ScaleBar and Compass
    mapWidget->setShowOverviewMap(false);
    mapWidget->setShowScaleBar(false);
    mapWidget->setShowCompass(false);
    mapWidget->setShowCrosshairs(false);
    mapWidget->setShowGrid(true);
    mapWidget->setShowCities(false);
    mapWidget->setShowPlaces(false);
    mapWidget->setShowOtherPlaces(false);

    if (hc == 2) mapWidget->setShowClouds( true );

    // Create a horizontal zoom slider and set the default zoom
    QSlider * zoomSlider = new QSlider(Qt::Horizontal);
    zoomSlider->setMinimum( 1000 );
    zoomSlider->setMaximum( 3200 );
 
    mapWidget->zoomView( zoomSlider->value() );
 
    // Create a label to show the geodetic position
    QLabel * positionLabel = new QLabel();
    positionLabel->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );

    // Add all widgets to the vertical layout.
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(mapWidget);
    layout->addWidget(zoomSlider);
    layout->addWidget(positionLabel);

    // Center the map onto position of maximum eclipse
    ecps.getMaxPos(lat, lng);
    GeoDataCoordinates home(lng, lat, 0.0, GeoDataCoordinates::Degree);
    mapWidget->centerOn(home);
 
    // Connect the map widget to the position label.
    QObject::connect( mapWidget, SIGNAL( mouseMoveGeoPosition( QString ) ),
                      positionLabel, SLOT( setText( QString ) ) );
 
    // Connect the zoom slider to the map widget and vice versa.
    QObject::connect( zoomSlider, SIGNAL( valueChanged(int) ),
                      mapWidget, SLOT( zoomView(int) ) );
    QObject::connect( mapWidget, SIGNAL( zoomChanged(int) ),
                      zoomSlider, SLOT( setValue(int) ) );

    window->setLayout(layout);
    window->resize( 700, 500 );

    ecps.getEclTxt(0,wbuf);  // text for currently selected eclipse
    window->setWindowTitle(wbuf);
    window->show();

    zoomSlider->setValue(1080);

    return app.exec();

}

