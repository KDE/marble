//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Eckhart Wörner <ewoerner@kde.org>
//

#include "Accuracy.h"

#include <QtDBus/QDBusArgument>



using namespace GeoCute;

QDBusArgument& GeoCute::operator<<(QDBusArgument& argument,
    const Accuracy& accuracy) {
    argument.beginStructure();
    argument << accuracy.level << accuracy.horizontal << accuracy.vertical;
    argument.endStructure();
    return argument;
}

const QDBusArgument& GeoCute::operator>>(const QDBusArgument& argument,
    Accuracy& accuracy) {
    int level = static_cast<int>(accuracy.level);
    argument.beginStructure();
    argument >> level >> accuracy.horizontal >> accuracy.vertical;
    argument.endStructure();
    return argument;
}
