<?xml version="1.0" encoding="UTF-8"?>
<!--
########################################################################
# $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-adapters/tags/CODAC-CORE-6.2B2/src/main/resources/xmi2snl_adapter001.xsl $
# $Id: xmi2snl_adapter001.xsl 96477 2019-01-09 14:53:55Z sivecm $
#
# Project       : <Project name>
#
# Description   : A script that converts Enterprise Architect model (in XML format) to the SNL state machines.
#
# Author(s)     : Benjamin Ocepek
#
# Copyright (c) : 2010-2019 ITER Organization,
#                 CS 90 046
#                 13067 St. Paul-lez-Durance Cedex
#                 France
#
# This file is part of the ITER CODAC software.
# For the terms and conditions of redistribution or use of this software
# refer to the file ITER-LICENSE.TXT located in the top level directory
# of the distribution package.
########################################################################
This script has an expected model of the Enterprise Architect data, described in the Technical Report:
https://svnpub.iter.org/codac/iter/codac/contracts/2015/IO_CT_6000000132_TO3/ST5_PS_Simulators/DNFM-Simulator/doc/Sim4_TechnicalReport.docx

Script can be run as (example): java -jar /opt/codac/lib/saxon-9.1.0.8.jar -xsl:xmi2snl_adaptor001.xsl -s:auto1.xml top_state=StateMachine
-->
<xsl:stylesheet version="2.0" xmlns="http://www.w3.org/2005/07/scxml" 
                                xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
                                xmlns:xmi="http://schema.omg.org/spec/XMI/2.1" 
                                xmlns:uml="http://www.eclipse.org/uml2/3.0.0/UML" 
                                xmlns:scxml="http://www.w3.org/2005/07/scxml">
    <!-- OUTPUT Control -->
    <xsl:output method="text" indent="yes" encoding="UTF-8"/>
    <xsl:param name="top_state"/>

    <!-- ###################### DEFINE PART ###################### -->

    <xsl:template match="/">
        <xsl:variable name="programName" select="translate(replace($top_state, '&amp;', 'And'), ':- ', '__')"/>
        <xsl:variable name="filename" select="concat($programName,'.st')"/>

        <!-- OUTPUT Control - Create a text file -->
        <xsl:result-document href="{$filename}" method="text">
            <!-- The name of the program is the name of the top state machine -->
            <xsl:value-of select="concat('program ', $programName, '&#xa;&#xa;')"/>
            <!-- Look for the header files -->
            <xsl:apply-templates mode="interface" select="//packagedElement[@xmi:type='uml:Interface']"/>
            <xsl:apply-templates mode="interface" select="//packagedElement/required"/>
            <xsl:apply-templates mode="interface" select="//packagedElement/provided"/>

            <!-- Look for the COS OPREQ PV which is used for the IsCOS() function -->
            <xsl:apply-templates mode="define" select="//nestedClassifier[@xmi:type='uml:Class' and @name='Adapter_Interface']/ownedAttribute[@xmi:type='uml:Property' and @name='COS_OPREQ'] |
                //packagedElement[@xmi:type='uml:Class' and @name='Adapter_Interface']/ownedAttribute[@xmi:type='uml:Property' and @name='COS_OPREQ']">
                <xsl:with-param name="varname" select="'COS_OPREQ'"/>
                <xsl:with-param name="monitor" select="true()"/>
            </xsl:apply-templates>
            <xsl:text>#define IsCOS(state) COS_OPREQ == COS_Request_ ## state&#xa;&#xa;</xsl:text> <!-- Expands the IsCOS function -->
            
            <!-- Looks for the COS OPSTATE PV -->
            <xsl:apply-templates mode="define" select="//nestedClassifier[@xmi:type='uml:Class' and @name='Adapter_Interface']/ownedAttribute[@xmi:type='uml:Property' and @name='COS_OPSTATE'] |
                //packagedElement[@xmi:type='uml:Class' and @name='Adapter_Interface']/ownedAttribute[@xmi:type='uml:Property' and @name='COS_OPSTATE']">
                <xsl:with-param name="varname" select="'COS_OPSTATE'"/>
                <xsl:with-param name="monitor" select="false()"/>
            </xsl:apply-templates>
            
            <!-- Looks for the PSOS states -->
            <xsl:apply-templates mode="define" select="//nestedClassifier[@xmi:type='uml:Class' and @name='PSOS_States'] | //packagedElement[@xmi:type='uml:Class' and @name='PSOS_States']">
                <xsl:with-param name="prefix" select="'PSOS_State_'"/>
            </xsl:apply-templates>
            <!-- Look for the PSOS OP STATE PV whis is used for IsPSOS() function -->
            <xsl:apply-templates mode="define" select="//nestedClassifier[@xmi:type='uml:Class' and @name='Adapter_Interface']/ownedAttribute[@xmi:type='uml:Property' and @name='PSOS_OPSTATE']  |
                //packagedElement[@xmi:type='uml:Class' and @name='Adapter_Interface']/ownedAttribute[@xmi:type='uml:Property' and @name='PSOS_OPSTATE']">
                <xsl:with-param name="varname" select="'PSOS_OPSTATE'"/>
                <xsl:with-param name="monitor" select="true()"/>
            </xsl:apply-templates>
            <xsl:text>#define IsPSOS(state) PSOS_OPSTATE == PSOS_State_ ## state&#xa;&#xa;</xsl:text> <!-- Expands he isPSOS function -->


            <!-- Looks for the PSOS Events: Generates list of #define entries -->
            <xsl:apply-templates mode="define" select="//nestedClassifier[@xmi:type='uml:Class' and @name='PSOS_Events'] | //packagedElement[@xmi:type='uml:Class' and @name='PSOS_Events']">
                <xsl:with-param name="prefix" select="'PSOS_Request_'"/>
            </xsl:apply-templates>
            <!-- Look for the PSOS OP EVENT PV whis is used for SetPSOS() function -->
            <xsl:apply-templates mode="define" select="//nestedClassifier[@xmi:type='uml:Class' and @name='Adapter_Interface']/ownedAttribute[@xmi:type='uml:Property' and @name='PSOS_OPEVENT']  |
                //packagedElement[@xmi:type='uml:Class' and @name='Adapter_Interface']/ownedAttribute[@xmi:type='uml:Property' and @name='PSOS_OPEVENT']">
                <xsl:with-param name="varname" select="'PSOS_OPEVENT'"/>
                <xsl:with-param name="monitor" select="false()"/>
            </xsl:apply-templates>


            <!-- Looks for the Control Event PV and defines events (used to load and unload child state machine) -->
            <xsl:apply-templates mode="define" select="//nestedClassifier[@xmi:type='uml:Class' and @name='Control_Event'] | //packagedElement[@xmi:type='uml:Class' and @name='Control_Event']">
                <xsl:with-param name="monitor" select="true()"/>
            </xsl:apply-templates>
            <!-- Looks for the top state machine OP STATE PV and defines its states -->
            <xsl:apply-templates mode="define" select="//nestedClassifier[@xmi:type='uml:Class' and @name='Control_State'] | //packagedElement[@xmi:type='uml:Class' and @name='Control_State']">
                <xsl:with-param name="prefix" select="'CO_State_'"/>
                <xsl:with-param name="monitor" select="false()"/>
            </xsl:apply-templates>
            <!-- Looks for the top state machine and starts creating it and it's children. -->
            <xsl:apply-templates mode="topstate" select="//nestedClassifier[@xmi:type='uml:StateMachine' and @name=$top_state] | //packagedElement[@xmi:type='uml:StateMachine' and @name=$top_state]"/>
        </xsl:result-document>
    </xsl:template>

    <!-- Include header file -->
    <xsl:template mode="interface" match="packagedElement">
        <xsl:variable name="this_name" select="@name"/>
        <xsl:text>// Global "interface" reference&#xa;</xsl:text>
        <xsl:if test="//packagedElement/provided[@name=$this_name]">
            <xsl:text>// But it also matches 'provided' interface declaration. Commenting out.&#xa;//</xsl:text>
        </xsl:if>        
        <xsl:value-of select="concat('%%#include &quot;', @name, '.h&quot;&#xa;')"/>
    </xsl:template>

    <!-- Include header file -->
    <xsl:template mode="interface" match="required">
        <xsl:text>// "Required" interface&#xa;</xsl:text>
        <xsl:value-of select="concat('%%#include &lt;', @name, '.h&gt;&#xa;&#xa;')"/>
    </xsl:template>
    <xsl:template mode="interface" match="provided">
        <xsl:text>// "Provided" interface&#xa;</xsl:text>
        <xsl:value-of select="concat('#include &quot;', @name, '.h&quot;&#xa;&#xa;')"/>
    </xsl:template>

    <!-- Creates a SNL variable, assign it to PV and optionally puts a monitor on it. -->
    <xsl:template mode="define" match="ownedAttribute">
        <xsl:param name="varname"/>

        <!-- Get the return type from the model. If starts with EA*_, strip the leading EA*_ -->
        <!-- <xsl:value-of select="concat('&#xa;int ', $varname, ';&#xa;', 'assign ', $varname, ' to &quot;', defaultValue/@value, '&quot;; ')"/>  -->
        <xsl:variable name="return_type" select="type/@xmi:idref"/>
        <xsl:choose>
            <xsl:when test="starts-with($return_type, 'EAC')">
                <xsl:value-of select="concat('&#xa;', substring($return_type, 5), ' ', $varname, ';&#xa;')"/>
            </xsl:when>
            <xsl:when test="starts-with($return_type, 'EAJava')">
                <xsl:value-of select="concat('&#xa;', substring($return_type, 8), ' ', $varname, ';&#xa;')"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="concat('&#xa;', $return_type, ' ', $varname, '; ')"/>
            </xsl:otherwise>
        </xsl:choose>
        <xsl:value-of select="concat('assign ', $varname, ' to &quot;', defaultValue/@value, '&quot;; ')"/>
        
        <!-- If the PV is marked as "ReadOnly", it means that the SNL should put a monitor on it -->
        <xsl:if test="@isReadOnly = 'true'">
            <xsl:value-of select="concat('monitor ', $varname, ';')"/>
        </xsl:if>
        <xsl:text>&#xa;&#xa;</xsl:text>
    </xsl:template>

    <!-- If the attribute of the entry is PV_Name then it creates a SNL variable, assign it to PV and optionally puts a monitor on it.
         Otherwise, it defines the new entry with default value (for enumerated states) -->
    <xsl:template mode="define" match="nestedClassifier|packagedElement">
        <xsl:param name="prefix"/>
        <xsl:param name="monitor"/>
        <xsl:variable name="varname" select="@name"/>
        <xsl:for-each select="ownedAttribute">
            <xsl:if test="@name != 'PV_Name'">
                <xsl:value-of select="concat('#define ', $prefix, @name, ' ', defaultValue/@value, '&#xa;')"/>
            </xsl:if>
            <xsl:if test="@name = 'PV_Name'">
                <xsl:value-of select="concat('&#xa;int ', $varname, ';&#xa;assign ', $varname, ' to &quot;', defaultValue/@value, '&quot;; ')"/>
                <xsl:if test="$monitor=true()">
                    <xsl:value-of select="concat('monitor ', $varname, ';')"/>
                </xsl:if>
                <xsl:text>&#xa;&#xa;</xsl:text>
            </xsl:if>
        </xsl:for-each>
    </xsl:template>

