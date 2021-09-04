// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Valery Kharitonov <kharvd@gmail.com>
//

#ifndef POSTALCODEITEM_H
#define POSTALCODEITEM_H

#include "AbstractDataPluginItem.h"

#include <QFont>

namespace Marble
{

class PostalCodeItem : public AbstractDataPluginItem
{
    Q_OBJECT

 public:
    explicit PostalCodeItem( QObject *parent );

    ~PostalCodeItem() override;

    bool initialized() const override;

    void paint( QPainter *painter ) override;

    bool operator<( const AbstractDataPluginItem *other ) const override;

    QString text() const;

    void setText( const QString& text );

 private:
    QString m_text;

    static const QFont s_font;
    static const int s_labelOutlineWidth;
};

}
#endif // POSTALCODEITEM_H
