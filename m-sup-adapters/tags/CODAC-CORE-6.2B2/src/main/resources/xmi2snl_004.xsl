<?xml version="1.0" encoding="UTF-8"?>
<!--
########################################################################
# $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-adapters/tags/CODAC-CORE-6.2B2/src/main/resources/xmi2snl_004.xsl $
# $Id: xmi2snl_004.xsl 96477 2019-01-09 14:53:55Z sivecm $
#
# Project       : <Project name>
#
# Description   : Enterprise Architect model (XMI 2.1 format) to SNL state machines converter.
#
# Author(s)     : Kevin Meyer
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

Script can be run as (example): java -jar /opt/codac/lib/saxon-9.1.0.8.jar -xsl:xmi2snl_004.xsl -s:POS_model.xml top=POS
-->
<xsl:stylesheet version="2.0" xmlns="http://www.w3.org/2005/07/scxml" 
                                xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
                                xmlns:xmi="http://schema.omg.org/spec/XMI/2.1" 
                                xmlns:uml="http://www.eclipse.org/uml2/3.0.0/UML" 
                                xmlns:scxml="http://www.w3.org/2005/07/scxml">
    <!-- OUTPUT Control -->
    <xsl:output method="text" indent="yes" encoding="UTF-8"/>
    <xsl:param name="top"/>
    <xsl:variable name="adapter_element" select='//packagedElement[@xmi:type="uml:Component" and @name=$top]'/>
    <xsl:variable name="adapter_package" select='$adapter_element/parent::node()'/>
    <xsl:variable name="top_state_var" select='$adapter_element/ownedAttribute[@xmi:type="uml:Property" and @name="StateMachine"]'/>
    <xsl:variable name="top_state" select='$top_state_var/defaultValue/@value'/>    
    <!-- Note:
        Once the adapter_element or adapter_package have been selected, they can be used to discriminate against other content in the model.
        For example:
           Matches only packageElements that are in the adapter_package.
           <xsl:apply-templates mode="interface" select="$adapter_package/packagedElement/required"/>
        Design choice:
            Should Adapter-specific elements be: 
                "nestedClassifiers" in the adapter element
                    or
                "packagedElement" in the adapter package
    -->
    <!-- ###################### DEFINE PART ###################### -->
    <xsl:template match='/'>
        <xsl:value-of select='concat("Adapter: ", $top, " ")'/>
        <!-- <xsl:value-of select='concat("In package: ", $adapter_package/@xmi:id, " ")'/> -->
        <xsl:value-of select='concat("TopState:", $top_state, " ")'/>
        <xsl:if test="$top_state != ''">
            <xsl:variable name="programName" select="translate(replace($top_state, '&amp;', 'And'), ':- ', '__')"/>
            <xsl:variable name="filename" select="concat($programName,'.st')"/>

            <!-- OUTPUT Control - Create a text file -->
            <xsl:result-document href="{$filename}" method="text">
                <!-- The name of the program is the name of the top state machine -->
                <xsl:value-of select="concat('program ', $programName, '&#xa;&#xa;')"/>
                <!-- Loggging -->
                <xsl:text>// Optional logging for debugging purposes&#xa;%%#include &lt;common/log-wrap.h&gt;&#xa;&#xa;</xsl:text>
                
                <!-- Look for the header files -->
                <xsl:apply-templates mode="interface" select="$adapter_package/packagedElement[@xmi:type='uml:Interface']"/>
                <xsl:apply-templates mode="interface" select="$adapter_package/packagedElement/required"/>
                <xsl:apply-templates mode="interface" select="$adapter_package/packagedElement/provided"/>
                
                <xsl:text>&#xa;// State Machine Interfaces</xsl:text>
                <!-- Looks for the Control Event PV and defines events (used to load and unload child state machine) -->
                <xsl:apply-templates mode="define" select="$adapter_element/nestedClassifier[@xmi:type='uml:Class' and @name='SM_Interface']/ownedAttribute"/>
                
                <!-- Looks for the top state machine OP STATE PV and defines its states -->
                <xsl:apply-templates mode="define" select="$adapter_element/nestedClassifier[@xmi:type='uml:Class' and @name='EPICS_Interface']/ownedAttribute"/>
                
                <!-- Looks operations/functions defined in the model -->
                <xsl:text>&#xa;&#xa;// Model-defined functions...&#xa;</xsl:text>
                <xsl:apply-templates mode="define" select="$adapter_element/nestedClassifier[@xmi:type='uml:Class' and @name='EPICS_Interface']/ownedOperation"/>
                
                <!-- Look for top "entry" operation -->
                <xsl:apply-templates mode="entry_exit" select="$adapter_element/nestedClassifier[@xmi:type='uml:Class' and @name='SM_Interface']/ownedOperation[@name='entry']"/>
                
                <!-- Looks for the top state machine and starts creating it. Note: It must be a child of the top level component -->
                <xsl:text>&#xa;// Model-defined Main State-machines&#xa;</xsl:text>
                <xsl:apply-templates mode="topstate" select="$adapter_element/nestedClassifier[@xmi:type='uml:StateMachine' and @name=$top_state]"/>

                <xsl:text>&#xa;// Model-defined Other State-machines&#xa;</xsl:text>
                <xsl:apply-templates mode="topstate" select="$adapter_element/nestedClassifier[@xmi:type='uml:StateMachine' and @name!=$top_state]"/>
                
                <!-- Look for top "exit" operation -->
                <xsl:apply-templates mode="entry_exit" select="$adapter_element/nestedClassifier[@xmi:type='uml:Class' and @name='SM_Interface']/ownedOperation[@name='exit']"/>
                
            </xsl:result-document>
        </xsl:if>
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
        <xsl:value-of select="concat('%%#include &lt;', @name, '.h&gt;&#xa;')"/>
    </xsl:template>
    <xsl:template mode="interface" match="provided">
        <xsl:text>// "Provided" interface&#xa;</xsl:text>
        <xsl:value-of select="concat('%%#include &quot;', @name, '.h&quot;&#xa;')"/>
    </xsl:template>

    <!-- Creates a SNL variable, assign it to PV and optionally puts a monitor on it. -->
    <xsl:template mode="define" match="ownedAttribute">
        <xsl:if test="not (ends-with(@name, '_Prefix'))">

            <xsl:variable name="sm_name" select="$adapter_element/@name"/>
            <xsl:variable name="varname" select="concat($sm_name,'_', @name)"/>

            <!-- Get the return type from the model. If starts with EA*_, strip the leading EA*_ -->
            <xsl:variable name="return_type" select="type/@xmi:idref"/>
            <xsl:choose>
                <xsl:when test="starts-with($return_type, 'EAC')">
                    <xsl:value-of select="concat('&#xa;', substring($return_type, 5), ' ', $varname, '; ')"/>
                </xsl:when>
                <xsl:when test="starts-with($return_type, 'EAJava')">
                    <xsl:value-of select="concat('&#xa;', substring($return_type, 8), ' ', $varname, '; ')"/>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="concat('&#xa;', $return_type, ' ', $varname, '; ')"/>
                </xsl:otherwise>
            </xsl:choose>
            
            <!-- Only assign to PVs if the variable is public -->
            <xsl:if test="@visibility = 'public'">
            <xsl:value-of select="concat('assign ', $varname, ' to &quot;', defaultValue/@value, '&quot;; ')"/>
            </xsl:if>
            
            <!-- If the PV is marked as "ReadOnly", it means that the SNL should put a monitor on it -->
            <xsl:if test="@isReadOnly = 'true'">
                <xsl:value-of select="concat('monitor ', $varname, ';')"/>
            </xsl:if>
        </xsl:if>
    </xsl:template>
    
    <!-- Define a function / #define from the model -->
    <!--
        <ownedOperation xmi:id="EAID_2AF86708_A4CD_4fb9_A94B_DC41469069AA" name="IsCOS" visibility="public" concurrency="sequential">
            <ownedParameter xmi:id="EAID_AB835EAF_FAB0_4f52_9549_03E7626DCC0B" name="desiredState" direction="in" isStream="false" isException="false" isOrdered="false" isUnique="true" type="EAJava_int"/>
            <ownedParameter xmi:id="EAID_RT000000_A4CD_4fb9_A94B_DC41469069AA" name="return" direction="return" type="EAJava_int"/>
            <bodyCondition xmi:type="uml:Constraint" xmi:id="EAID_CO000000_A4CD_4fb9_A94B_DC41469069AA">
                <specification xmi:type="uml:OpaqueExpression" xmi:id="EAID_OE000000_A4CD_4fb9_A94B_DC41469069AA" body="(PV_COS_State == (COS_State_ ## desiredState))"/>
            </bodyCondition>
        </ownedOperation>    
        #define IsCOS(desiredState) (COSSTATE == (desiredState ## _STATE_VAL))        
  -->
    <xsl:template mode='define' match='ownedOperation'>
        <xsl:variable name='operation_name' select='@name'/>
        <xsl:variable name='operation_parameter' select='ownedParameter[@direction="in"]/@name'/>
        <xsl:variable name='operation_body' select='bodyCondition/specification/@body'/>
        <xsl:value-of select='concat("#define ", $operation_name, "(", $operation_parameter, ") ", $operation_body,"&#xa;")'/>
    </xsl:template>

    <!-- Global Entry/Exit regions : SNL only supports one of each -->
    <xsl:template mode='entry_exit' match='ownedOperation'>
        <xsl:variable name='operation_name' select='@name'/>
        <xsl:variable name='operation_body' select='bodyCondition/specification/@body'/>
        <xsl:value-of select='concat($operation_name," {&#xa;   ", $operation_body,"&#xa;}")'/>
    </xsl:template>

