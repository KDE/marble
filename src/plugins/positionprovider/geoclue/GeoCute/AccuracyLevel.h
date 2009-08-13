#ifndef GEOCUTE_ACCURACYLEVEL_H
#define GEOCUTE_ACCURACYLEVEL_H



namespace GeoCute
{

enum AccuracyLevel {
    AccuracyLevelNone = 0,
    AccuracyLevelCountry,
    AccuracyLevelRegion,
    AccuracyLevelLocality,
    AccuracyLevelPostalCode,
    AccuracyLevelStreet,
    AccuracyLevelDetailed
};

}



#endif
