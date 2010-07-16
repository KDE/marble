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

#ifndef MARBLE_MARBLEABSTRACTRUNNER_H
#define MARBLE_MARBLEABSTRACTRUNNER_H

#include "GeoDataFeature.h"
#include "GeoDataPlacemark.h"

#include <QtCore/QThread>
#include <QtCore/QVector>

class QAbstractItemModel;
class QString;

namespace Marble
{

class MarbleMap;
class GeoDataPlacemark;

/**
 * This class is the base class for all Marble Runners.
 */
class MarbleAbstractRunner : public QThread
{
    Q_OBJECT
public:
    /**
     * Constructor; note that @p parent should be 0 in order to move the
     * thread object into its own thread.
     */
    explicit MarbleAbstractRunner(QObject *parent = 0);
    /**
     * This function gives the  icon for this runner
     * @return the icon of the runner
     */
    virtual GeoDataFeature::GeoDataVisualCategory category() const;

    void setMap(MarbleMap * map);
    MarbleMap * map();

public slots:
    /**
     * This function does the actual parsing. This should emit runnerStarted()
     * at the beginning and emit runnerFinished() to give the result back. Note
     * that there <b>must</b> be one runnerFinished() emitted for each runnerStarted()
     * emitted.
     * @p input the string to be parsed
     */
    virtual void parse(const QString &input);

protected:
    QString m_input;
    QAbstractItemModel * m_placemarkModel;
    MarbleMap * m_map;

signals:
    /**
     * This is emitted to indicate that the runner has started to work.
     * @see parse()
     */
    void runnerStarted();
    /**
     * This is emitted to indicate that the runner has finished.
     * @param the address of the runner itself
     * @param result the result of the parsing.
     * @see parse()
     */
    void runnerFinished(MarbleAbstractRunner* runner, QVector<GeoDataPlacemark*> result);
};

}

#endif
