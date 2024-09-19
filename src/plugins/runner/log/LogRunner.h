// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>

#ifndef MARBLE_LOG_RUNNER_H
#define MARBLE_LOG_RUNNER_H

#include "ParsingRunner.h"

namespace Marble
{

class LogRunner : public ParsingRunner
{
    Q_OBJECT

public:
    explicit LogRunner(QObject *parent = nullptr);
    ~LogRunner() override;

    GeoDataDocument *parseFile(const QString &fileName, DocumentRole role, QString &error) override;
};

}

#endif // MARBLE_LOGFILE_RUNNER_H
