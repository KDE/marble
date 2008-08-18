/*
    Copyright 2008 Henry de Valence <hdevalence@gmail.com>
    
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public 
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MARBLEABSTRACTRUNNER_H
#define MARBLEABSTRACTRUNNER_H

#include "MarbleRunnerResult.h"

#include <QThread>

class QString;

namespace Marble
{
/**
 * This class is the base class for all Marble Runners. 
 */
class MarbleAbstractRunner : public QThread
{
    Q_OBJECT
public:
    /**
     * Constructor; note that @p parent should be 0
     */
    MarbleAbstractRunner(QObject *parent = 0);
    /**
     * Destructor.
     */
    ~MarbleAbstractRunner();
    /**
     * This function should return the user-visible name for this runner
     */
    virtual QString name() const;
    
public slots:
    /**
     * This function does the actual parsing. This should emit runnerStarted()
     * at the beginning and emit runnerFinished() to give the result back. Note
     * that there <b>must</b> be one runnerFinished() emitted for each runnerStarted()
     * emitted.
     * @p input the string to be parsed
     */
    virtual void parse(const QString &input);
    
signals:
    /**
     * This is emitted to indicate that the runner has started to work.
     * @param runnerName the user-visible name for the runner.
     * @see parse()
     * @see name()
     */
    void runnerStarted(QString runnerName);
    /**
     * This is emitted to indicate that the runner has finished.
     * If the parsing failed, @p result should have MarbleRunnerResult::NoMatch
     * @param result the result of the parsing.
     * @see parse()
     */
    void runnerFinished(MarbleRunnerResult result);
};

}

#endif
