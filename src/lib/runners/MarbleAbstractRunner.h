//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Henry de Valence <hdevalence@gmail.com>

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
