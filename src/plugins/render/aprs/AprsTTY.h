//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Wes Hardaker <hardaker@users.sourceforge.net>
//

#ifndef APRSTTY_H
#define APRSTTY_H

#include "AprsSource.h"
#include <QtCore/QString>

namespace Marble {
    class AprsTTY : public AprsSource
    {
      public:
        AprsTTY( QString ttyName );
        ~AprsTTY();

        QString sourceName() const;
        QIODevice *openSocket();
        void       checkReadReturn( int length, QIODevice **socket,
                                    AprsGatherer *gatherer );

        bool       canDoDirect() const;

      private:
        QString m_ttyName;
        int     m_numErrors;
    };
}

#endif /* APRSTTY_H */
//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Wes Hardaker <hardaker@users.sourceforge.net>
//

#ifndef APRSTTY_H
#define APRSTTY_H

#include "AprsSource.h"
#include <QtCore/QString>

namespace Marble {
    class AprsTTY : public AprsSource
    {
      public:
        AprsTTY( QString ttyName );
        ~AprsTTY();

        QString sourceName() const;
        QIODevice *openSocket();
        void       checkReadReturn( int length, QIODevice **socket,
                                    AprsGatherer *gatherer );

        bool       canDoDirect() const;

      private:
        QString m_ttyName;
        int     m_numErrors;
    };
}

#endif /* APRSTTY_H */
