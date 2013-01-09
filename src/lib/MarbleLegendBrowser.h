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


#include <QString>
#include "MarbleWebView.h"

#include "marble_export.h"

class QEvent;
class QUrl;

namespace Marble
{

class MarbleModel;
class MarbleLegendBrowserPrivate;

class MARBLE_EXPORT MarbleLegendBrowser : public MarbleWebView
{
    Q_OBJECT

 public:
    explicit MarbleLegendBrowser( QWidget* parent );
    ~MarbleLegendBrowser();

    void setMarbleModel( MarbleModel *marbleModel );

 public slots:
    void setCheckedProperty( const QString& name, bool checked );

 signals:
    void toggledShowProperty( QString, bool );

 private Q_SLOTS:
    void initTheme();
    void loadLegend();
    void injectCheckBoxChecker();

 protected:
    bool event( QEvent * event );
    QString  readHtml( const QUrl & name );
    QString  generateSectionsHtml();
    void  translateHtml( QString & html );

 private:
    void reverseSupportCheckboxes( QString &html );

 private:
    Q_DISABLE_COPY( MarbleLegendBrowser )
    MarbleLegendBrowserPrivate  * const d; 
};

}

#endif
