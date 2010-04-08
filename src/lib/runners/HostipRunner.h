/*
    Copyright 2010 Dennis Nienhüser <earthwings@gentoo.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MARBLE_HOSTIPRUNNER_H
#define MARBLE_HOSTIPRUNNER_H

#include "MarbleAbstractRunner.h"

#include <QtCore/QString>
#include <QtNetwork/QHostInfo>

class QNetworkReply;

namespace Marble
{

class HostipRunner : public MarbleAbstractRunner
{
    Q_OBJECT
public:
    explicit HostipRunner(QObject *parent = 0);

    ~HostipRunner();

    // Overriding MarbleAbstractRunner
    GeoDataFeature::GeoDataVisualCategory category() const;

private Q_SLOTS:
    // Http request with hostip.info done
    void slotRequestFinished( QNetworkReply* );

    // IP address lookup finished
    void slotLookupFinished(const QHostInfo &host);

    // No results (or an error)
    void slotNoResults();

protected:
    // Overriding QThread
    virtual void run();

private:
    QHostInfo m_hostInfo;
};

}

#endif
