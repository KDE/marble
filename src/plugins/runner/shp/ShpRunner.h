// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLESHPRUNNER_H
#define MARBLESHPRUNNER_H

#include "ParsingRunner.h"

namespace Marble
{

class ShpRunner : public ParsingRunner
{
    Q_OBJECT
public:
    explicit ShpRunner(QObject *parent = nullptr);
    ~ShpRunner() override;
    GeoDataDocument *parseFile(const QString &fileName, DocumentRole role, QString &error) override;
};

}
#endif // MARBLESHPRUNNER_H
