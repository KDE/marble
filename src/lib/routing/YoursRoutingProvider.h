//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_YOURSROUTINGPROVIDER_H
#define MARBLE_YOURSROUTINGPROVIDER_H

#include "AbstractRoutingProvider.h"

#include <QtNetwork/QNetworkReply>

class QNetworkAccessManager;

namespace Marble {

/**
  * @brief A AbstractRoutingProvider that retrieves routing
  * directions from cloudmade.com via http.
  */
class YoursRoutingProvider : public AbstractRoutingProvider
{
    Q_OBJECT

public:
    /** Constructor */
    explicit YoursRoutingProvider( QObject *parent = 0 );

    /** Overload of AbstractRoutingProvider */
    virtual void retrieveDirections( RouteSkeleton* route );

private Q_SLOTS:
    /** Route data was retrieved via http */
    void retrieveData( QNetworkReply* reply );

    /** A network error occurred */
    void handleError( QNetworkReply::NetworkError );

private:
    QNetworkAccessManager *m_networkAccessManager;
};

} // namespace Marble


#endif
