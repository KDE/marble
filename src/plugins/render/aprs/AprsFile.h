//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Wes Hardaker <hardaker@users.sourceforge.net>
//

#ifndef APRSFILE_H
#define APRSFILE_H

#include "AprsSource.h"
#include <QtCore/QString>

namespace Marble {
class AprsFile : public AprsSource
    {
      public:
        explicit AprsFile( const QString &fileName );
        ~AprsFile();

        QString    sourceName() const;
        QIODevice *openSocket();
        void       checkReadReturn( int length, QIODevice **socket,
                                    AprsGatherer *gatherer );

        bool       canDoDirect() const;

      private:
        QString m_fileName;
        int     m_errorCount;
    };
}

#endif /* APRSFILE_H */
