// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_DECLARATIVE_OBJECT_H
#define MARBLE_DECLARATIVE_OBJECT_H

#include <QObject>
#include <qqmlintegration.h>

class MarbleDeclarativeObjectPrivate;

/**
 * Provides access to global Marble related properties and methods
 * to QML (intended to be registered as a global QML object)
 */
class MarbleDeclarativeObject : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Marble)
    QML_SINGLETON

    Q_PROPERTY(QString version READ version CONSTANT)

public:
    explicit MarbleDeclarativeObject(QObject *parent = nullptr);

    ~MarbleDeclarativeObject() override;

    /** Returns the Marble library version */
    QString version() const;

public Q_SLOTS:
    /**
     * @brief resolvePath Resolves files in the marble data path
     * @param path Relative path to a file installed in Marble's data path
     * @return The absolute path to the given file, or an empty string if the
     * relative path cannot be resolved
     * @see MarbleDirs
     */
    QString resolvePath(const QString &path) const;

    /**
     * @brief canExecute Check whether a given program is found in the path
     * @param program The name of the program
     * @return Returns true iff the given program is found in one of
     * directories defined by the PATH environment variable (as reported by
     * QProcessEnvironment) and has the executable bit set
     */
    bool canExecute(const QString &program) const;

private:
    MarbleDeclarativeObjectPrivate *const d;
};

#endif
