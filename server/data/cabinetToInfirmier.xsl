<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:cab="http://www.ujf-grenoble.fr/l3miage/cabinet" 
    xmlns:act="http://www.ujf-grenoble.fr/l3miage/actes"
    xmlns="http://www.w3.org/1999/xhtml"
>
    <xsl:output method="html"/>

    <xsl:param name="destinedId" select="001"/>
    <xsl:template match="/">
        <html>
            <head>
                <title>Votre tournée de patients aujourd'hui</title>
                <xsl:element name="link">
                    <xsl:attribute name="rel">stylesheet</xsl:attribute>
                    <xsl:attribute name="type">text/css</xsl:attribute>
                    <xsl:attribute name="href">css/mystyle.css</xsl:attribute>
                </xsl:element>
                <script type="text/javascript" src="js/lwUtils.js"/>
                <script type="text/javascript" src="js/facture.js"/>
                <script type="text/javascript">
                    <![CDATA[
                    function openFacture(prenom, nom, actes) {
                        var width  = 500;
                        var height = 300;
                        if(window.innerWidth) {
                            var left = (window.innerWidth-width)/2;
                            var top = (window.innerHeight-height)/2;
                        }
                        else
                        {
                            var left = (document.body.clientWidth-width)/2;
                            var top = (document.body.clientHeight-height)/2;
                        }
                        var factureWindow = window.open('','facture','menubar=yes, scrollbars=yes, top='+top+', left='+left+', width='+width+', height='+height+'');
                        var factureText = afficherFacture(prenom, nom, actes);
                        // var factureText = "Facture pour : " + prenom + " " + nom;
                        factureWindow.document.write(factureText);
                    }
                    ]]>
                </script>
               
            </head>
            <body>
                <xsl:apply-templates select="/cab:cabinet/cab:infirmiers/cab:infirmier"/>                
            </body>
        </html>
    </xsl:template>
    
    <xsl:template match="cab:infirmier">
        <!-- On sélectionne seulement l'infirmière avec le bon id -->
        
        <xsl:if test="@id=$destinedId">
            <h1>Bonjour <xsl:value-of select="cab:prénom"/></h1>
            
            <!-- On compte le nombre de patients -->
            <xsl:variable name="visitesDuJour" select="//cab:visite[@intervenant=$destinedId]"/>            
            <p> Aujourd'hui, vous avez <xsl:value-of select="count($visitesDuJour)"/> visites.</p>
            <ol>
                <xsl:apply-templates select="$visitesDuJour"/>
            </ol>
        </xsl:if>
</xsl:template>

<xsl:template match="cab:visite">
    <li>
        <xsl:value-of select="../cab:prénom"/>
        <xsl:text> </xsl:text>
        <xsl:value-of select="../cab:nom"/>
        <ul>
                <li>Domicilié au:<br/>
                    <xsl:apply-templates select="../cab:adresse"/>
                </li>
                <li>Actes à effecturer:
                    <ul>
                        <xsl:apply-templates select="cab:acte"/>
                    </ul>
                </li>
            </ul>                    
            <xsl:element name="button">
                <xsl:variable name="actes">
                    <xsl:for-each select="cab:acte"><xsl:value-of select="@id"/><xsl:text> </xsl:text></xsl:for-each>
                </xsl:variable>
                <xsl:attribute name="type">button</xsl:attribute>
                <xsl:attribute name="onclick">openFacture('<xsl:value-of select="../cab:prénom"/>', '<xsl:value-of select="../cab:nom"/>', '<xsl:value-of select="$actes"/>')</xsl:attribute> 
                Afficher la Facture
            </xsl:element>
    </li>
    
</xsl:template>
    
    <xsl:template match="cab:adresse">
        <xsl:value-of select="cab:numéro"/> <xsl:text> </xsl:text> <xsl:value-of select="cab:rue"/><br/>
        <xsl:value-of select="cab:codePostal"/> <xsl:text> </xsl:text> <xsl:value-of select="cab:ville"/>
    </xsl:template>

    <xsl:template match="cab:acte"> 
        <xsl:variable name="actes" select="document('actes.xml', /)/act:ngap"/>
        <xsl:variable name="acteId" select="@id"/>
        <xsl:variable name="theActe" select="$actes/act:actes/act:acte[@id=$acteId]"/>
        
        <li>
            <xsl:value-of select="$theActe"/>
        </li>
        
    </xsl:template>
    
</xsl:stylesheet>
