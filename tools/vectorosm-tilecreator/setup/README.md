# Server-side setup notes

Note: this has been deployed to maps.kde.org in November 2020.

## Overview

The outside interface for this is the standard [Slippy map](https://wiki.openstreetmap.org/wiki/Slippy_map_tilenames),
under `/earth/vectorosm/v1/`. Zoom levels 1, 3, 5, 7, 9, 11, 13, 15 and 17 are offered. The individual tiles are served
as [o5m](https://wiki.openstreetmap.org/wiki/O5m) encoded files.

Internally this is split into two parts:
* Staticly generated low-resolution tiles for zoom levels 1, 3, 5, 7 and 9, based on the [Natural Earth](https://www.naturalearthdata.com/)
data set. Those tiles exist in o5m format on disk in the exact layout they are served by the webserver.
* Dynamically generated high-resolution tiles for zoom levels 11, 13, 15 and 17. Creation and expiry of those is managed
by Tirex, and they are stored in the [metatile](https://wiki.openstreetmap.org/wiki/Tirex/Internals#Metatile_file_structure)
format (8x8 tiles in a single binary file in a 5 layer hashed folder structure). mod_tile takes care of translating that
to the outside interface.
* Input data for the dynamic generation: This is provided via an [OSMX](https://github.com/protomaps/OSMExpress)
database, which allows for fast spatial queries and efficient incremental updates.

## Dependencies

The following components are assumed to be on the server:
* Apache2
* mod_tile - https://wiki.openstreetmap.org/wiki/Mod_tile
* Tirex - https://wiki.openstreetmap.org/wiki/Tirex
* osmx and osmx-update - https://github.com/protomaps/OSMExpress (static binary of osmx available there, osmx-update is a Python script)
* marble-vectorosm-tirex-backend

The following components are needed for the static/low-z tile generation and can be run on a different machine:
* Python 3
* osmctools - https://gitlab.com/osm-c-tools/osmctools
* ogr2ogr from gdal (?)
* ne_tilegenerator.py
* marble-vectorosm-tilecreator
* marble-vectorosm-process-land-polygons

Precompiled packages:
* mod_tile: PPA by OSM admin team: https://launchpad.net/~osmadmins/+archive/ubuntu/ppa
* Tirex: PPA by the author: https://launchpad.net/~framm/+archive/ubuntu/tirex - unfortunately only for Ubuntu 18.04
  To work around this, Debian packages can be built by running `make` in the `build` sub-directory of this folder as well.

## Setup

See configuration files in the etc/ subdir.

### Static low-z tile generation

run ne_tilegenerator.py from ../natural-earth-vector-tiling. For this to work, all its dependencies (including marble-vectorosm-tilecreator)
have to be in `PATH`.

```
mkdir -p /k/osm/htdocs/earth/vectorosm/v1/
mkdir -p /k/osm/cache/natural_earth
./ne_tilegenerator.py -z 1,3,5,7,9 -f `pwd`/level_info.txt -o /k/osm/htdocs/earth/vectorosm/v1/ -i /k/osm/cache/natural_earth/ -c /k/osm/cache/natural_earth/ -r 30 -ow
```

TODO: this still generates files in its source dir, so probably this is better run inside the cache directory instead?

The source data updates infrequently, so a low-frequency cron job is an option. This can also be done locally or otherwise
off the live system, the resulting amount of data is small enough to be rsync'ed.

### Dynamic high-z tile generation

Preparing the land polygon input data by running:
`marble-vectorosm-process-land-polygons -c /k/osm/cache`

This step can also be done offline and the result copied to the production system.

Preparing the OSMX database:

* Download the latest full planet data dump (in PBF format!) from a mirror listed here: https://wiki.openstreetmap.org/wiki/Planet.osm
* Run `osmx expand planet.osm.pbf /k/osm/cache/planet.osmx` to create the OSMX database.
* The downloaded data dump can be discarded afterwards to free some disk space.

This step produces by far the most data, doing this on a different system is probably not feasible in most cases.

Initial pre-generation of level 11 tiles:

```
# North America
tirex-batch -f not-exists map=vectorosm/v1 x=310-680 y=660-940 z=11
# South America
tirex-batch -f not-exists map=vectorosm/v1 x=560-824 y=1024-1400 z=11
# North Africa, Asia, Europe
tirex-batch -f not-exists map=vectorosm/v1 x=920-2047 y=432-1000 z=11
# South Africa
tirex-batch -f not-exists map=vectorosm/v1 x=1072-1312 y=1000-1232 z=11
# Australia
tirex-batch -f not-exists map=vectorosm/v1 x=1560-2032 y=1000-1320 z=11
```

This enqueues batch jobs for generating all level 11 tiles that don't exist yet. Due to the existance filter this could be re-run
after every server restart for example without causing extra generation cost.

## Incremental Updates

Run the following command as a daily cron job (for server locations outside for central Europe pick a different mirror):

`osmx-update <path-to>/planet.osmx https://ftp5.gwdg.de/pub/misc/openstreetmap/planet.openstreetmap.org/replication/day/`

## Resource Requirements

For the static low-z tiles:
* 1.2GB disk space, 265k files, 700 directories, 260k inodes for the generated data
* Generation takes about 60-90min (single core), needs about 2GB of temporary disk space, a few 100MB download volume, and ~6GB RAM peak

For the dynamic high-z tiles (estimates and bounds, exact prediction is not possible here):
* Low-to medium density metatiles (batches of 64 tiles) generate in 100ms or less.
* High-density metatiles take ~15s - this is addressed by pre-generating the level 11 tiles initially.
* Amount of parallel processes used for generation can be adjusted in the Tirex config, each process only uses a single core.
* RAM peak should remain well below 1GB per generation process, exact amount varies with the level of detail of the processed tile.
* Disk space requirement for the generator output varies with access patterns:
    * Access stats from mid 2020 show 44k distinct tiles being used in a 2w period.
    * Metatiles of high-density areas are up to 1.5M in size, 10x less for lower-density areas.
    * Simply multiplying this results in 66GB and 44k files, however that assumes only distinct high-z tiles are requested.
    * The full world OSM data in o5m format is around 60GB as well, so that is a sensible upper bound for volume.
    * The theoretical upper bound for z17 files is 2^(2*17 - 6) = 268M, however even the
      [OSM access statistics](https://wiki.openstreetmap.org/wiki/Tile_disk_usage) only show about 2.5% of z17 tiles actually being loaded.
      It can further be assumed that tile access is not random but clustered, which further reduces the amount of metatiles need.
    * 10k to 1M files would therefore seem like the best guess for this.

For input data updates:
* Initial download of a full OSM dataset is about 60GB (available on several fast mirrors).
* Initial creation of the OSMX database takes 6h, needs 8GB RAM and generates 700GB on disk in a single file.
* Incremental updates: 100MB download and about 20s CPU time per day, and 6GB RAM peak during that.
* Land polygons:
    * 700MB download
    * 1GB disk space, 16k inodes
    * and an addtional 1.5GB temporary disk use during generation
    * generation takes 2-3 minutes and 4.5GB RAM

## OSMX Database Rebuilds

Incremental updates of the OSMX database seem to cause that to grow faster than what a clean import produces, and there is
no built-in database compaction command. We therefore have to rebuild it from scratch every couple of months to avoid running
out of disk space.

```
ssh mapsadmin@rhei.kde.org

# Make sure to start the lengthy process in a screen session independent of the SSH session
screen

# Check the scratch space has at least 1TB of free space
cd /srv/scratch/maps/

# Download the latest OSM data dump (~15min)
wget https://ftp5.gwdg.de/pub/misc/openstreetmap/planet.openstreetmap.org/pbf/planet-latest.osm.pbf

# Import the OSM dump into the OSMX database (6-9h)
/opt/osm/bin/osmx expand planet-latest.osm.pbf planet.osmx

# Replace the previous OSMX database with the new one (1-2h)
# The server is not able to generate new tiles during this period
cd /var/cache/tirex/cache/
rm planet.osmx planet.osmx-lock
cp /srv/scratch/maps/planet.osmx .

# Check that both the tirex and mapsadmin user can access the OSMX db ("osmx query planet.osmx" must not fail)
chmod 664 planet.osmx-lock
chown mapsadmin:_tirex planet.osmx-lock

# Clean up scratch space again
rm /srv/scratch/maps/planet.osmx /srv/scratch/maps/planet.osmx-lock /srv/scratch/maps/planet-latest.osm.pbf
```
