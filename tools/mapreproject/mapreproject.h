#ifndef MAPREPROJECT_H
#define MAPREPROJECT_H

enum EInterpolationMethod { UnknownInterpolationMethod,
                            IntegerInterpolationMethod,
                            NearestNeighborInterpolationMethod,
                            AverageInterpolationMethod,
                            BilinearInterpolationMethod };

enum MapSourceType { UnknownMapSource,
                     NasaWorldWindMap,
                     BathymetryMap };

#endif
