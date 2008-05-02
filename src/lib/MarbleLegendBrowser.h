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


#include <QtCore/QDebug>
#include <QtGui/QTextBrowser>

#include "marble_export.h"


class MarbleWidget;
class MarbleLegendBrowserPrivate;

class MARBLE_EXPORT MarbleLegendBrowser : public QTextBrowser
{
    Q_OBJECT

 public:
    MarbleLegendBrowser( QWidget* parent );
    ~MarbleLegendBrowser();

    void  setMarbleWidget( MarbleWidget *marbleWidget );

 public Q_SLOTS:
    void setCheckedLocations( bool checked );
    void setCheckedCities( bool checked );
    void setCheckedTerrain( bool checked );
    void setCheckedBorders( bool checked );
    void setCheckedWaterBodies( bool checked );
    void setCheckedIceLayer( bool checked );
    void setCheckedOtherPlaces( bool checked );
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
    void toggledOtherPlaces( bool );
    void toggledIceLayer( bool );
    void toggledGrid( bool );
    void toggledRelief( bool );
    void toggledCompass( bool );
    void toggledScaleBar( bool );

    void toggledShowProperty( QString, bool );

 private Q_SLOTS:
    void loadLegend();
    void test(){ qDebug() << "TEST"; }

    void toggleCheckBoxStatus( const QUrl &);

 protected:
    QString  readHtml( const QUrl & name );
    QString  generateSectionsHtml();
    void  translateHtml( QString & html );

    QVariant loadResource ( int type, const QUrl & name );
    void     sendSignals( const QString &name, bool checked );

 private:
    MarbleLegendBrowserPrivate  * const d; 
};


#endif // MARBLELEGENDBROWSER_H