<!-- ################### END OF DEFINE PART #################  -->

    <!-- Creates a parent state machine -->
    <xsl:template mode="topstate" match="nestedClassifier">
        <xsl:variable name="name" select="@name"/>
        <xsl:variable name="id" select="@xmi:id"/>
        <xsl:variable name="programName" select="translate(replace($name, '&amp;', 'And'), ':- ', '__')"/>        

        <!-- debugging: Only create these vars if top component attribute "StateMachine" ($top_state) matches $name -->
        <xsl:variable name="update_status" select='$top_state eq $name'/> <!-- true or false -->
        
        <xsl:variable name="sm_name" select="$adapter_element/@name"/>
        <xsl:variable name="sm_event" select="concat($sm_name, '_Event')"/> <!-- Hard-coded -->
        <!-- If sm_status is non-empty, the current state is written to the Status variable and the event variable is cleared -->
        <xsl:variable name="sm_status"> <!-- Hard-coded -->
            <xsl:if test='$update_status'>
                <xsl:value-of select="concat($sm_name, '_State')"/>
            </xsl:if>
        </xsl:variable>
        <xsl:variable name="sm_status_prefix" select='$adapter_element/nestedClassifier[@xmi:type="uml:Class" and @name="SM_Interface"]/ownedAttribute[@name="State_Prefix"]/defaultValue/@value'/>        
        <xsl:variable name="sm_event_prefix" select='$adapter_element/nestedClassifier[@xmi:type="uml:Class" and @name="SM_Interface"]/ownedAttribute[@name="Event_Prefix"]/defaultValue/@value'/>        
        
        
        <xsl:value-of select="concat('// Component name: ', $sm_name, ' &#xa;')"/>
        <xsl:value-of select="concat('ss ', $programName, ' {&#xa;')"/>
        <!-- Search for default state -->
        <xsl:apply-templates mode="initial" select="region/subvertex[@xmi:type='uml:Pseudostate' and @kind='initial']"/>
        <!-- Search for states -->
        <xsl:apply-templates select="region/subvertex[@xmi:type='uml:State']">
            <xsl:with-param name="parent_statemachine" select="$id"/>
            <xsl:with-param name="sm_event" select="$sm_event"/>
            <xsl:with-param name="sm_status" select="$sm_status"/>
            <xsl:with-param name="sm_status_prefix" select="$sm_status_prefix"/>
            <xsl:with-param name="sm_event_prefix" select="$sm_event_prefix"/>
        </xsl:apply-templates>
        <!-- DELETED: search for transitions to child state-machine: i.e. the target must be a child state-machine. And create a state in parent state machine for each child state machine -->
        <xsl:text>&#xa;}&#xa;</xsl:text>
        
        <!-- DELETED: Create child state machines -->
    </xsl:template>

    <!-- Search for default state and create temporal initial state -->
    <xsl:template mode="initial" match="region/subvertex">
        <xsl:variable name="id" select="@xmi:id"/>
        <xsl:variable name="name" select="translate(replace(@name, '&amp;', 'And'), ':- ', '__')"/>
        <xsl:text>&#xa;    // Initial state&#xa;</xsl:text>
        <xsl:text>    state </xsl:text>
        <xsl:value-of select="$name"/>
        <xsl:text> {</xsl:text>
        <!-- On entry: Log location -->
        <xsl:text>&#xa;        entry {&#xa;</xsl:text>  
            <xsl:value-of select='"            log_info(""Entering initialisation recovery state", $name, """);&#xa;"'/>
        <xsl:text>        }</xsl:text>
        
        <!-- search for outgoing transition which should point to the default state -->
        <xsl:apply-templates select="outgoing">
            <xsl:with-param name="source_id" select="$id"/>
        </xsl:apply-templates>
        <xsl:text>&#xa;    }&#xa;</xsl:text>
    </xsl:template>

    <!-- DELETED: Create child state machines -->


    <!-- For each subvertex found (It is a State) -->
    <xsl:template match="region/subvertex">
        <xsl:param name="parent_statemachine"/>
        <xsl:param name="sm_event"/>
        <xsl:param name="sm_status"/>
        <xsl:param name="sm_status_prefix"/>
        <xsl:param name="sm_event_prefix"/>
        
        <xsl:variable name="name" select="translate(replace(@name, '&amp;', 'And'), ':- ', '__')"/>
        <xsl:variable name="id" select="@xmi:id"/>

        <xsl:text>&#xa;    // Regular transition within a single state-machine&#xa;</xsl:text>
        <xsl:text>    state </xsl:text>
        <xsl:value-of select="$name"/>
        <xsl:text> {</xsl:text>

        <!-- On entry update OP STATE PV related to this state machine -->
        <xsl:text>&#xa;        entry {&#xa;</xsl:text>  
            <xsl:if test="$sm_status != ''">
                <xsl:value-of select='"           ", concat($sm_status, " = ", $sm_status_prefix, $name),"; pvPut(", $sm_status, ");&#xa;"'/>
                <xsl:value-of select='"           ", $sm_event, " = 0; pvPut(", $sm_event, ");&#xa;"'/>
            </xsl:if>
            <xsl:value-of select='"            log_info(""Entering state", $name, """);&#xa;"'/>
            
            <!-- Search for '<entry xmi:type="uml:Behavior" ...' behaviour -->
            <xsl:apply-templates mode='do_entry' select='entry[@xmi:type="uml:Behavior"]'/>
        <xsl:text>        }</xsl:text>
        
        <!-- search for regular outgoing transitions -->
        <xsl:apply-templates select="outgoing">
            <xsl:with-param name="source_id" select="$id"/>
            <xsl:with-param name="sm_event"  select="$sm_event"/>
            <xsl:with-param name="sm_event_prefix"  select="$sm_event_prefix"/>
        </xsl:apply-templates>

        <!-- Search for global outgoing transitions -->
        <xsl:apply-templates select='//nestedClassifier[@xmi:type="uml:StateMachine" and @xmi:id=$parent_statemachine]/outgoing'>
            <xsl:with-param name="source_id" select="$id"/>
            <xsl:with-param name="sm_event"  select="$sm_event"/>
            <xsl:with-param name="sm_event_prefix"  select="$sm_event_prefix"/>
        </xsl:apply-templates>
        
        <xsl:text>&#xa;    }&#xa;</xsl:text>
    </xsl:template>
    
    <!-- For each "entry" behaviour -->
    <xsl:template mode='do_entry' match='entry'>
            <xsl:text>            // 'entry' Behaviour&#xa;</xsl:text>
            <xsl:apply-templates mode='do_entry' select='ownedOperation'/>
    </xsl:template>
    <xsl:template mode='do_entry' match='ownedOperation'>
        <xsl:variable name='parameters' select='ownedParameter[@direction="in"]'/>
        <xsl:choose>
            <xsl:when test="count($parameters) &gt; 0">
                <xsl:value-of select='"           ", @name,"("'/>
                    <xsl:for-each select="$parameters">
                        <xsl:value-of select="defaultValue/@value" />
                        <!-- write out a comma if we are not at the end of the list -->
                        <xsl:if test="position() &lt; count($parameters)">, </xsl:if> 
                    </xsl:for-each>
                <xsl:value-of select='");&#xa;"' />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select='"           ", @name,";&#xa;"'/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <!-- For each outgoing found (It is a Transition) -->
    <xsl:template match="outgoing" mode='test'>
        <xsl:param name="source_id"/>
        <xsl:param name="sm_event"/>
        <xsl:param name="sm_event_prefix"/>

        XXX. <xsl:value-of select='@xmi:idref' />
        
    </xsl:template>
    

    <!-- For each outgoing found (It is a Transition) -->
    <xsl:template match="outgoing">
        <xsl:param name="source_id"/>
        <xsl:param name="sm_event"/>
        <xsl:param name="sm_event_prefix"/>
        
        <xsl:variable name="transition_id" select="@xmi:idref"/>
        <xsl:variable name="source_name_state_vertex" select="//subvertex[@xmi:type='uml:State' and @xmi:id=$source_id]"/>
        <xsl:variable name="transition" select="//transition[@xmi:id=$transition_id]"/>
        
        <xsl:variable name="transition_source_id" select="$transition/@source"/>
        <xsl:variable name="transition_target_id" select="$transition/@target"/>
        <xsl:variable name="target_name_state_vertex" select="//subvertex[@xmi:type='uml:State' and @xmi:id=$transition_target_id]"/>
        <xsl:variable name="target_name_state_raw" select="$target_name_state_vertex/@name"/>
        <xsl:variable name="target_name_state" select="translate(replace($target_name_state_raw, '&amp;', 'And'), ':- ', '__')"/>
        <xsl:variable name="target_name_statemachine_raw" select="//*[@xmi:id=$transition_target_id]/@name"/>
        <xsl:variable name="target_name_statemachine" select="translate(replace($target_name_statemachine_raw, '&amp;', 'And'), ':- ', '__')"/>
        
        <!-- <xsl:value-of select='concat("&#xa;&#xa;//transition_source_id:", $transition_source_id, ",  transition_target_id:", $transition_target_id)'/> -->
        
        <!-- Temporary: Do not create transition if it is to another state machine (until we decide how to handle this, generally) -->
        <xsl:if test="string-length($target_name_statemachine) &gt; 0">
            <!-- Do not create  transitions from src to src, unless it's in a single transition (i.e. a transition to self) -->
            <xsl:if test="($transition_source_id = $transition_target_id) or not($source_id = $transition_target_id)">
                <xsl:text>&#xa;        when ( </xsl:text>
                <xsl:apply-templates mode="guard" select="//transition[@xmi:id=$transition_id]"> <!-- Search for conditions -->
                    <xsl:with-param name="sm_event"  select="$sm_event"/>
                    <xsl:with-param name="sm_event_prefix"  select="$sm_event_prefix"/>
                </xsl:apply-templates>
                
                <xsl:text> ) {&#xa;</xsl:text>
                <xsl:apply-templates select="//transition[@xmi:id=$transition_id]/effect"/> <!-- Effects for that transition -->
                
                <xsl:value-of select='concat("            log_info(""Transition from ", $source_name_state_vertex/@name, " to ", $target_name_state, """);&#xa;")'/>
                
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
        </xsl:if>
    </xsl:template>


    <!-- For each transition, process guard conditions and events. Currently only one should be used at the same time -->
    <xsl:template mode="guard" match="transition">
        <xsl:param name="sm_event"/>
        <xsl:param name="sm_event_prefix"/>
        
        <xsl:variable name="eventname" select="@name"/>
        <!-- find and insert guard conditions -->
        <xsl:variable name="guard_body" select="guard/specification/@body"/>
        <xsl:value-of select="$guard_body"/>
        <!-- if event is not empty, expand it -->
        <xsl:if test="$eventname != ''">
            <xsl:if test="$guard_body != ''"> &amp;&amp; </xsl:if>
            <xsl:value-of select="concat('(', $sm_event ,' == ', $sm_event_prefix, $eventname, ')')"/>
        </xsl:if>
    </xsl:template>

    <!-- For each transition, process effects -->
    <xsl:template match="effect">
        <xsl:variable name="effect_body" select="@body"/>
        <xsl:text>            </xsl:text>
        <xsl:value-of select="$effect_body"/>
        <xsl:text>;&#xa;</xsl:text>
    </xsl:template>


</xsl:stylesheet>
