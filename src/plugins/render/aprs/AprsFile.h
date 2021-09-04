//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Wes Hardaker <hardaker@users.sourceforge.net>
//

#ifndef APRSFILE_H
#define APRSFILE_H

#include "AprsSource.h"
#include <QString>

namespace Marble {
class AprsFile : public AprsSource
    {
      public:
        explicit AprsFile( const QString &fileName );
        ~AprsFile() override;

        QString    sourceName() const override;
        QIODevice *openSocket() override;
        void       checkReadReturn( int length, QIODevice **socket,
                                    AprsGatherer *gatherer ) override;

        bool       canDoDirect() const override;

      private:
        QString m_fileName;
        int     m_errorCount;
    };
}

#endif /* APRSFILE_H */
