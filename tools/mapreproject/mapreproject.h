#ifndef MAPREPROJECT_H
#define MAPREPROJECT_H

enum InterpolationMethod { UnknownInterpolation,
                           NearestNeighborInterpolation,
                           BilinearInterpolation };

enum MapSourceType { UnknownMapSource,
                     NasaWorldWindMap,
                     BathymetryMap };

#endif
