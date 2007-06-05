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

 public slots:
    void setCheckedLocations( bool checked ){ m_checkBoxMap[ "locations" ] = checked; }
    void setCheckedBorders( bool checked ){ m_checkBoxMap[ "borders" ] = checked; }
    void setCheckedWaterBodies( bool checked ){ m_checkBoxMap[ "waterbodies" ] = checked; }

 signals:
    void toggledLocations( bool );
    void toggledBorders( bool );
    void toggledWaterBodies( bool );

 private slots:
    void toggleCheckBoxStatus( QUrl );

 protected:
    QVariant loadResource ( int type, const QUrl & name );
    void sendSignals( QString name, bool checked );

    QMap<QString, int> m_checkBoxMap;
};


#endif // LEGENDBROWSER_H
