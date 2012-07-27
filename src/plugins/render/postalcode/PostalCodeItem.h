//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Valery Kharitonov <kharvd@gmail.com>
//

#ifndef POSTALCODEITEM_H
#define POSTALCODEITEM_H

#include "AbstractDataPluginItem.h"

#include <QtGui/QFont>

namespace Marble
{

class PostalCodeItem : public AbstractDataPluginItem
{
    Q_OBJECT

 public:
    PostalCodeItem( QObject *parent );

    ~PostalCodeItem();

    QString itemType() const;

    bool initialized();

    void paint( GeoPainter *painter, ViewportParams *viewport,
                const QString& renderPos, GeoSceneLayer * layer = 0 );

    bool operator<( const AbstractDataPluginItem *other ) const;

    QString text() const;

    void setText( const QString& text );

 private:
    QString m_text;

    static const QFont s_font;
    static const int s_labelOutlineWidth;
};

}
#endif // POSTALCODEITEM_H
