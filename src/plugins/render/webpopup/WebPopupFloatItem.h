//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012   Mohammed Nafees   <nafees.technocool@gmail.com>
//

#ifndef WEBPOPUPFLOATITEM_H
#define WEBPOPUPFLOATITEM_H

#include "RenderPlugin.h"
#include "ViewportParams.h"
#include "GeoPainter.h"

#include <QtCore/QObject>
#include <QtCore/QUrl>

namespace Marble
{

class PopupItem;

class WebPopupFloatItem : public RenderPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN( WebPopupFloatItem )
public:
    explicit WebPopupFloatItem( const MarbleModel *marbleModel = 0 );
    ~WebPopupFloatItem();

    QStringList backendTypes() const;
    QString name() const;
    QString guiString() const;
    QString nameId() const;
    QString version() const;
    QString description() const;
    QString copyrightYears() const;
    QList<PluginAuthor> pluginAuthors() const;
    QIcon icon () const;
    void initialize ();
    bool isInitialized () const;
    QStringList renderPosition() const;
    QString renderPolicy() const;
    bool render( GeoPainter *painter, ViewportParams *viewport,
                 const QString &, GeoSceneLayer * );
    virtual bool eventFilter( QObject *, QEvent * );

    void setCoordinates( const GeoDataCoordinates &coordinates );
    void setUrl( const QUrl &url );
    void setContent( const QString &html );

private:
    PopupItem *m_popupItem;
    GeoDataCoordinates m_coordinates;
    QUrl m_url;
    QString m_content;
};

}

#endif
