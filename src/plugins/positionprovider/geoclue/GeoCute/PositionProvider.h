//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Eckhart Wörner <ewoerner@kde.org>
//

#ifndef GEOCUTE_POSITIONPROVIDER_H
#define GEOCUTE_POSITIONPROVIDER_H

#include <QtCore/QObject>

#include "Provider.h"
#include "Position.h"



class QString;

namespace GeoCute
{

class PositionProvider : public Provider
{
    Q_OBJECT
    
    public:
        PositionProvider(const QString& service, const QString& path,
            QObject* parent = 0);
        ~PositionProvider();
        Position position() const;

        static PositionProvider* detailed();

    signals:
        void positionChanged(GeoCute::Position position);
        
    private:
        class Private;
        Private* const d;
        Q_PRIVATE_SLOT(d, void positionChangedCall(QDBusMessage message))
};

}



#endif
