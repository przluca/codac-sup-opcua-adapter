<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0" xmlns="http://www.w3.org/2005/07/scxml" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:xmi="http://schema.omg.org/spec/XMI/2.1" xmlns:uml="http://www.eclipse.org/uml2/3.0.0/UML" xmlns:scxml="http://www.w3.org/2005/07/scxml">
    <!-- OUTPUT Control -->
    <xsl:output method="text" indent="yes" encoding="UTF-8"/>    
    <xsl:param name="top_state"/>
    
    <!-- Search the model for all interfaces -->
    <xsl:template match="/">
        <xsl:apply-templates select="//packagedElement[@xmi:type='uml:Interface']|//nestedClassifier[@xmi:type='uml:Interface']" mode="Interface"/>
    </xsl:template>
    

    <!-- For each interface, create a header file -->
    <xsl:template match="packagedElement|nestedClassifier" mode="Interface">
        <xsl:value-of select="concat('// Found Interface:', @name, '&#xa;')"/>
        <!-- OUTPUT Control -->
        <xsl:variable name="name" select="@name"/>
        <xsl:result-document href="{@name}.h" method="text">
            <xsl:value-of select="concat('#ifndef ', $name, '_h_&#xa;')"/> 
            <xsl:value-of select="concat('#define ', $name, '_h_&#xa;')"/> 
            <xsl:text>#ifdef __cplusplus &#xa;extern "C" { &#xa;#endif&#xa;</xsl:text>
            <!-- For each interface, search for "ownedAttributes", which define constants, etc -->
            <xsl:apply-templates select="ownedAttribute" mode="Interface"/>
            <!-- For each interface, search for "ownedOperation", which define functions -->
            <xsl:apply-templates select="ownedOperation" mode="Interface"/>
            
            <xsl:text>#ifdef __cplusplus &#xa;}&#xa;#endif&#xa;</xsl:text>
            
            <xsl:text>#endif&#xa;</xsl:text>
        </xsl:result-document> 
    </xsl:template>
    
    <!-- ownedAttributes define constants etc -->
    <xsl:template match="ownedAttribute" mode="Interface">
        <xsl:variable name="type_id" select="type/@xmi:idref"/>
        <xsl:variable name="name" select="@name"/>

        <!-- Search for enumerations -->
        <xsl:apply-templates select="//packagedElement[@xmi:type='uml:Enumeration' and @xmi:id=$type_id]" mode="Enumerate">
            <xsl:with-param name="enum_var_name" select="@name"/>
        </xsl:apply-templates>
    </xsl:template>
    
    <!-- Found a defined type, so assume it is an enumeration and enumerate it -->
    <xsl:template match="packagedElement" mode="Enumerate">
        <xsl:param name="enum_var_name"/>
    
        <xsl:value-of select="concat('//Found enumeration: ', @name, '&#xa;')"/> 
        <xsl:value-of select="'typedef enum {&#xa;'"/> 
        <!-- Now process each "ownedLiteral" to extract the enumeration name and maybe it's value, if defined -->
        <xsl:apply-templates select="ownedLiteral" mode="Enumerate">
            <xsl:with-param name="enum_name" select="@name"/>
        </xsl:apply-templates>
        
        <xsl:value-of select="concat('} ', @name, ';&#xa;')"/> 
        
        <!-- Now declare variable of type, if defined -->
        <xsl:if test="string-length($enum_var_name) &gt; 0">
            <xsl:value-of select="concat(@name, ' ', $enum_var_name, ';&#xa;')"/> 
        </xsl:if>
        <xsl:text>&#xa;</xsl:text>
    
    </xsl:template>

    <!-- Found an enumeration, so expand its values -->
    <xsl:template match="ownedLiteral" mode="Enumerate">
        <xsl:param name="enum_name"/>
        
        <xsl:value-of select="concat('    ', $enum_name, '_', @name)"/>
        <xsl:variable name="attrib_id" select="@xmi:id"/>
        <xsl:variable name="enum_val" select="//attribute[@xmi:idref=$attrib_id]/initial/@body"/>
        <xsl:if test="not(empty($enum_val))">
            <xsl:value-of select="concat(' = ', $enum_val)"/>
        </xsl:if>
        <xsl:value-of select="',&#xa;'"/>
        
    </xsl:template>
    
    <!-- ownedOperations define functions -->
    <xsl:template match="ownedOperation" mode="Interface">
        <xsl:variable name="name" select="@name"/>
        <xsl:variable name="return_type" select="ownedParameter[@direction='return']/@type"/>

        <!-- Search for enumerations -->
        <!-- Disabled until further notice. Enumerations should be defined via an Interface attribute, i.e. Interface must define an attribute as well as an operation....
        <xsl:variable name="type_id" select="ownedParameter/@type"/>
        <xsl:apply-templates select="//packagedElement[@xmi:type='uml:Enumeration' and @xmi:id=$type_id]" mode="Enumerate"/>
        -->

            <!-- Write out operation declaration, starting with return type -->
            <xsl:choose>
                <!-- If starts with EAC_, strip the leading EAC_ -->
                <xsl:when test="starts-with($return_type, 'EAC')">
                    <xsl:value-of select="concat( substring($return_type, 5), ' ', @name, '(')"/>
                </xsl:when>
                <xsl:when test="starts-with($return_type, 'EAJava')">
                    <xsl:value-of select="concat( substring($return_type, 8), ' ', @name, '(')"/>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="concat($return_type, ' ', @name, '(')"/>
                </xsl:otherwise>
            </xsl:choose>
        
        <!-- Search for other parameters -->
        <xsl:variable name="parameters" select="ownedParameter[@direction='in']" />
        <xsl:variable name="count" select="count($parameters)" />
        <xsl:for-each select="$parameters">
            <xsl:variable name="param_name" select="@name" />
            <!-- Get the parameter type i -->
            <xsl:variable name="param_type_id" select="@type" />
            <!-- Get the name of the parameter type -->
            <xsl:variable name="param_type_name" select="//packagedElement[@xmi:id=$param_type_id]/@name" />
            <!-- Get the actual type of the parameter-->
            <xsl:variable name="param_type_type" select="//packagedElement[@xmi:id=$param_type_id]/@xmi:type" />

            <!-- Now declare variable of type, if defined -->
            <!--
            <xsl:if test="$param_type_type = 'uml:Enumeration'">
                <xsl:text>enum </xsl:text>
            </xsl:if>
            -->
            
            <xsl:value-of select="concat($param_type_name, ' ', $param_name)" />
            <!-- Print a comma if there are more parameters to come -->
            <xsl:if test="position() != $count">
                <xsl:value-of select="', '" />
            </xsl:if>
        </xsl:for-each>
        
            <xsl:value-of select="');&#xa;'"/>
        
    </xsl:template>
</xsl:stylesheet>
