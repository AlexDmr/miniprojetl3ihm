<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:cb='http://www.ujf-grenoble.fr/l3miage/cabinet'
    xmlns='http://www.ujf-grenoble.fr/l3miage/cabinet'
    xmlns:ac='http://www.ujf-grenoble.fr/l3miage/actes'
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    version="1.0">
    <xsl:output method="xml"/>
   
    <xsl:template match="/">
        <xsl:element name="infirmiers">
            <xsl:apply-templates select="cb:cabinet/cb:infirmiers/cb:infirmier"/>
        </xsl:element>        
    </xsl:template>
    
    <xsl:template match="cb:infirmier">
        <xsl:element name="infirmier">
            <xsl:element name="nom"><xsl:value-of select="cb:nom"/></xsl:element>
            <xsl:element name="prénom"><xsl:value-of select="cb:prénom"/></xsl:element>
            <xsl:element name="photo"><xsl:value-of select="cb:photo"/></xsl:element>
            <xsl:element name="visites">
                <xsl:apply-templates select="//cb:visite">
                    <xsl:with-param name="infirmierId"><xsl:value-of select="@id"/></xsl:with-param>
                    <xsl:sort select="@date"/>
                </xsl:apply-templates>
            </xsl:element>
        </xsl:element>
    </xsl:template>

    <xsl:template match="cb:visite">
        <xsl:param name="infirmierId"/>
        <xsl:if test="@intervenant=$infirmierId">
            <xsl:variable name="actes" select="document('actes.xml', /)/ac:ngap"/>
        <xsl:variable name="acteId" select="cb:acte/@id"/>
        <xsl:variable name="theActe" select="$actes/ac:actes/ac:acte[@id=$acteId]"/>
        
            <xsl:element name="visite">
                <xsl:attribute name="date"><xsl:value-of select="@date"/></xsl:attribute>
                <xsl:element name="patient">
                    <xsl:element name="nom"><xsl:value-of select="../../cb:nom"/></xsl:element>
                    <xsl:element name="prénom"><xsl:value-of select="../../cb:prénom"/></xsl:element>
                    <xsl:element name="adresse">
                        <xsl:copy-of select="../../cb:adresse"/>
                    </xsl:element>
                </xsl:element>
                <xsl:element name="soin"><xsl:value-of select="$theActe"/></xsl:element>
            </xsl:element>
        </xsl:if>
    </xsl:template>
</xsl:stylesheet>
