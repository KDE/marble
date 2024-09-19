// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#ifndef KMLSOUNDCUETAGHANDLER_H
#define KMLSOUNDCUETAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmlSoundCueTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &parser) const override;
};

} // namespace kml
} // namespace Marble

#endif // KMLSOUNDCUETAGHANDLER_H
