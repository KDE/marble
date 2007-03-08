#!/bin/sh

perl -p0777i -e 's/(\s+)?(sodipodi|inkscape):[a-z\-]+?=".+?"//mig' *.svg
perl -p0777i -e 's/(\s+)?xmlns:(sodipodi|inkscape)=".+?"//mig' *.svg

# rounding off the paths
perl -pi -e 's/([ \t][0-9])([0-9\.]{4})[0-9\.]+,([0-9])([0-9\.]{4})[0-9\.]+/\1\2,\3\4/mig' *.svg
perl -pi -e 's/stroke\-width:([0-9])([0-9\.]{4})[0-9\.]+/stroke-width:\1\2/mig' *.svg
