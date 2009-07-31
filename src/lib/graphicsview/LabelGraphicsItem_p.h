//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef LABELGRAPHICSITEMPRIVATE_H
#define LABELGRAPHICSITEMPRIVATE_H

#include <QtCore/QString>

class QFont;

namespace Marble
{

class LabelGraphicsItemPrivate
{
 public:
    LabelGraphicsItemPrivate();

    // TODO: This has to go up to MarbleGraphicsItem
    QFont font() const;

    QString m_text;
};

} // namespace Marble

#endif // LABELGRAPHICSITEMPRIVATE_H
