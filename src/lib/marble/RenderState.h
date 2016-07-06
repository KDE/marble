//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_RENDERSTATE_H
#define MARBLE_RENDERSTATE_H

#include "marble_export.h"
#include "MarbleGlobal.h"

#include <QDebug>

namespace Marble {

class MARBLE_EXPORT RenderState
{
public:
    explicit RenderState( const QString &name = QString(), RenderStatus status=Complete );
    RenderState( const RenderState &other );
    RenderState& operator=( const RenderState &other );
    ~RenderState();

    RenderStatus status() const;
    QString name() const;
    int children() const;
    RenderState childAt( int index ) const;
    void addChild( const RenderState& child );
    operator QString() const;

private:
    class Private;
    Private* const d;
};

} // namespace Marble

#endif
