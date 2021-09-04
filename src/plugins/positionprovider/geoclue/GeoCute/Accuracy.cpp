//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Eckhart Wörner <ewoerner@kde.org>
//

#include "Accuracy.h"

#include <QDBusArgument>



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