<!-- ################### END OF DEFINE PART #################  -->

    <!-- Creates a parent state machine -->
    <xsl:template mode="topstate" match="nestedClassifier|packagedElement">
        <xsl:variable name="name" select="@name"/>
        <xsl:variable name="id" select="@xmi:id"/>
        <xsl:variable name="programName" select="translate(replace($name, '&amp;', 'And'), ':- ', '__')"/>

        <xsl:value-of select="concat('&#xa;ss ', $programName, ' {&#xa;')"/>
        <!-- Search for default state -->
        <xsl:apply-templates mode="initial" select="region/subvertex[@xmi:type='uml:Pseudostate' and @kind='initial']"/>
        <!-- Search for states -->
        <xsl:apply-templates select="region/subvertex[@xmi:type='uml:State']">
            <xsl:with-param name="parent_statemachine" select="$id"/>
        </xsl:apply-templates>
        <!-- search for transitions to child state-machine: i.e. the target must be a child state-machine. And create a state in parent state machine for each child state machine -->
        <xsl:apply-templates mode="tochild" select="region/transition">
            <xsl:with-param name="parent_statemachine" select="$id"/>
        </xsl:apply-templates>
        <xsl:text>&#xa;}&#xa;</xsl:text>
        <!-- Create child state machines -->
        <xsl:apply-templates mode="substate" select="nestedClassifier[@xmi:type='uml:StateMachine']">
            <xsl:with-param name="parent_statemachine" select="$id"/>
        </xsl:apply-templates>
    </xsl:template>

    <!-- Search for default state and create temporal initial state -->
    <xsl:template mode="initial" match="region/subvertex">
        <xsl:variable name="id" select="@xmi:id"/>
        <xsl:variable name="name" select="translate(replace(@name, '&amp;', 'And'), ':- ', '__')"/>
        <xsl:text>&#xa;    // Initial state&#xa;</xsl:text>
        <xsl:text>    state </xsl:text>
        <xsl:value-of select="$name"/>
        <xsl:text> {</xsl:text>
        <!-- search for outgoing transition which should point to the default state -->
        <xsl:apply-templates select="outgoing"/>
        <xsl:text>&#xa;    }&#xa;</xsl:text>
    </xsl:template>

    <!-- Create child state machines -->
    <xsl:template mode="substate" match="nestedClassifier">
        <xsl:param name="parent_statemachine"/>
        <xsl:variable name="name" select="@name"/>
        <xsl:variable name="id" select="@xmi:id"/>
        <xsl:variable name="programName" select="translate(replace($name, '&amp;', 'And'), ':- ', '__')"/>

        <xsl:text>&#xa;ss </xsl:text>
        <xsl:value-of select="$programName"/>
        <xsl:text> {&#xa;</xsl:text>
        <!-- search for transitions back to parent state-machine: i.e. the target must be a subvertex of the parent.
             Create a state in child state machine that corresponds to the state in parent that the transition points to. -->
        <xsl:apply-templates mode="toparent" select="//nestedClassifier[@xmi:type='uml:StateMachine' and @xmi:id=$parent_statemachine]/region/transition">
            <xsl:with-param name="parent_statemachine" select="$parent_statemachine"/>
            <xsl:with-param name="child_statemachine" select="$id"/>
        </xsl:apply-templates>
        <!-- Search for states -->
        <xsl:apply-templates select="region/subvertex[@xmi:type='uml:State']">
            <xsl:with-param name="parent_statemachine" select="$id"/>
        </xsl:apply-templates>
        <xsl:text>&#xa;}&#xa;</xsl:text>
    </xsl:template>

    <!-- For each subvertex found (It is a State) -->
    <xsl:template match="region/subvertex">
        <xsl:param name="parent_statemachine"/>
        <xsl:variable name="OPStateName" select="//*[@xmi:type='uml:StateMachine' and @xmi:id=$parent_statemachine]/@name"/>
        <xsl:variable name="name" select="translate(replace(@name, '&amp;', 'And'), ':- ', '__')"/>
        <xsl:variable name="id" select="@xmi:id"/>

        <xsl:text>&#xa;    // Regular transition within a single state-machine&#xa;</xsl:text>
        <xsl:text>    state </xsl:text>
        <xsl:value-of select="$name"/>
        <xsl:text> {</xsl:text>

        <!-- On entry update OP STATE PV related to this state machine -->
        <xsl:text>&#xa;        entry {&#xa;</xsl:text>
        <xsl:choose>
            <xsl:when test = "$OPStateName = $top_state">
                <xsl:text>            Control_State = CO_State_</xsl:text>
                <xsl:value-of select="$name"/>
                <xsl:text>;&#xa;            pvPut(Control_State);&#xa;</xsl:text>
            </xsl:when>
            <xsl:otherwise>
                <xsl:text>            COS_OPSTATE = COS_State_</xsl:text>
                <xsl:value-of select="$name"/>
                <xsl:text>;&#xa;            pvPut(COS_OPSTATE);&#xa;</xsl:text>
            </xsl:otherwise>
        </xsl:choose>
        <xsl:text>        }</xsl:text>
        
        
        <!-- search for global outgoing transitions -->
        <xsl:apply-templates select="//nestedClassifier[@xmi:id=$parent_statemachine]/outgoing | //packagedElement[@xmi:id=$parent_statemachine]/outgoing">
            <xsl:with-param name="source_id" select="$id"/>
        </xsl:apply-templates>

        <!-- search for regular outgoing transitions -->
        <xsl:apply-templates select="outgoing">
            <xsl:with-param name="source_id" select="$id"/>
        </xsl:apply-templates>
        
        <xsl:text>&#xa;    }&#xa;</xsl:text>
    </xsl:template>

    <!-- For each outgoing found (It is a Transition) -->
    <xsl:template match="outgoing">
        <xsl:param name="source_id"/>
        <xsl:variable name="transition_id" select="@xmi:idref"/>
        <xsl:variable name="source_name_state_vertex" select="//subvertex[@xmi:type='uml:State' and @xmi:id=$source_id]"/>
        <xsl:variable name="target_id" select="//transition[@xmi:id=$transition_id]/@target"/>
        <xsl:variable name="target_name_state_vertex" select="//subvertex[@xmi:type='uml:State' and @xmi:id=$target_id]"/>
        <xsl:variable name="target_name_state_raw" select="$target_name_state_vertex/@name"/>
        <xsl:variable name="target_name_state" select="translate(replace($target_name_state_raw, '&amp;', 'And'), ':- ', '__')"/>
        <xsl:variable name="target_name_statemachine_raw" select="//*[@xmi:id=$target_id]/@name"/>
        <xsl:variable name="target_name_statemachine" select="translate(replace($target_name_statemachine_raw, '&amp;', 'And'), ':- ', '__')"/>

        <!-- Do not create global transitions from src to src -->
        <xsl:if test="not($source_id = $target_id)">
            <xsl:text>&#xa;        when ( </xsl:text>
            <xsl:apply-templates mode="guard" select="//transition[@xmi:id=$transition_id]"/> <!-- Search for conditions -->
            <xsl:text> ) {&#xa;</xsl:text>
            <xsl:apply-templates select="//transition[@xmi:id=$transition_id]/effect"/> <!-- Effects for that transition -->
            <xsl:text>        } state </xsl:text>
            <!-- Next state is target state or a state named after targed child state machine. -->
            <xsl:choose>
                <xsl:when test="string-length($target_name_state) &gt; 0">
                    <xsl:value-of select="$target_name_state"/>
                </xsl:when>
                <xsl:when test="string-length($target_name_statemachine) &gt; 0">
                    <xsl:value-of select="$target_name_statemachine"/>
                </xsl:when>
            </xsl:choose>
        </xsl:if>
    </xsl:template>

    <!-- For each outgoing to parent transition that is found (It is a Transition) -->
    <xsl:template mode="toparent" match="outgoing">
        <xsl:param name="parent_statemachine"/>
        <xsl:variable name="transition_id" select="@xmi:idref"/>
        <xsl:variable name="target_id" select="//transition[@xmi:id=$transition_id]/@target"/>
        <xsl:variable name="target_name_state_raw" select="//subvertex[@xmi:type='uml:State' and @xmi:id=$target_id]/@name"/>
        <xsl:variable name="target_name_state" select="translate(replace($target_name_state_raw, '&amp;', 'And'), ':- ', '__')"/>

        <xsl:variable name="var_subvertex_name" select="//*[@xmi:type='uml:StateMachine' and @xmi:id=$parent_statemachine]/region/subvertex[@xmi:type='uml:State' and @xmi:id=$target_id]/@name"/>

        <!-- <xsl:value-of select="concat('// 294 ', $target_id, ' ',$var_subvertex_name, '&#xa;')"/> -->

        <xsl:if test="string-length($var_subvertex_name) &gt; 0">
            <xsl:text>&#xa;        when ( </xsl:text>
            <xsl:apply-templates mode="guard" select="//transition[@xmi:id=$transition_id]"/>
            <xsl:text> ) {&#xa;</xsl:text>
            <xsl:apply-templates select="//transition[@xmi:id=$transition_id]/effect"/>
            <xsl:text>        } state </xsl:text>
            <xsl:value-of select="$target_name_state"/>
        </xsl:if>
    </xsl:template>

    <!-- For each outgoing to child found (It is a Transition). Target here is a child state machine.
         Look for the next hidden state which should include a pointer to the default state. -->
    <xsl:template mode="tochild" match="outgoing">
        <xsl:param name="child_statemachine"/>
        <xsl:variable name="transition_id" select="@xmi:idref"/>
        <xsl:variable name="target_id" select="//transition[@xmi:id=$transition_id]/@target"/>
        <xsl:variable name="hidden_state" select="//*[@xmi:type='uml:StateMachine' and @xmi:id=$child_statemachine]/region/subvertex[@xmi:type='uml:Pseudostate' and @kind='shallowHistory']/@name"/>

        <xsl:if test="$target_id = $child_statemachine">
            <xsl:text>&#xa;        when ( </xsl:text>
            <xsl:apply-templates mode="guard" select="//transition[@xmi:id=$transition_id]"/>
            <xsl:text> ) {&#xa;</xsl:text>
            <xsl:apply-templates select="//transition[@xmi:id=$transition_id]/effect"/>
            <xsl:text>        } state </xsl:text>
            <xsl:value-of select="$hidden_state"/>
        </xsl:if>
    </xsl:template>

    <!-- For each transition, process guard conditions and events. Currently only one should be used at the same time -->
    <xsl:template mode="guard" match="transition">
        <xsl:variable name="eventname" select="@name"/>
        <!-- find and insert guard conditions -->
        <xsl:variable name="guard_body" select="guard/specification/@body"/>
        <xsl:value-of select="$guard_body"/>
        <!-- if event is not empty, expand it -->
        <xsl:if test="$eventname != ''">
            <xsl:text>Control_Event == </xsl:text>
            <xsl:value-of select="$eventname"/>
        </xsl:if>
    </xsl:template>

    <!-- For each transition, process effects -->
    <xsl:template match="effect">
        <xsl:variable name="effect_body" select="@body"/>
        <xsl:text>            </xsl:text>
        <xsl:value-of select="$effect_body"/>
        <xsl:text>;&#xa;</xsl:text>
    </xsl:template>

    <!-- For each transition in the parent state machine that goes to the child state machine, create a local psuedo-state. Look for the state machine alias to properly update OPSTATE PV -->
    <xsl:template mode="tochild" match="transition">
        <xsl:param name="parent_statemachine"/>
        <xsl:variable name="target_id" select="@target"/>
        <xsl:variable name="target_name_statemachine" select="//nestedClassifier[@xmi:id=$target_id]/@name"/> <!-- Must be nested classifier -->
        <xsl:variable name="statemachine_alias" select="//element[@xmi:idref=$target_id and @xmi:type='uml:StateMachine']/properties/@alias"/>
        <xsl:variable name="OPStateName" select="//*[@xmi:type='uml:StateMachine' and @xmi:id=$parent_statemachine]/@name"/>

        <!-- Only do this if the target is a child state machine -->
        <xsl:if test="not(empty($target_name_statemachine))">
            <!-- Create a state with the name of child state machine -->
            <xsl:text>&#xa;    state </xsl:text>
            <xsl:value-of select="$target_name_statemachine"/>
            <xsl:text> {</xsl:text>
            <!-- Update OPSTATE PV with proper state name - from alias -->
            <xsl:text>&#xa;        entry {&#xa;</xsl:text>
            <xsl:text>            </xsl:text>
            <xsl:if test = "$OPStateName = $top_state">
                <xsl:text>Control_State</xsl:text>
            </xsl:if>
            <xsl:if test = "$OPStateName != $top_state">
                <xsl:text>COS_OPSTATE</xsl:text>
            </xsl:if>
            <xsl:text> = CO_State_</xsl:text>
            <xsl:if test="not(empty($statemachine_alias))">
                <xsl:value-of select="$statemachine_alias"/>
            </xsl:if>
            <xsl:if test="empty($statemachine_alias)">
                <xsl:value-of select="$target_name_statemachine"/>
            </xsl:if>
            <xsl:text>;&#xa;</xsl:text>
            <xsl:text>            </xsl:text>
            <xsl:if test = "$OPStateName = $top_state">
                <xsl:text>pvPut(Control_State);&#xa;</xsl:text>
            </xsl:if>
            <xsl:if test = "$OPStateName != $top_state">
                <xsl:text>pvPut(COS_OPSTATE);&#xa;</xsl:text>
            </xsl:if>
            <xsl:text>        }</xsl:text>
            <!-- Find transitions to the parent state machine for when() statements -->
            <xsl:apply-templates mode="toparent" select="//nestedClassifier[@xmi:type='uml:StateMachine' and @xmi:id=$target_id]/outgoing">
                <xsl:with-param name="parent_statemachine" select="$parent_statemachine"/>
            </xsl:apply-templates>
            <xsl:text>&#xa;    }</xsl:text>
        </xsl:if>
    </xsl:template>

    <!-- For each transition in the sub-state that goes to the parent state machine, create a local psuedo-state in child state machine -->
    <xsl:template mode="toparent" match="transition">
        <xsl:param name="parent_statemachine"/>
        <xsl:param name="child_statemachine"/>
        <xsl:variable name="target_id" select="@target"/>

        <!-- Only do this if the target is a child vertex of the parent state machine -->
        <xsl:variable name="target_name_state_raw" select="//*[@xmi:type='uml:StateMachine' and @xmi:id=$parent_statemachine]/region/subvertex[@xmi:type='uml:State' and @xmi:id=$target_id]/@name"/>
        <xsl:variable name="target_name_state" select="translate(replace($target_name_state_raw, '&amp;', 'And'), ':- ', '__')"/>
        <xsl:if test="$target_name_state != ''">
            <!-- Here is where I want to install "Parent_" to indicate that the state is derived from the parent -->
            <!-- But I can not find out to ID the inverse direction (+- line 281) to insert "Parent_" into the target state name. -->
            <xsl:text>&#xa;    //  Psuedo-state that matches the parent state-machine state&#xa;</xsl:text>
            <xsl:text>    state </xsl:text>
            <xsl:value-of select="$target_name_state"/>
            <xsl:text> { </xsl:text>
            <!-- Find transitions back to the child state machine and proceed to the hidden state -->
            <xsl:apply-templates mode="tochild" select="//nestedClassifier[@xmi:type='uml:StateMachine' and @xmi:id=$parent_statemachine]/region/subvertex[@xmi:type='uml:State' and @xmi:id=$target_id]/outgoing">
                <xsl:with-param name="child_statemachine" select="$child_statemachine"/>
            </xsl:apply-templates>
            <xsl:text>&#xa;    }&#xa;</xsl:text>
            
            
            <xsl:variable name="history_state" select="//*[@xmi:type='uml:StateMachine' and @xmi:id=$child_statemachine]/region/subvertex[@xmi:type='uml:Pseudostate' and @kind='shallowHistory']/@name"/>
            <xsl:if test="$history_state != ''">
                <!-- A hidden state that can restore the last state of the state machine or proceed to the default state -->
                <xsl:text>&#xa;    // Special History state to recover the state-machine to last know state&#xa;</xsl:text>
                <xsl:text>    state </xsl:text>
                <xsl:value-of select="$history_state"/>
                <xsl:text> { </xsl:text>
                <!-- Find transitions to all states of the child state machine -->
                <xsl:apply-templates mode="toall" select="//nestedClassifier[@xmi:type='uml:StateMachine' and @xmi:id=$child_statemachine]/region/subvertex[@xmi:type='uml:State']">
                    <xsl:with-param name="child_statemachine" select="$child_statemachine"/>
                </xsl:apply-templates>
                
                <!-- Find default state: The final state has auto-selected. If no history transition is satisfied, the default state is selected -->
                <xsl:variable name="default_transition_id" select="//*[@xmi:type='uml:StateMachine' and @xmi:id=$child_statemachine]/region/subvertex[@xmi:type='uml:Pseudostate' and @kind='shallowHistory']/outgoing/@xmi:idref"/>
                <xsl:variable name="default_target_id" select="//transition[@xmi:id=$default_transition_id]/@target"/>
                <xsl:text>&#xa;        when ( ) {&#xa;        } state </xsl:text>
                <xsl:value-of select="//subvertex[@xmi:id=$default_target_id]/@name"/>
                <xsl:text>&#xa;    }&#xa;</xsl:text>
            </xsl:if>
            
        </xsl:if>

    </xsl:template>

    <!-- Find transitions to all states in child state machine -->
    <xsl:template mode="toall" match="subvertex">
        <xsl:param name="child_statemachine"/>

        <xsl:text>&#xa;        when ( COS_OPSTATE == COS_State_</xsl:text>
        <xsl:value-of select="@name"/>
        <xsl:text> ) {&#xa;        } state </xsl:text>
        <xsl:value-of select="@name"/>
    </xsl:template>

</xsl:stylesheet>
