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
// Author: Torsten Rahn
//


#ifndef LEGENDBROWSER_H
#define LEGENDBROWSER_H


#include <QtGui/QTextBrowser>

#include "marble_export.h"

class MARBLE_EXPORT LegendBrowser : public QTextBrowser
{

    Q_OBJECT

 public:
    LegendBrowser( QWidget* parent );

 public Q_SLOTS:
    void setCheckedLocations( bool checked ){ m_checkBoxMap[ "locations" ] = checked; }
    void setCheckedCities( bool checked ){ m_checkBoxMap[ "cities" ] = checked; }
    void setCheckedTerrain( bool checked ){ m_checkBoxMap[ "terrain" ] = checked; }
    void setCheckedBorders( bool checked ){ m_checkBoxMap[ "borders" ] = checked; }
    void setCheckedWaterBodies( bool checked ){ m_checkBoxMap[ "waterbodies" ] = checked; }
    void setCheckedIceLayer( bool checked ){ m_checkBoxMap[ "ice" ] = checked; }
    void setCheckedGrid( bool checked ){ m_checkBoxMap[ "grid" ] = checked; }
    void setCheckedRelief( bool checked ){ m_checkBoxMap[ "relief" ] = checked; }
    void setCheckedWindRose( bool checked ){ m_checkBoxMap[ "windrose" ] = checked; }
    void setCheckedScaleBar( bool checked ){ m_checkBoxMap[ "scalebar" ] = checked; }

 Q_SIGNALS:
    void toggledLocations( bool );
    void toggledCities( bool );
    void toggledTerrain( bool );
    void toggledBorders( bool );
    void toggledWaterBodies( bool );
    void toggledIceLayer( bool );
    void toggledGrid( bool );
    void toggledRelief( bool );
    void toggledWindRose( bool );
    void toggledScaleBar( bool );

 private Q_SLOTS:
    void toggleCheckBoxStatus( QUrl );

 protected:
    QVariant loadResource ( int type, const QUrl & name );
    void sendSignals( QString name, bool checked );

    QMap<QString, bool> m_checkBoxMap;
};


#endif // LEGENDBROWSER_H
