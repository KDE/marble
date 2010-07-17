//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Henry de Valence <hdevalence@gmail.com>

#ifndef MARBLE_ONFRUNNER_H
#define MARBLE_ONFRUNNER_H

#include "MarbleAbstractRunner.h"

#include <QtCore/QString>

class QHttp;
class QBuffer;

namespace Marble
{
//Openstreetmap Name Finder
class OnfRunner : public MarbleAbstractRunner
{
    Q_OBJECT
public:
    explicit OnfRunner(QObject *parent = 0);
    ~OnfRunner();
    GeoDataFeature::GeoDataVisualCategory category() const;

public slots:
    void slotRequestFinished( int id, bool error );

protected:
    virtual void run();

private:
    void fail();
    QHttp *m_http;
    QBuffer *m_buffer;
};

}

#endif
