//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Anton Chernov <chernov.anton.mail@gmail.com>
// Copyright 2012      "LOTES TM" LLC <lotes.sis@gmail.com>
//

#ifndef DECLARATIVEDATAPLUGINMODELITEM_H
#define DECLARATIVEDATAPLUGINMODELITEM_H

#include "AbstractDataPluginItem.h"

namespace Marble {

class DeclarativeDataPluginItemPrivate;

class DeclarativeDataPluginItem: public AbstractDataPluginItem
{
    Q_OBJECT

public:
    explicit DeclarativeDataPluginItem ( QObject *parent = 0 );

    ~DeclarativeDataPluginItem();

    virtual QString itemType() const;

    virtual bool initialized();

    bool operator<( const AbstractDataPluginItem *other ) const;
};

}
#endif // DECLARATIVEDATAPLUGINMODELITEM_H
