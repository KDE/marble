// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mohammed Nafees <nafees.technocool@gmail.com>

#ifndef MARBLE_GPSBABEL_RUNNER_H
#define MARBLE_GPSBABEL_RUNNER_H

#include "ParsingRunner.h"

namespace Marble
{

class GpsbabelRunner : public ParsingRunner
{
    Q_OBJECT

public:
    explicit GpsbabelRunner( QObject *parent = nullptr );

    GeoDataDocument* parseFile( const QString &fileName, DocumentRole role, QString& error ) override;

};

}

#endif // MARBLE_GPSBABEL_RUNNER_H
