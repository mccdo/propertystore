/*************** <auto-copyright.rb BEGIN do not edit this line> **************
 *
 * Copyright 2012-2012 by Ames Laboratory
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 *************** <auto-copyright.rb END do not edit this line> ***************/
#include "PropertySet.h"
#include "Property.h"

#include <crunchstore/SearchCriterion.h>
#include <crunchstore/DataManager.h>

#include <propertystore/MakeLive.h>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/concept_check.hpp>

#include <iostream>
#include <stdexcept>

#include <Poco/Timer.h>

namespace propertystore
{
////////////////////////////////////////////////////////////////////////////////
PropertySet::PropertySet()
    :
    crunchstore::Persistable(),
    m_isLive( false ),
    m_timer( 0 ),
    m_writeDirty( false ),
    m_liveWriteDirty( false ),
    m_logger( Poco::Logger::get("PropertySet") )
{
    m_logStream = LogStreamPtr( new Poco::LogStream( m_logger ) );

    PS_LOG_TRACE( "ctor" );


    // Set NameTag to the first 4-characters of the uuid
    AddProperty("NameTag", m_UUIDString.substr( 0, 4 ), "Name Tag");
}
////////////////////////////////////////////////////////////////////////////////
PropertySet::PropertySet( const PropertySet& orig )
    :
    crunchstore::Persistable( orig ),
    m_logger( orig.m_logger ),
    m_logStream( orig.m_logStream )
{
    boost::ignore_unused_variable_warning( orig );
}
////////////////////////////////////////////////////////////////////////////////
PropertySet::~PropertySet()
{
    if(m_timer)
    {
        m_timer->restart( 0 );
        delete m_timer;
        m_timer = 0;
    }
}
////////////////////////////////////////////////////////////////////////////////
PropertySetPtr PropertySet::CreateNew()
{
    return PropertySetPtr( new PropertySet );
}
////////////////////////////////////////////////////////////////////////////////
void PropertySet::AddProperty( std::string const& propertyName,
                               boost::any value,
                               std::string uiLabel )
{
    // No-op if property already exists
    if( DatumExists( propertyName ) )
    {
        return;
    }

    PropertyPtr property = PropertyPtr( new Property( value ) );
    Persistable::AddDatumPtr( propertyName, property );

    property->SetAttribute( "uiLabel", uiLabel );
    property->SetAttribute( "nameInSet", propertyName );

    // Connect change signals to the change accumulator
    _connectChanges( property );
}
////////////////////////////////////////////////////////////////////////////////
bool PropertySet::SetPropertyValue( const std::string& propertyName,
                                    boost::any value )
{
    return SetDatumValue( propertyName, value );
}
////////////////////////////////////////////////////////////////////////////////
boost::any PropertySet::GetPropertyValue( const std::string& propertyName )
{
    try
    {
        return GetDatum( propertyName )->GetValue();
    }
    catch( std::runtime_error)
    {
        return boost::any();
    }
}
////////////////////////////////////////////////////////////////////////////////
PropertyPtr PropertySet::GetProperty( const std::string& propertyName )
{
    return boost::dynamic_pointer_cast<Property>( GetDatum( propertyName ) );
}
////////////////////////////////////////////////////////////////////////////////
bool PropertySet::PropertyExists( std::string const& propertyName ) const
{
    return DatumExists( propertyName );
}
////////////////////////////////////////////////////////////////////////////////
const PropertySet::PSVectorOfStrings&
                    PropertySet::GetPropertyAttributeList( std::string const&
                                                           propertyName )
{
    DataMap::const_iterator iterator = m_dataMap.find( propertyName );
    if( iterator != m_dataMap.end() )
    {
        //Property* property = reinterpret_cast<Property*>( iterator->second.get() );
        PropertyPtr property( iterator->second, boost::detail::static_cast_tag() );
        return property->GetAttributeList();
    }
    else
    {
        // return an empty vector of strings
        return emptyPSVectorOfStrings;
    }
}
////////////////////////////////////////////////////////////////////////////////
crunchstore::DatumPtr PropertySet::GetPropertyAttribute( std::string const& propertyName,
                                              std::string const& attributeName ) const
{
    DataMap::const_iterator iterator = m_dataMap.find( propertyName );
    if( iterator != m_dataMap.end() )
    {
        //Property* property = reinterpret_cast<Property*>( iterator->second.get() );
        PropertyPtr property( iterator->second, boost::detail::static_cast_tag() );
        return property->GetAttribute( attributeName );
    }
    else
    {
        std::string error( "PropertySet::GetPropertyAttribute: No property named " );
        error += propertyName;
        error += " in PropertySet named " + GetTypeName();
        throw std::runtime_error( error );
        //return crunchstore::DatumPtr();
    }
}
////////////////////////////////////////////////////////////////////////////////
bool PropertySet::GetPropertyEnabled( std::string const& propertyName ) const
{
    DataMap::const_iterator iterator = m_dataMap.find( propertyName );
    if( iterator != m_dataMap.end() )
    {
        //Property* property = reinterpret_cast<Property*>( iterator->second.get() );
        PropertyPtr property( iterator->second, boost::detail::static_cast_tag() );
        return property->GetEnabled();
    }
    else
    {
        return false;
    }
}
////////////////////////////////////////////////////////////////////////////////
void PropertySet::SetPropertyAttribute( std::string const& propertyName,
                                        std::string const& attributeName,
                                        boost::any value )
{
    DataMap::const_iterator iterator = m_dataMap.find( propertyName );
    if( iterator != m_dataMap.end() )
    {
        //Property* property = reinterpret_cast<Property*>( iterator->second.get() );
        PropertyPtr property( iterator->second, boost::detail::static_cast_tag() );
        property->SetAttribute( attributeName, value );
    }
}
////////////////////////////////////////////////////////////////////////////////
bool PropertySet::HasPropertyAttribute( std::string const& propertyName,
                                   std::string const& attributeName ) const
{
    DataMap::const_iterator iterator = m_dataMap.find( propertyName );
    if( iterator != m_dataMap.end() )
    {
        //Property* property = reinterpret_cast<Property*>( iterator->second.get() );
        PropertyPtr property( iterator->second, boost::detail::static_cast_tag() );
        return property->AttributeExists( attributeName );
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
void PropertySet::SetPropertyEnabled( std::string const& propertyName,
                                      bool enabled )
{
    DataMap::const_iterator iterator = m_dataMap.find( propertyName );
    if( iterator != m_dataMap.end() )
    {
        //Property* property = reinterpret_cast<Property*>( iterator->second.get() );
        PropertyPtr property( iterator->second, boost::detail::static_cast_tag() );
        if( enabled )
        {
            property->SetEnabled();
        }
        else
        {
            property->SetDisabled();
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
const PropertySet::PSVectorOfStrings& PropertySet::GetChanges()
{
    m_accumulatedChangesReturnable = m_accumulatedChanges;
    ClearAccumulatedChanges();
    return m_accumulatedChangesReturnable;
}
////////////////////////////////////////////////////////////////////////////////
void PropertySet::ClearAccumulatedChanges()
{
    m_accumulatedChanges.clear();
}
////////////////////////////////////////////////////////////////////////////////
void PropertySet::SetDataManager( crunchstore::DataManagerPtr manager )
{
    m_dataManager = manager;
}
////////////////////////////////////////////////////////////////////////////////
bool PropertySet::Remove()
{
    if( !m_dataManager )
    {
        return false;
    }
    m_dataManager->Remove( *this );
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool PropertySet::Load()
{
    //std::cout << "PropertySet::Load -- " << GetTypeName() << ": " << GetUUIDAsString() << std::endl << std::flush;
    if( !m_dataManager )
    {
        return false;
    }
    m_dataManager->Load( *this );
    m_writeDirty = false;
    return true;
}
////////////////////////////////////////////////////////////////////////////////
bool PropertySet::LoadByKey( std::string const& KeyName, boost::any KeyValue )
{
    if( !m_dataManager )
    {
        return false;
    }

    std::vector< std::string > results;
    crunchstore::SearchCriterion kvc( KeyName, "=", KeyValue );
    std::vector< crunchstore::SearchCriterion > criteria;
    criteria.push_back( kvc );

    m_dataManager->Search( GetTypeName(), criteria, "uuid", results );
    if( results.empty() )
    {
        return false;
    }

    SetUUID( results.at(0) );
    Load();
    return true;
}
////////////////////////////////////////////////////////////////////////////////
bool PropertySet::Save()
{
    //std::cout << "PropertySet::Save -- " << GetTypeName() << std::endl << std::flush;
    if( !m_dataManager )
    {
        return false;
    }
    m_dataManager->Save( *this );

    m_writeDirty = false;

    return true;
}
////////////////////////////////////////////////////////////////////////////////
void PropertySet::_connectChanges( PropertyPtr property )
{
    property->SignalAttributeChanged.connect( boost::bind( &PropertySet::
                                                           ChangeAccumulator,
                                                           this, ::_1 ) );

    property->SignalDisabled.connect( boost::bind( &PropertySet::
                                                   ChangeAccumulator,
                                                   this, ::_1 ) );

    property->SignalEnabled.connect( boost::bind( &PropertySet::
                                                  ChangeAccumulator,
                                                  this, ::_1 ) );

    property->SignalValueChanged.connect( boost::bind( &PropertySet::
                                                       ChangeAccumulator,
                                                       this, ::_1 ) );
}
////////////////////////////////////////////////////////////////////////////////
void PropertySet::ChangeAccumulator( PropertyPtr property )
{
    // Data has changed, so set dirty flag
    m_writeDirty = true;

    // Ask the property for its name
    const std::string nameInSet = 
            property->GetAttribute("nameInSet")->extract< std::string >();

    // See if we already have changes recorded for this property
    bool found = false;
    PSVectorOfStrings::const_iterator iterator = m_accumulatedChanges.begin();
    PSVectorOfStrings::const_iterator end = m_accumulatedChanges.end();
    while( ( !found ) && ( iterator != end ) )
    {
        if( ( *iterator ) == nameInSet )
        {
            found = true;
        }
        iterator++;
    }

    // Add the property's name to our list if it isn't already there, but also
    // restrict the size of the vector to 1000 elements.
    if( ( !found ) && ( m_accumulatedChanges.size() < 1000 ) )
    {
        m_accumulatedChanges.push_back( nameInSet );

        // If liveWriteDirty flag isn't already set, check whether this is a
        // live property. If so, set the  m_liveWriteDirty flag
        if( !m_liveWriteDirty )
        {
            std::vector< MakeLiveBasePtr >::const_iterator mlb =
                    m_liveObjects.begin();
            while( mlb != m_liveObjects.end() )
            {
                std::vector<std::string> liveNames = (*mlb)->GetNames();
                std::vector<std::string>::const_iterator name =
                        liveNames.begin();
                while( name != liveNames.end() )
                {
                    if( *name == nameInSet )
                    {
                        m_liveWriteDirty = true;
                        break;
                    }
                    ++name;
                }
                if( m_liveWriteDirty )
                {
                    break;
                }
                ++mlb;
            }
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
unsigned int PropertySet::GetBoostAnyVectorSize( const boost::any& value )
{
    unsigned int size = 0;
    Property temp( 0 );
    if( temp.IsIntVector( value ) )
    {
        size = boost::any_cast< std::vector<int> >( value ).size();
    }
    else if( temp.IsFloatVector( value ) )
    {
        size = boost::any_cast< std::vector<float> >( value ).size();
    }
    else if( temp.IsDoubleVector( value ) )
    {
        size = boost::any_cast< std::vector<double> >( value ).size();
    }
    else if( temp.IsStringVector( value ) )
    {
        size = boost::any_cast< std::vector<std::string> >( value ).size();
    }

    return size;
}
////////////////////////////////////////////////////////////////////////////////
void PropertySet::EnableLiveProperties( bool live )
{
    // Derived classes should override this method if they want
    // delayed live properties.

    // If live is true, we want a timer to fire every 2 seconds to call
    // SaveLiveProperties to ensure that the persisted version of this
    // PropertySet stays synced with the in-memory version. If live is false,
    // we stop the timer.
    m_isLive = live;
    if( live )
    {
        if( !m_timer )
        {
            // Create timer that fires every two seconds
            m_timer = new Poco::Timer( 2000, 2000 );
        }
        Poco::TimerCallback<PropertySet> callback(*this, &PropertySet::SaveLiveProperties);
        m_timer->start( callback );
    }
    else if( m_timer )
    {
        m_timer->restart( 0 );
    }
}
////////////////////////////////////////////////////////////////////////////////
void PropertySet::SaveLiveProperties( Poco::Timer& timer )
{
    boost::ignore_unused_variable_warning( timer );
    if( m_liveWriteDirty )
    {
        PS_LOG_INFO( "Changes detected in live property in propertyset " << m_UUIDString <<
                ": auto-saving." );
        SaveNoOverride();
    }
    else
    {
        PS_LOG_TRACE( "No live data changes detected in propertyset " << mUUIDString );
    }
}
////////////////////////////////////////////////////////////////////////////////
void PropertySet::SaveNoOverride()
{
    if( m_dataManager )
    {
        m_dataManager->Save( *this );
    }
}
////////////////////////////////////////////////////////////////////////////////
void PropertySet::EmitValueChangedSignals() const
{
    std::vector< std::string >::const_iterator itr = m_dataList.begin();
    DataMap::const_iterator mapItr;

    // Emit these signals in order of property addition
    while( itr != m_dataList.end() )
    {
        mapItr = m_dataMap.find( *itr );
        if( mapItr != m_dataMap.end() )
        {
            PropertyPtr property( mapItr->second,
                              boost::detail::polymorphic_cast_tag() );
            property->EmitValueChangedSignal();
        }
        ++itr;
    }
}
////////////////////////////////////////////////////////////////////////////////
}

