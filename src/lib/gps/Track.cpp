#include "Track.h"

Track::Track() : AbstractLayerData(0,0)
{
    m_trackSegs = new QVector<WaypointContainer>();
}

Track::~Track()
{
}

void Track::addTrackSeg(WaypointContainer trackSeg)
{
    m_trackSegs->append(trackSeg);
}
