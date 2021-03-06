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
//#define PROPERTYSTORE_DEBUG
#include "PropertySet.h"
#include "Property.h"

#include <crunchstore/SearchCriterion.h>
#include <crunchstore/DataManager.h>

#include <propertystore/MakeLive.h>

#include <boost/bind.hpp>
#include <boost/cast.hpp>
#include <boost/version.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/concept_check.hpp>
#include <boost/shared_ptr.hpp>

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
    m_dataManager( orig.m_dataManager ),
    m_timer( 0 ),
    m_writeDirty( false ),
    m_liveWriteDirty( false ),
    m_logger( orig.m_logger ),
    m_logStream( orig.m_logStream )
{
    // The copy of crunchstore::Persistable in the initializer list puts
    // instances of Datum into m_dataMap. We need to get rid of those and
    // replace them with instances of the derived class Property so property
    // attributes get copied correctly.
    m_dataMap.clear();

    DataMap::const_iterator it = orig.m_dataMap.begin();
    while( it != orig.m_dataMap.end() )
    {
        PropertyPtr property( new Property( *(boost::static_pointer_cast<Property>(it->second)) ));
        m_dataMap[ it->first ] = property;
        _connectChanges( property );
        ++it;
    }
}
////////////////////////////////////////////////////////////////////////////////
PropertySet::~PropertySet()
{
    PS_LOG_TRACE( "dtor" );
    if(m_timer)
    {
        PS_LOG_TRACE( "dtor: stopping timer" );
        m_timer->restart( 0 );
        //m_timer->stop():
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
        PropertyPtr property = boost::static_pointer_cast<Property>( iterator->second );
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
        PropertyPtr property = boost::static_pointer_cast<Property>( iterator->second );
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
        PropertyPtr property = boost::static_pointer_cast<Property>( iterator->second );
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
        PropertyPtr property = boost::static_pointer_cast<Property>( iterator->second );
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
        PropertyPtr property = boost::static_pointer_cast<Property>( iterator->second );
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
        PropertyPtr property = boost::static_pointer_cast<Property>( iterator->second );
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
    PS_LOG_TRACE( "Save" );
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
bool PropertySet::Save( const crunchstore::TransactionKey &transactionKey )
{
    if( !m_dataManager )
    {
        return false;
    }
    m_dataManager->Save( *this, crunchstore::Store::DEFAULT_ROLE, transactionKey );

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

    // Add the property's name to our list if it isn't already there, but also
    // restrict the size of the vector to 1000 elements.
    if( ( !found ) && ( m_accumulatedChanges.size() < 1000 ) )
    {
        m_accumulatedChanges.push_back( nameInSet );
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
    PS_LOG_TRACE( "EnableLiveProperties: " << live );
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
void PropertySet::SaveLiveProperties( Poco::Timer&  )
{
    if( m_liveWriteDirty )
    {
        PS_LOG_INFO( "Changes detected in live property in propertyset " << m_UUIDString <<
                ": auto-saving." );
        SaveNoOverride();
        m_liveWriteDirty = false;
    }
    else
    {
        //PS_LOG_TRACE( "No live data changes detected in propertyset " << m_UUIDString );
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
            PropertyPtr property = boost::reinterpret_pointer_cast<Property>( mapItr->second );
            property->EmitValueChangedSignal();
        }
        ++itr;
    }
}
////////////////////////////////////////////////////////////////////////////////
}

