//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2013      Yazeed Zoabi      <yazeedz.zoabi@gmail.com>
//

//
// The Legend Browser displays the legend
//

#ifndef MARBLE_MARBLELEGENDBROWSER_H
#define MARBLE_MARBLELEGENDBROWSER_H

#ifdef MARBLE_NO_WEBKITWIDGETS
#include "NullMarbleWebView.h"
#else
#include "MarbleWebView.h"
#endif

#include "marble_export.h"

class QEvent;
class QUrl;
class QString;

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
    QSize sizeHint() const;

 public Q_SLOTS:
    void setCheckedProperty( const QString& name, bool checked );
    void setRadioCheckedProperty( const QString& value,const QString& name, bool checked );


 Q_SIGNALS:
    void toggledShowProperty( const QString&, bool );
    void tourLinkClicked( const QString &url );

 private Q_SLOTS:
    void initTheme();
    void loadLegend();
    void injectCheckBoxChecker();
    void openLinkExternally( const QUrl &url );

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
