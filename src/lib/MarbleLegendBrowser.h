//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

//
// The Legend Browser displays the legend
//


#ifndef MARBLELEGENDBROWSER_H
#define MARBLELEGENDBROWSER_H


#include <QtGui/QTextBrowser>

#include "marble_export.h"


class MarbleLegendBrowserPrivate;

class MARBLE_EXPORT MarbleLegendBrowser : public QTextBrowser
{
    Q_OBJECT

 public:
    MarbleLegendBrowser( QWidget* parent );

 public Q_SLOTS:
    void setCheckedLocations( bool checked );
    void setCheckedCities( bool checked );
    void setCheckedTerrain( bool checked );
    void setCheckedBorders( bool checked );
    void setCheckedWaterBodies( bool checked );
    void setCheckedIceLayer( bool checked );
    void setCheckedGrid( bool checked );
    void setCheckedRelief( bool checked );
    void setCheckedCompass( bool checked );
    void setCheckedScaleBar( bool checked );

 Q_SIGNALS:
    void toggledLocations( bool );
    void toggledCities( bool );
    void toggledTerrain( bool );
    void toggledBorders( bool );
    void toggledWaterBodies( bool );
    void toggledIceLayer( bool );
    void toggledGrid( bool );
    void toggledRelief( bool );
    void toggledCompass( bool );
    void toggledScaleBar( bool );

 private Q_SLOTS:
    void toggleCheckBoxStatus( const QUrl &);

 protected:
    QVariant loadResource ( int type, const QUrl & name );
    void sendSignals( const QString &name, bool checked );
    void readHtml( const QUrl & name );

 private:
    MarbleLegendBrowserPrivate  * const d; 
};


#endif // MARBLELEGENDBROWSER_H
