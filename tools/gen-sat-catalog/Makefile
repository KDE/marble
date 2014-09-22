# Planetary satellite catalog makefile
# René Küttner <rene@bitkanal.net>

.PHONY: PlanetarySatellites.xml

PlanetarySatellites.msc: PlanetarySatellites.xml
	@xsltproc to-legacy-msc.xsl $< > $@
	@echo Catalog written to $@.

PlanetarySatellites.xml: gen-sat-catalog.py
	@echo Collecting satellite data...
	@./gen-sat-catalog.py

clean:
	@rm -f PlanetarySatellites.xml PlanetarySatellites.msc
