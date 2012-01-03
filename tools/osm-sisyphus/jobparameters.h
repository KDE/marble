//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef JOBPARAMETERS_H
#define JOBPARAMETERS_H

#include <QtCore/QDir>

class JobParameters
{
public:
    JobParameters();

    QDir base();

    void setBase(const QDir &dir);

    bool cacheData() const;

    void setCacheData(bool cache);

private:
    QDir m_base;

    bool m_cacheData;
};

#endif // JOBPARAMETERS_H
