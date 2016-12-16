//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Wes Hardaker <hardaker@users.sourceforge.net>
//

#ifndef APRSTCPIP_H
#define APRSTCPIP_H

#include "AprsSource.h"
#include <QString>

namespace Marble {
    class AprsTCPIP : public AprsSource
    {
      public:
        AprsTCPIP( const QString& hostName, int port );
        ~AprsTCPIP() override;

        QString    sourceName() const override;
        QIODevice *openSocket() override;
        void       checkReadReturn( int length, QIODevice **socket,
                                    AprsGatherer *gatherer ) override;

        bool       canDoDirect() const override;

      private:
        QString m_hostName;
        int     m_port;
        int     m_numErrors;
    };
}

#endif /* APRSTCPIP_H */
