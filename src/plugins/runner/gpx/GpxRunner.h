// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLEGPXRUNNER_H
#define MARBLEGPXRUNNER_H

#include "ParsingRunner.h"

namespace Marble
{

class GpxRunner : public ParsingRunner
{
    Q_OBJECT
public:
    explicit GpxRunner(QObject *parent = nullptr);
    ~GpxRunner() override;
    GeoDataDocument *parseFile(const QString &fileName, DocumentRole role, QString &error) override;
};

}
#endif // MARBLEGPXRUNNER_H
