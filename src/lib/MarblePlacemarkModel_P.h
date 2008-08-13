//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
//

#ifndef MARBLEPLACEMARKMODEL_P_H
#define MARBLEPLACEMARKMODEL_P_H

#include <QtCore/QDebug>
#include <QtCore/QRegExp>

namespace Marble
{

namespace GeoString
{
    static const QRegExp combiningDiacriticalMarks("[\\x0300-\\x036F]+");

    QString deaccent( const QString& accentString )
    {
        QString    result;

        result = accentString.normalized( QString::NormalizationForm_D ).remove( combiningDiacriticalMarks );
        result = result.replace(QChar(0x00F8),'o');
        result = result.replace(QChar(0x0142),'l');
        return result;
    }
}
}

#endif
