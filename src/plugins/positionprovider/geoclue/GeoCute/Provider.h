//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Eckhart Wörner <ewoerner@kde.org>
//

#ifndef GEOCUTE_PROVIDER_H
#define GEOCUTE_PROVIDER_H

#include <QtCore/QObject>
#include <QtCore/QString>

#include "Status.h"



namespace GeoCute
{

class Provider : public QObject
{
    Q_OBJECT
    
    public:
        Provider(const QString& service, const QString& path,
            QObject* parent = 0);
        virtual ~Provider();
        Status status() const;

    signals:
        void statusChanged(GeoCute::Status status);
    
    private:
        class Private;
        Private* const d;
        Q_PRIVATE_SLOT(d, void statusChangedCall(int status))
};

}



#endif
