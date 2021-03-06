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
#pragma once

#include <propertystore/Property.h>
#include <propertystore/PropertySet.h>
#include <propertystore/PointerTypes.h>
#include <propertystore/Logging.h>

#include <switchwire/EventManager.h>
#include <switchwire/Event.h>

#include <string>
#include <iostream>
#include <vector>

#include <boost/bind.hpp>
#include <boost/any.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/concept_check.hpp>

/// @file MakeLive.h
/// @namespace propertystore


namespace propertystore
{

/// Base class for MakeLive and MakeLiveLinked that allows differing templated
/// types to be stored in the same container
class MakeLiveBase 
{
public:
    virtual ~MakeLiveBase(){;}
    std::vector<std::string> GetNames()
    {
        return m_propertyNames;
    }
protected:
    std::vector<std::string> m_propertyNames;
};

/// @class MakeLive
/// The easy way to turn a property in a propertyset into a live feature that
/// fires a typed signal each time the property's value changes. The template
/// parameter specifies the actual data type of the property's value. The
/// emitted signal will have signature void( const std::string&, T ), where
/// the first argument will contain the uuid passed into the ctor, and the
/// second argument is the property's value cast to type T.
template <class T>
class MakeLive : public MakeLiveBase
{

public:

    /// @param uuid Reference to the string version of the UUID of the
    ///             propertyset containing the property. This is passed in
    ///             the fired signal since slots will frequently need to know
    ///             the identity of the caller.
    /// @param property Smart pointer to the property that should be made live.
    /// @param signalName The name of the signal that should be registered. Note
    ///                   that the name provided in this parameter will not be
    ///                   the full name of the signal. The full name will
    ///                   contain a unique identifier to differentiate different
    ///                   instances of the same propertyset.
    /// @param passUUID If true, emitted signal will contain the uuid passed in
    ///                 ctor as the first argument, in addition to the value of
    ///                 the property (as the second argument). If false,
    ///                 the uuid is omitted in both the signal signature as
    ///                 well as signal emission. Default: true.
    MakeLive( const std::string& uuid, PropertyPtr property,
              const std::string& signalName,
              bool passUUID = true ):
        MakeLiveBase(),
        m_UUID(uuid),
        m_passUUID( passUUID )
    {
        std::string name = property->GetAttribute("nameInSet")->
                extract<std::string>();
        m_propertyNames.push_back( name );

        m_SignalName = boost::lexical_cast<std::string>( this );
        m_SignalName.append( "." );
        m_SignalName.append( signalName );

        using switchwire::EventManager;
        if( m_passUUID )
        {
            EventManager::instance()->RegisterSignal( &m_Signal, m_SignalName );
        }
        else
        {
            EventManager::instance()->RegisterSignal( &m_SignalNoUUID,
                                                      m_SignalName );
        }
        property->SignalValueChanged.connect(
                    boost::bind( &MakeLive<T>::ValueChangedSlot, this, _1 )  );
    }

    void ValueChangedSlot( PropertyPtr property )
    {
        try
        {
            //boost::any bval = property->GetValue();
            //if( !bval.empty() )
            {
                //T value = boost::any_cast<T>( bval );
                T value = property->extract<T>();
                if( m_passUUID )
                {
                    m_Signal.signal( m_UUID, value );
                }
                else
                {
                    m_SignalNoUUID.signal( value );
                }
            }
        }
        catch(...)
        {
            PS_STATIC_LOG_CRITICAL( "propertystore",
                                 "Failed boost::any_cast in MakeLive during attempt to call "
                                    << m_SignalName << " signal for property ");
        }
    }

private:
    typedef switchwire::Event<void(const std::string&, T)> m_Signal_Type;
    m_Signal_Type m_Signal;
    typedef switchwire::Event< void( T ) > m_SignalNoUUID_Type;
    m_SignalNoUUID_Type m_SignalNoUUID;
    std::string m_SignalName;
    const std::string& m_UUID;
    bool m_passUUID;

};


/// The easy way to turn a group of related properties in a propertyset into a
/// live feature that fires a typed signal each time any of the properties'
/// value changes. This is especially useful when the data from multiple
/// properties is required by a slot for proper processing. The linked
/// properties must all have the same underlying type.
/// The template parameter specifies the actual data type of the properties'
/// value. The emitted signal will have signature
/// void( const std::string&, std::vector<T> ), where
/// the first argument will contain the uuid passed into the ctor, and the
/// second argument is a vector containing the values of each property, in the
/// order the properties were specified in the ctor.
template <class T>
class MakeLiveLinked : public MakeLiveBase
{

public:
    /// @param uuid Reference to the string version of the UUID of the
    ///             propertyset containing the property. This is passed in
    ///             the fired signal since slots will frequently need to know
    ///             the identity of the caller.
    /// @param properties Vector of smart pointer to each of the related
    ///                   properties that should be made live.
    /// @param signalName The name of the signal that should be registered. Note
    ///                   that the name provided in this parameter will not be
    ///                   the full name of the signal. The full name will
    ///                   contain a unique identifier to differentiate different
    ///                   instances of the same propertyset.
    MakeLiveLinked( const std::string& uuid,
                    const std::vector< PropertyPtr >& properties,
                    const std::string& signalName ):
        MakeLiveBase(),
        m_UUID(uuid)
    {
        m_Properties = properties;

        m_SignalName = boost::lexical_cast<std::string>( this );
        m_SignalName.append( "." );
        m_SignalName.append( signalName );

        using switchwire::EventManager;
        EventManager::instance()->RegisterSignal( &m_Signal, m_SignalName );

        std::vector< PropertyPtr >::const_iterator iter = m_Properties.begin();
        while( iter != m_Properties.end() )
        {
            (*iter)->SignalValueChanged.connect(
                        boost::bind( &MakeLiveLinked<T>::ValueChangedSlot,
                                     this, _1 )  );
            std::string name =
                    (*iter)->GetAttribute("nameInSet")->extract<std::string>();
            m_propertyNames.push_back( name );
            ++iter;
        }
    }

    void ValueChangedSlot( PropertyPtr property )
    {
        boost::ignore_unused_variable_warning( property );

        try
        {
            std::vector< T > result;

            std::vector< PropertyPtr >::const_iterator iter =
                    m_Properties.begin();
            while( iter != m_Properties.end() )
            {

                result.push_back( (*iter)->extract<T>() );
                ++iter;
            }
            m_Signal.signal( m_UUID, result );
        }
        catch(...)
        {
            PS_STATIC_LOG_CRITICAL( "propertystore",
                                 "Failed value extraction in MakeLiveLinked during attempt to call "
                                    << m_SignalName << " signal for property ");
        }
    }

private:
    typedef switchwire::Event< void( const std::string&, const std::vector< T >& ) > m_Signal_Type;
    m_Signal_Type m_Signal;
    std::string m_SignalName;
    const std::string& m_UUID;

    std::vector< PropertyPtr > m_Properties;
};

} // namespace
