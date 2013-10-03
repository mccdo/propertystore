#include "DeclarativePropertySet.h"

#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Handler.h>
#include <Poco/Version.h>
#if POCO_VERSION >= 1050200
#include <Poco/JSON/ParseHandler.h>
#else
#include <Poco/JSON/DefaultHandler.h>
#endif
#include <Poco/Dynamic/Var.h>
#include "Poco/Path.h"
#include "Poco/File.h"
#include "Poco/FileStream.h"
#include "Poco/StreamCopier.h"
#include "Poco/JSON/JSONException.h"

namespace propertystore
{



////////////////////////////////////////////////////////////////////////////////
DeclarativePropertySet::DeclarativePropertySet()
{
}
////////////////////////////////////////////////////////////////////////////////
void DeclarativePropertySet::EnableLiveProperties( bool enable )
{
    EnableDelayedLiveSignals<bool>( m_delayedBooleans );
    EnableDelayedLiveSignals<int>( m_delayedIntegers );
    EnableDelayedLiveSignals<float>( m_delayedFloats );
    EnableDelayedLiveSignals<double>( m_delayedDoubles );
    EnableDelayedLiveSignals<std::string>( m_delayedStrings );
    EnableDelayedLiveSignals< std::vector<char> >( m_delayedBLOBs );

    PropertySet::EnableLiveProperties( enable );
}
////////////////////////////////////////////////////////////////////////////////
void DeclarativePropertySet::FromJSONFile( const std::string& filename )
{
    Poco::Path filePath( filename );

    std::ostringstream ostr;

    if ( filePath.isFile() )
    {
        Poco::File inputFile( filePath );
        if ( inputFile.exists() )
        {
            Poco::FileInputStream fis( filePath.toString() );
            Poco::StreamCopier::copyStream( fis, ostr );
        }
        else
        {
            std::cout << filePath.toString() << " doesn't exist!" << std::endl;
            return;
        }
    }

    std::string json = ostr.str();
    FromJSON( json );
}
////////////////////////////////////////////////////////////////////////////////
void DeclarativePropertySet::FromJSON(const std::string &json)
{
    try{
        Poco::JSON::ParseHandler handler;
        Poco::JSON::Parser parser;
        parser.setHandler( &handler );
        parser.parse( json );
#if POCO_VERSION >= 1050200
        Poco::DynamicAny result = handler.asVar();
#else
        Poco::DynamicAny result = handler.result();
#endif
        Poco::JSON::Object::Ptr set;
        if ( result.type() == typeid( Poco::JSON::Object::Ptr ) )
        {
            set = result.extract<Poco::JSON::Object::Ptr>();
        }

        if( !set->has( "TypeName" ) )
        {
            // throw an exception about malformed propertyset without typename
        }
        this->SetTypeName( set->getValue<std::string>( "TypeName" ) );

        Poco::JSON::Object::Ptr properties;
        properties = set->getObject( "properties" );
        std::vector< std::string > propertyNames;
        if( !set->has( "propertyOrder" ) )
        {
            //throw malformed declaration exception: no propertyOrder array
            std::cout << "No propertyOrder array!" << std::endl;
            return;
        }
        Poco::JSON::Array::Ptr order = set->getArray( "propertyOrder" );
        for( unsigned int idx = 0; idx < order->size(); ++idx )
        {
            std::string propertyName = order->getElement<std::string>(idx);

            Poco::DynamicAny propertyValue = properties->get( propertyName );
            if( propertyValue.type() == typeid( Poco::JSON::Object::Ptr ) )
            {
                Poco::JSON::Object::Ptr property = propertyValue.extract<Poco::JSON::Object::Ptr>();

                // isUIGroupOnly and having a type are mutually exclusive attributes
                if( property->has( "isUIGroupOnly" ) )
                {
                    if( property->getValue<bool>( "isUIGroupOnly" ) )
                    {
                        this->AddProperty( propertyName, boost::any(), GetUILabel( property ) );
                        this->SetPropertyAttribute( propertyName, "isUIGroupOnly", true );
                    }
                    property->remove( "isUIGroupOnly" );
                }
                // UUID and NameTag are added internally by the PropertySet,
                // so we must ensure they're not added again
                else if( (property->has( "type" )) &&
                         (propertyName != "UUID") &&
                         (propertyName != "NameTag") )
                {
                    std::string type = property->getValue<std::string>( "type" );
                    property->remove( "type" );
                    if( type == "boolean" )
                    {
                        AddBoolean( propertyName, property );
                    }
                    else if( type == "integer" )
                    {
                        AddInteger( propertyName, property );
                    }
                    else if( type == "float" )
                    {
                        AddFloat( propertyName, property );
                    }
                    else if( type == "double" )
                    {
                        AddDouble( propertyName, property );
                    }
                    else if( type == "string" )
                    {
                        AddString( propertyName, property );
                    }
                    else if( type == "BLOB" )
                    {
                        AddBLOB( propertyName, property );
                    }
                }
                else
                {
                    // throw exception about invalid property definition
                }

                // We've been removing special attributes along the way as we
                // deal with them. Anything that's left shouldn't need special
                // attention, so we can just iterate through the remaining
                // attributes and set them based on apparent type.
                std::vector< std::string > attributeNames;
                property->getNames( attributeNames );
                for( size_t adx = 0; adx < attributeNames.size(); ++adx )
                {
                    std::string attrName = attributeNames.at( adx );
                    Poco::DynamicAny attr = property->get( attrName );
                    if( attr.type() == typeid( int ) )
                    {
                        this->SetPropertyAttribute( propertyName, attrName, attr.extract<int>() );
                    }
                    else if( attr.type() == typeid( double ) )
                    {
                        this->SetPropertyAttribute( propertyName, attrName, attr.extract<double>() );
                    }
                    else if( attr.type() == typeid( bool ) )
                    {
                        this->SetPropertyAttribute( propertyName, attrName, attr.extract<bool>() );
                    }
                    else if( attr.type() == typeid( std::string ) )
                    {
                        this->SetPropertyAttribute( propertyName, attrName, attr.extract<std::string>() );
                    }
                }
            }
        }
    }
    catch(Poco::JSON::JSONException jsone)
    {
        std::cout << jsone.message() << std::endl;
    }
}
////////////////////////////////////////////////////////////////////////////////
std::string DeclarativePropertySet::GetUILabel( Poco::JSON::Object::Ptr property )
{
    std::string uiLabel( "" );
    if( property->has( "uiLabel" ) )
    {
        uiLabel = property->getValue< std::string >( "uiLabel" );
        property->remove( "uiLabel" );
    }
    return uiLabel;
}
////////////////////////////////////////////////////////////////////////////////
void DeclarativePropertySet::SetMinMax( const std::string& name, Poco::JSON::Object::Ptr property )
{
    if( property->has( "minimum" ) )
    {
        double min = property->getValue< double >( "minimum" );
        this->SetPropertyAttribute( name, "minimumValue", min );
        property->remove( "minimum" );
    }
    if( property->has( "maximum" ) )
    {
        double max = property->getValue< double >( "maximum" );
        this->SetPropertyAttribute( name, "maximumValue", max );
        property->remove( "maximum" );
    }
}
////////////////////////////////////////////////////////////////////////////////
void DeclarativePropertySet::AddInteger( const std::string& name, Poco::JSON::Object::Ptr property )
{
    int defaultValue = 0;
    if( property->has( "default" ) )
    {
        defaultValue = property->getValue< int >( "default" );
        property->remove( "default" );
    }

    this->AddProperty( name, defaultValue, GetUILabel( property ) );

    SetMinMax( name, property );

    ProcessLiveSignal<int>( name, property, m_delayedIntegers );
}
////////////////////////////////////////////////////////////////////////////////
void DeclarativePropertySet::AddDouble( const std::string& name, Poco::JSON::Object::Ptr property )
{
    double defaultValue = 0.0;
    if( property->has( "default" ) )
    {
        defaultValue = property->getValue< double >( "default" );
        property->remove( "default" );
    }

    this->AddProperty( name, defaultValue, GetUILabel( property ) );

    SetMinMax( name, property );

    ProcessLiveSignal<double>( name, property, m_delayedDoubles );
}
////////////////////////////////////////////////////////////////////////////////
void DeclarativePropertySet::AddFloat( const std::string& name, Poco::JSON::Object::Ptr property )
{
    float defaultValue = 0.0f;
    if( property->has( "default" ) )
    {
        defaultValue = property->getValue< float >( "default" );
        property->remove( "default" );
    }

    this->AddProperty( name, defaultValue, GetUILabel( property ) );

    SetMinMax( name, property );

    ProcessLiveSignal<float>( name, property, m_delayedFloats );
}
////////////////////////////////////////////////////////////////////////////////
void DeclarativePropertySet::AddBoolean( const std::string& name, Poco::JSON::Object::Ptr property )
{
    bool defaultValue = false;
    if( property->has( "default" ) )
    {
        defaultValue = property->getValue< bool >( "default" );
        property->remove( "default" );
    }

    this->AddProperty( name, defaultValue, GetUILabel( property ) );

    ProcessLiveSignal<bool>( name, property, m_delayedBooleans );
}
////////////////////////////////////////////////////////////////////////////////
void DeclarativePropertySet::AddString( const std::string& name, Poco::JSON::Object::Ptr property )
{
    std::string defaultValue = "";
    if( property->has( "default" ) )
    {
        defaultValue = property->getValue< std::string >( "default" );
        property->remove( "default" );
    }

    this->AddProperty( name, defaultValue, GetUILabel( property ) );

    if( property->has( "enumValues" ) )
    {
        Poco::JSON::Array::Ptr enumArray = property->getArray( "enumValues" );
        std::vector< std::string > enums;
        for( unsigned int idx = 0; idx < enumArray->size(); ++idx )
        {
            enums.push_back( enumArray->getElement< std::string >( idx ) );
        }
        this->SetPropertyAttribute( name, "enumValues", enums );
        property->remove( "enumValues" );
    }

    ProcessLiveSignal<std::string>( name, property, m_delayedStrings );
}
////////////////////////////////////////////////////////////////////////////////
void DeclarativePropertySet::AddBLOB( const std::string& name, Poco::JSON::Object::Ptr property )
{
    std::vector< char > defaultValue;
    if( property->has( "default" ) )
    {
        // We don't support default values for BLOBS because it's ridiculously
        // unwieldy, so we just remove this attribute if it exists
        property->remove( "default" );
    }

    this->AddProperty( name, defaultValue, GetUILabel( property ) );

    // BLOBS cannot be shown in the UI using the default propertystore browsers;
    // we enforce this explicitly
    this->SetPropertyAttribute( name, "userVisible", false );
    property->remove( "userVisible" );

    ProcessLiveSignal< std::vector<char> >( name, property, m_delayedBLOBs );
}
////////////////////////////////////////////////////////////////////////////////
} //namespace
