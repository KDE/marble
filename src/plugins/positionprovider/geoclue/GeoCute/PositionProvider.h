// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Eckhart Wörner <ewoerner@kde.org>
//

#ifndef GEOCUTE_POSITIONPROVIDER_H
#define GEOCUTE_POSITIONPROVIDER_H

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

    Q_SIGNALS:
        void positionChanged(GeoCute::Position position);
        
    private:
        class Private;
        Private* const d;
        Q_PRIVATE_SLOT(d, void positionChangedCall(QDBusMessage message))
};

}



#endif
