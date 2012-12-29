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

#include "LayerInterface.h"
#include "ViewportParams.h"
#include "GeoPainter.h"

#include <QtCore/QObject>
#include <QtCore/QUrl>

namespace Marble
{

class PopupItem;
class MarbleModel;

class MARBLE_EXPORT MapInfoDialog : public QObject, public LayerInterface
{
    Q_OBJECT
public:
    explicit MapInfoDialog( QObject* parent = 0 );
    ~MapInfoDialog();

    QStringList renderPosition() const;
    QString renderPolicy() const;
    bool render( GeoPainter *painter, ViewportParams *viewport,
                 const QString &, GeoSceneLayer * );
    virtual bool eventFilter( QObject *, QEvent * );
    qreal zValue() const;

    bool visible() const;
    void setVisible( bool visible );

    void setCoordinates( const GeoDataCoordinates &coordinates, Qt::Alignment alignment );
    void setUrl( const QUrl &url );
    void setSize( const QSizeF &size );
    void setPosition( const QPointF &position );
    void setContent( const QString &html );
    void setBackgroundColor( const QColor &color );
    void setTextColor( const QColor &color );

Q_SIGNALS:
    void repaintNeeded();

public slots:
    void hidePopupItem();

private:
    PopupItem *m_popupItem;
};

}

#endif
