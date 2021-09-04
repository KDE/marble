//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Eckhart Wörner <ewoerner@kde.org>
//

#ifndef GEOCUTE_PROVIDER_H
#define GEOCUTE_PROVIDER_H

#include <QObject>

#include "Status.h"

class QString;

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

    Q_SIGNALS:
        void statusChanged(GeoCute::Status status);
    
    private:
        class Private;
        Private* const d;
        Q_PRIVATE_SLOT(d, void statusChangedCall(int status))
};

}



#endif
