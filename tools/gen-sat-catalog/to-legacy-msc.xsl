<?xml version="1.0" encoding="utf-8" ?>
<!--
    vim: et tabstop=4 shiftwidth=4

    This file is part of the Marble Virtual Globe.

    This file is free software licensed under the GNU LGPL. You can
    find a copy of this license in LICENSE.txt in the top directory of
    the source code.

    This XSL stylesheet transforms the XML version of a Marble Satellite
    Catalog to the deprecated plain text comma separated format. It is
    used on marble servers to provide a legacy version of the satellite
    catalog for outdated versions of Marble's satellites plugin.

    In order to transform an XML catalog, use your favorite xslt processor.
    With 'xsltproc' the command would look as follows:

        xsltproc to-legacy-msc.xsl PlanetarySatellites.xml \
            > PlanetarySatellites.msc

    Copyright 2013 Rene Kuettner <rene@bitkanal.net>
-->
<xsl:stylesheet
    version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:msc="http://marble.kde.org/satellitecatalog">

    <xsl:output method="text"
                media-type="text/plain"
                encoding="utf-8" 
                omit-xml-declaration="yes"
                indent="no" />
    <xsl:strip-space elements="*" />

    <xsl:template match="msc:satellite">
        <xsl:value-of select="msc:name" />
        <xsl:text>, </xsl:text>
        <xsl:value-of select="msc:category" />
        <xsl:text>, </xsl:text>
        <xsl:value-of select="msc:relatedBody" />
        <xsl:text>, </xsl:text>
        <xsl:if test="msc:mission/msc:start">
            <xsl:value-of select="msc:mission/msc:start" />
        </xsl:if>
        <xsl:text>, </xsl:text>
        <t><xsl:if test="msc:mission/msc:end">
            <xsl:value-of select="msc:mission/msc:end" />
        </xsl:if>
        <xsl:text>, -, -, </xsl:text>
        <xsl:value-of select="msc:stateVector/@mjd" />, </t>
        <xsl:value-of select="msc:stateVector/msc:position/@x" />
        <xsl:text>, </xsl:text>
        <xsl:value-of select="msc:stateVector/msc:position/@y" />
        <xsl:text>, </xsl:text>
        <xsl:value-of select="msc:stateVector/msc:position/@z" />
        <xsl:text>, </xsl:text>
        <xsl:value-of select="msc:stateVector/msc:velocity/@x" />
        <xsl:text>, </xsl:text>
        <xsl:value-of select="msc:stateVector/msc:velocity/@y" />
        <xsl:text>, </xsl:text>
        <xsl:value-of select="msc:stateVector/msc:velocity/@z" />
        <xsl:text>&#xA;</xsl:text>
    </xsl:template>

    <xsl:template match="msc:MarbleSatelliteCatalog">
        <xsl:text><![CDATA[# name, category, rel_body, m_start, ]]></xsl:text>
        <xsl:text><![CDATA[m_end, -, -, <mjd>, <state_vector>]]></xsl:text>
        <xsl:text>&#xA;</xsl:text>
        <xsl:apply-templates />
    </xsl:template>

</xsl:stylesheet>
