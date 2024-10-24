// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#include "MarbleDeclarativeObject.h"

#include "MarbleDirs.h"
#include "MarbleGlobal.h"

#include <QProcessEnvironment>

class MarbleDeclarativeObjectPrivate
{
public:
    // Yet empty
};

MarbleDeclarativeObject::MarbleDeclarativeObject(QObject *parent)
    : QObject(parent)
    , d(new MarbleDeclarativeObjectPrivate)
{
    // nothing to do
}

MarbleDeclarativeObject::~MarbleDeclarativeObject()
{
    delete d;
}

QString MarbleDeclarativeObject::version() const
{
    return QString::fromLatin1(MARBLE_VERSION_STRING);
}

QString MarbleDeclarativeObject::resolvePath(const QString &path) const
{
    return Marble::MarbleDirs::path(path);
}

bool MarbleDeclarativeObject::canExecute(const QString &program) const
{
    QString path = QProcessEnvironment::systemEnvironment().value(QStringLiteral("PATH"), QStringLiteral("/usr/local/bin:/usr/bin:/bin"));
    for (const QString &dir : path.split(QLatin1Char(':'))) {
        QFileInfo const executable(QDir(dir), program);
        if (executable.exists() && executable.isExecutable()) {
            return true;
        }
    }

    return false;
}

#include "moc_MarbleDeclarativeObject.cpp"
