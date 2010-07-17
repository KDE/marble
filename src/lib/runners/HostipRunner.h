//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Dennis Nienhüser <earthwings@gentoo.org>

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
