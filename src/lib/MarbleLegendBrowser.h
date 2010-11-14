//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

//
// The Legend Browser displays the legend
//

#ifndef MARBLE_MARBLELEGENDBROWSER_H
#define MARBLE_MARBLELEGENDBROWSER_H


#include <QtCore/QString>
#include <QtGui/QTextBrowser>

#include "marble_export.h"

class QEvent;
class QUrl;

namespace Marble
{

class MarbleWidget;
class MarbleLegendBrowserPrivate;

class MARBLE_EXPORT MarbleLegendBrowser : public QTextBrowser
{
    Q_OBJECT

 public:
    explicit MarbleLegendBrowser( QWidget* parent );
    ~MarbleLegendBrowser();

    void  setMarbleWidget( MarbleWidget *marbleWidget );

 public Q_SLOTS:
    void setCheckedProperty( const QString& name, bool checked );

 Q_SIGNALS:
/*
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
*/
    void toggledShowProperty( QString, bool );

 private Q_SLOTS:
    void initTheme();

    void loadLegend();

    void toggleCheckBoxStatus( const QUrl &);

 protected:
    bool event( QEvent * event );
    QString  readHtml( const QUrl & name );
    QString  generateSectionsHtml();
    void  translateHtml( QString & html );

    QVariant loadResource ( int type, const QUrl & name );

 private:
    Q_DISABLE_COPY( MarbleLegendBrowser )
    MarbleLegendBrowserPrivate  * const d; 
};

}

#endif
