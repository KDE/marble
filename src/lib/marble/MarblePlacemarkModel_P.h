//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
//

#ifndef MARBLE_MARBLEPLACEMARKMODEL_P_H
#define MARBLE_MARBLEPLACEMARKMODEL_P_H

#include <QChar>
#include <QRegExp>
#include <QString>

namespace Marble
{

namespace GeoString
{
    static const QRegExp combiningDiacriticalMarks("[\\x0300-\\x036F]+");

    QString deaccent( const QString& accentString )
    {
        QString    result;

        result = accentString.normalized( QString::NormalizationForm_D ).remove( combiningDiacriticalMarks );
        result.replace(QChar(0x00F8), QLatin1Char('o'));
        result.replace(QChar(0x0142), QLatin1Char('l'));
        return result;
    }
}
}

#endif
