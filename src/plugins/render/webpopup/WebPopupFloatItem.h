//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012   Mohammed Nafees   <nafees.technocool@gmail.com>
// Copyright 2012   Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#ifndef WEBPOPUPFLOATITEM_H
#define WEBPOPUPFLOATITEM_H

#include "RenderPlugin.h"
#include "ViewportParams.h"
#include "GeoPainter.h"
#include "AbstractInfoDialog.h"

#include <QtCore/QObject>
#include <QtCore/QUrl>

namespace Marble
{

class PopupItem;

class WebPopupFloatItem : public RenderPlugin, public AbstractInfoDialog
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
    qreal zValue() const;

    void setCoordinates( const GeoDataCoordinates &coordinates, Qt::Alignment alignment );
    void setUrl( const QUrl &url );
    void setSize( const QSizeF &size );
    void setPosition( const QPointF &position );
    void setContent( const QString &html );
    void setBackgroundColor( const QColor &color );
    void setTextColor( const QColor &color );

public slots:
    void hidePopupItem();

private:
    PopupItem *m_popupItem;
};

}

#endif
