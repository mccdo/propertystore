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
#define PROPERTYSTORE_DEBUG
#include <propertystore/PropertyParser.h>

#include <iostream>
#include <cmath>

namespace propertystore
{
////////////////////////////////////////////////////////////////////////////////
PropertyParser::PropertyParser( QObject* parent ) :
    QObject( parent ),
    m_ignoreValueChanges( false ),
    m_parseOperation( false ),
    m_displayUnitSystem( "USDisplayUnit" ),
    m_logger( Poco::Logger::get("PropertyParser") ),
    m_logStream( LogStreamPtr( new Poco::LogStream( m_logger ) ) )
{
    // Create a standard set of property managers
    mDoubleManager = new QtDoublePropertyManager( this );
    mStringManager = new QtStringPropertyManager( this );
    mBooleanManager = new QtBoolPropertyManager( this );
    mEnumManager = new QtEnumPropertyManager( this );
    mGroupManager = new QtGroupPropertyManager( this );
    mIntManager = new QtIntPropertyManager( this );

    // Connect managers' valueChanged signals to our slots
    connect( mBooleanManager, SIGNAL( valueChanged( QtProperty*, bool ) ),
             this, SLOT( BoolValueChanged( QtProperty*, bool ) ) );
    connect( mIntManager, SIGNAL( valueChanged( QtProperty*, int ) ),
             this, SLOT( IntValueChanged( QtProperty*, int ) ) );
    connect( mDoubleManager, SIGNAL( valueChanged( QtProperty*, double ) ),
             this, SLOT( DoubleValueChanged( QtProperty*, double ) ) );
    connect( mEnumManager, SIGNAL( valueChanged( QtProperty*, int ) ),
             this, SLOT( IntValueChanged( QtProperty*, int ) ) );
    connect( mStringManager, SIGNAL( valueChanged( QtProperty*, QString ) ),
             this, SLOT( StringValueChanged( QtProperty*, QString ) ) );
}
////////////////////////////////////////////////////////////////////////////////
PropertyParser::~PropertyParser()
{
    // Delete items
    size_t max = mItems.size();
    for( size_t index = 0; index < max; index++ )
    {
        delete mItems[index];
    }

    mItems.clear();
    mProperties.clear();
    mTreedItems.clear();
    mGroupManager->clear();
    mBooleanManager->clear();
    mEnumManager->clear();
    mIntManager->clear();
    mDoubleManager->clear();
    mStringManager->clear();

    std::map< std::string, ExternalStringSelectManager* >::iterator itr =
            m_customStringManagers.begin();
    while( itr != m_customStringManagers.end() )
    {
        itr->second->clear();
        delete itr->second;
        ++itr;
    }

    delete mGroupManager;
    delete mBooleanManager;
    delete mEnumManager;
    delete mIntManager;
    delete mDoubleManager;
    delete mStringManager;
}
////////////////////////////////////////////////////////////////////////////////
QtDoublePropertyManager* PropertyParser::GetDoubleManager()
{
    return mDoubleManager;
}
////////////////////////////////////////////////////////////////////////////////
QtStringPropertyManager* PropertyParser::GetStringManager()
{
    return mStringManager;
}
////////////////////////////////////////////////////////////////////////////////
QtBoolPropertyManager* PropertyParser::GetBoolManager()
{
    return mBooleanManager;
}
////////////////////////////////////////////////////////////////////////////////
QtEnumPropertyManager* PropertyParser::GetEnumManager()
{
    return mEnumManager;
}
////////////////////////////////////////////////////////////////////////////////
QtGroupPropertyManager* PropertyParser::GetGroupManager()
{
    return mGroupManager;
}
////////////////////////////////////////////////////////////////////////////////
QtIntPropertyManager* PropertyParser::GetIntManager()
{
    return mIntManager;
}
////////////////////////////////////////////////////////////////////////////////
void PropertyParser::AddCustomExternalStringManager(
        ExternalStringSelectManager* manager,
        const std::string& attributeFlag )
{
    // Reparent the manager to keep the Qt hierarchy correct
    manager->setParent(this);
    connect( manager, SIGNAL(valueChanged(QtProperty*,QString)),
                 this, SLOT(StringValueChanged(QtProperty*,QString)));
    m_customStringManagers[ attributeFlag ] = manager;
}
////////////////////////////////////////////////////////////////////////////////
PropertyParser::ItemVector* PropertyParser::GetItems()
{
    return &mTreedItems;
}
////////////////////////////////////////////////////////////////////////////////
void PropertyParser::ParsePropertySet( PropertySetPtr set )
{
    PS_LOG_TRACE( "ParsePropertySet" );
    m_parseOperation = true;

    // Delete old items, etc. before adding new
    size_t max = mItems.size();
    for( size_t index = 0; index < max; index++ )
    {
        delete mItems[index];
    }
    mItems.clear();
    mProperties.clear();
    mTreedItems.clear();
    mGroupManager->clear();
    mBooleanManager->clear();
    mEnumManager->clear();
    mIntManager->clear();
    mDoubleManager->clear();
    mStringManager->clear();

    std::map< std::string, ExternalStringSelectManager* >::iterator itr =
            m_customStringManagers.begin();
    while( itr != m_customStringManagers.end() )
    {
        itr->second->clear();
        ++itr;
    }

    mSet = set;

    // If we were passed a null set our slate has been cleaned above and we 
    // stop here.
    if( !mSet )
    {
        return;
    }
    // Ask the set for a list of its properties
    mPropertyNames = set->GetDataList();

    // Walk through properties list and store a pointer to each underlying property
    { // Bracket used to scope iterator and end
        PropertySet::PSVectorOfStrings::iterator iterator;
        //PropertySet::PSVectorOfStrings::iterator end = mPropertyNames.end();
        for( iterator = mPropertyNames.begin(); iterator != mPropertyNames.end();  )
        {
            // If the userVisible attribute is false, do not add this property
            // to any of our lists.
            bool show = true;
            if( set->HasPropertyAttribute( *iterator, "userVisible" ) )
            {
                show = set->GetPropertyAttribute( *iterator, "userVisible" )->
                        extract<bool>();
            }
            if( show )
            {
                //mProperties.push_back( PropertyPtr(static_cast<Property*>(set->GetDatum( ( *iterator ) ).get())) );
                mProperties.push_back( PropertyPtr(
                                        boost::dynamic_pointer_cast<Property>
                                        ( set->GetDatum( ( *iterator ) ) ) ) );
                PS_LOG_TRACE( "Adding property named " << (*iterator) );
                ++iterator;
            }
            else
            {
                PS_LOG_TRACE( "Not adding property named " << (*iterator) );
                iterator = mPropertyNames.erase( iterator );
            }
        }
    }

    // Look at each property in turn and create an appropriate browser item
    // for it
    PropertyVector::iterator iterator;
    PropertyVector::iterator end = mProperties.end();
    PropertyPtr property;
    for( iterator = mProperties.begin(); iterator != end; iterator++ )
    {
        property = ( *iterator );
        QtProperty* item = NULL;


        std::string propertyLabel = ( property->GetAttribute( "uiLabel" )
                                      ->extract<std::string>() );

        //Check if the property has a USDisplayLabel or SIDisplayLabel
        //set on it. If so, use the appropriate label instead of the uiLabel.
        //Only double values might have these.
        if( property->IsDouble() )
        {
            if( "US" == m_displayUnitSystem )
            {
                if( property->AttributeExists( "USDisplayLabel" ) )
                {
                    propertyLabel = property->
                       GetAttribute( "USDisplayLabel" )->extract<std::string>();
                }
            }
            else if( "SI" == m_displayUnitSystem )
            {
                if( property->AttributeExists( "SIDisplayLabel" ) )
                {
                    propertyLabel = property->
                       GetAttribute( "SIDisplayLabel" )->extract<std::string>();
                }
            }
        }

        // Convert label to type needed by Qt functions
        QString label = QString::fromStdString( propertyLabel );

        boost::any value = property->GetValue();

        // Check whether this property should be visible to users
        bool show = true;
        if( property->AttributeExists( "userVisible" ) )
        {
            show = property->GetAttribute( "userVisible" )->extract<bool>();
        }

        // Do type-specific item creation operations
        if( show )
        {
            if( value.empty() )
            {
                // Empty value...is this property intended to be a group?
                if( property->AttributeExists( "isUIGroupOnly" ) )
                {
                    item = mGroupManager->addProperty( label );
                }
            }
            else if( property->IsBool() )
            {
                item = mBooleanManager->addProperty( label );
            }
            else if( property->IsInt() )
            {
                item = mIntManager->addProperty( label );
            }
            else if( property->IsFloat() )
            {
                // Qt's property browser doesn't handle floats directly,
                // so all floats must be cast to double. We cast back and forth
                // in gets and sets to make sure that the underlying data type
                // remains float.
                item = mDoubleManager->addProperty( label );
            }
            else if( property->IsDouble() )
            {
                item = mDoubleManager->addProperty( label );
            }
            // We must be careful to ask about Enum status *BEFORE* string status,
            // since enums *ARE* strings. But not all strings are Enums....
            else if( property->IsEnum() )
            {
                item = mEnumManager->addProperty( label );
            }
            else if( property->IsString() )
            {
                PS_LOG_DEBUG( "Looping through custom string managers" );
                bool found = false;
                std::map< std::string, ExternalStringSelectManager* >::iterator itr =
                        m_customStringManagers.begin();
                while( itr != m_customStringManagers.end() )
                {
                    std::string str = itr->first;
                    if( (property->AttributeExists( str )) &&
                            ( property->GetAttribute( str ))->extract<bool>() )
                    {
                        found = true;
                        item = itr->second->addProperty( label );
                    }
                    ++itr;
                }
                if( !found )
                {
                    item = mStringManager->addProperty( label );
                }

            }

            // These are done for all items
            if( item )
            {
                // Tooltip block could potentially move down into _refreshItem
                // if it's likely tooltips might change frequently. Current
                // location ensures fewer calls to look at uiTooTip though.
//                if( property->AttributeExists( "uiToolTip" ) )
//                {
//                    QString toolTip = QString::fromStdString(
//                                property->GetAttribute( "uiToolTip" )->
//                                                       extract<std::string>() );
//                    if( !toolTip.isEmpty() )
//                    {
//                        item->setToolTip( toolTip );
//                    }
//                }

                mItems.push_back( item );
                _refreshItem( _getItemIndex( item ) );
            }
        }
    }

    // Set up specified hierarchy of elements
    _createHierarchy();
    m_parseOperation = false;
}
////////////////////////////////////////////////////////////////////////////////
void PropertyParser::RefreshAll()
{
    PS_LOG_TRACE( "RefreshAll" );
    int max = static_cast < int > ( mProperties.size() );
    for( int index = 0; index < max; index++ )
    {
        _refreshItem( index );
    }
}
////////////////////////////////////////////////////////////////////////////////
void PropertyParser::_refreshItem( int index )
{
    // This log message is usually too verbose to be useful
    //PS_LOG_TRACE( "_refreshItem " << index );
    if( index >= static_cast < int > ( mItems.size() ) )
    {
        // We haven't set this item up yet!
        return;
    }

    PropertyPtr property = mProperties[index];
    QtProperty* item = mItems[index];

    boost::any value = property->GetValue();

    bool hasMin = false;
    bool hasMax = false;
    double min = 0.0;
    double max = 0.0;
    _extractMinMaxValues( property, &min, &max, &hasMin, &hasMax );

    PS_LOG_TRACE( "Refreshing " << property->GetAttribute("nameInSet")->extract<std::string>() );

    // Block signals from all Qt...Manager instances so that altering range
    // or other non-value settings of an item does not trigger a value change.
    //_blockManagerSignals( true );
    m_ignoreValueChanges = true;

    // Do type-specific extra operations such as setting min/max
    if( property->IsEnum() )
    {
        // Update the list of valid choices
        QStringList qEnumNames;
        Property::PSVectorOfStrings enumNames =
                property->GetAttribute( "enumValues" )
                ->extract< Property::PSVectorOfStrings >() ;
        Property::PSVectorOfStrings::iterator iterator;
        Property::PSVectorOfStrings::iterator end = enumNames.end();
        for( iterator = enumNames.begin(); iterator != end; iterator++ )
        {
            qEnumNames << QString::fromStdString( ( *iterator ) );
        }
        mEnumManager->setEnumNames( item, qEnumNames );
    }
    else if( property->IsInt() )
    {
        if( hasMin )
        {
            mIntManager->setMinimum( item, static_cast < int > ( min ) );
        }
        if( hasMax )
        {
            mIntManager->setMaximum( item, static_cast < int > ( max ) );
        }
        //int currentMin = mIntManager->minimum( item );
        //int currentMax = mIntManager->maximum( item );
        //int step = static_cast < int > ( ( currentMax - currentMin ) / 100.0 );
        int step = 1;
        if( property->AttributeExists("StepSize") )
        {
            int tStep = property->GetAttribute( "StepSize" )->extract<int>();
            // Require positive, non-zero step size
            if( tStep > 0 )
            {
                step = tStep;
            }
        }
        mIntManager->setSingleStep( item, step );
    }
    else if( property->IsFloat() )
    {
        if( hasMin )
        {
            mDoubleManager->setMinimum( item, min );
        }
        if( hasMax )
        {
            mDoubleManager->setMaximum( item, max );
        }

        int precision = 2;
        if( property->AttributeExists("DisplayPrecision") )
        {
            precision = property->GetAttribute( "DisplayPrecision" )->extract<int>();
        }
        mDoubleManager->setDecimals( item, precision );

        // Pressing arrow keys or spinner arrows should change the values in
        // the least significant figure.
        double step = std::pow( double(10), double(-1 * precision) );
        mDoubleManager->setSingleStep( item, step );
    }
    else if( property->IsDouble() )
    {
        if( hasMin )
        {
            mDoubleManager->setMinimum( item, min );
        }
        if( hasMax )
        {
            mDoubleManager->setMaximum( item, max );
        }

        int precision = 2;
        if( property->AttributeExists("DisplayPrecision") )
        {
            precision = property->GetAttribute( "DisplayPrecision" )->extract<int>();
        }
        mDoubleManager->setDecimals( item, precision );

        // Pressing arrow keys or spinner arrows should change the values in
        // the least significant figure.
        double step = std::pow( double(10), double(-1 * precision) );
        mDoubleManager->setSingleStep( item, step );
    }
    else if( property->IsString() )
    {
        // Update the attribute map for the string.
        QMap< std::string, std::string> attributes;
        std::vector< std::string > attributeList = property->GetAttributeList();
        for( size_t index = 0; index < attributeList.size(); ++index )
        {
            std::string attrName = attributeList.at( index );
            if( property->GetAttribute( attrName )->IsString() )
            {
                attributes[ attrName ] = property->GetAttribute( attrName )->extract<std::string>();
            }
        }

        // Find the correct ExternalStringManager and set this item's attributes
        std::map< std::string, ExternalStringSelectManager* >::iterator itr =
                m_customStringManagers.begin();
        while( itr != m_customStringManagers.end() )
        {
            std::string str = itr->first;
            if( (property->AttributeExists( str )) &&
                    property->GetAttribute( str )->extract<bool>() )
            {
                itr->second->SetStringAttributes( item, attributes );
                break;
            }
            ++itr;
        }
    }

    // Update tooltips on the property
    if( property->AttributeExists( "uiToolTip" ) )
    {
        QString toolTip = QString::fromStdString(
                    property->GetAttribute( "uiToolTip" )->
                                           extract<std::string>() );
        if( !toolTip.isEmpty() )
        {
            item->setToolTip( toolTip );
        }
    }

    // Unblock value changed signals from managers. Failure to do this will
    // cause the value set below in _setItemValue to never show up in the browser.
    //_blockManagerSignals( false );
    m_ignoreValueChanges = false;

    // Update value
    _setItemValue( item, property );

    // Update enabled state
    item->setEnabled( property->GetEnabled() );
}
////////////////////////////////////////////////////////////////////////////////
void PropertyParser::_extractMinMaxValues( PropertyPtr property,
                                            double* min, double* max,
                                            bool* hasMin, bool* hasMax )
{
    if( property->AttributeExists( "minimumValue" ) )
    {
        ( *hasMin ) = true;
        crunchstore::DatumPtr minVal = property->GetAttribute( "minimumValue" );
        if( minVal->IsInt() )
        {
            ( *min ) = static_cast < double > (  minVal->extract<int>() );
        }
        else if( minVal->IsFloat() )
        {
            ( *min ) = static_cast < double > ( minVal->extract<float>() );
        }
        else if( minVal->IsDouble() )
        {
            ( *min ) = minVal->extract<double>();
        }
    }
    if( property->AttributeExists( "maximumValue" ) )
    {
        ( *hasMax ) = true;
        crunchstore::DatumPtr maxVal = property->GetAttribute( "maximumValue" );
        if( maxVal->IsInt() )
        {
            ( *max ) = static_cast < double > ( maxVal->extract<int>() );
        }
        else if( maxVal->IsFloat() )
        {
            ( *max ) = static_cast < double > ( maxVal->extract<float>() );
        }
        else if( maxVal->IsDouble() )
        {
            ( *max ) = maxVal->extract<double>();
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
void PropertyParser::_createHierarchy()
{
    PS_LOG_TRACE( "_createHierarchy" );
    PropertyPtr property;
    int index;
    int max = static_cast < int > ( mProperties.size() );
    for( index = 0; index < max; index++ )
    {
        QtProperty* item = mItems[index];
        bool subItem = false;
        property = mProperties[index];

        // Get the property's name
        std::string propertyName =
                property->GetAttribute( "nameInSet" )->extract<std::string>();
        QString name = QString::fromStdString( propertyName );

        // Put this item into the hierachical arrangement specified by its
        // name. Eg.: if name = "Axes_XLabel", then this item should
        // be a sub-item of the item named "Axes".
        QStringList nameList = name.split( "_" );
        if( nameList.size() > 1 )
        {
            // Rebuild the parent item's name, which may itself be a sub-item
            nameList.removeLast();
            QString parentName = nameList.join( "_" );

            // Do we have an item with the supposed parent's name?
            int parentIndex = -1;
            parentIndex = _getPropertyIndexByName( parentName.toStdString() );
            if( parentIndex > -1 )
            {
                // Got a hit; add this as sub-item
                subItem = true;
                QtProperty* parent = mItems[parentIndex];
                PS_LOG_TRACE( "parentIndex = " << parentIndex <<
                           " Looked for parent " << parentName.toStdString() );
                PS_LOG_TRACE( " and picked up property with label "
                           << parent->propertyName().toStdString() );
                parent->addSubProperty( item );
            }
            else
            {
                PS_LOG_DEBUG ( "No (visible) parent property named "
                              << parentName.toStdString() << " for property "
                              << propertyName );
            }

        }

        if( !subItem )
        {
            // We only add the item directly to this list if it is NOT a
            // subItem. SubItems will get automatically added to a viewer by
            // simply adding their parent.
            mTreedItems.push_back( item );
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
void PropertyParser::BoolValueChanged( QtProperty* item, bool value )
{
    if( m_ignoreValueChanges )
    {
        return;
    }

    PS_LOG_TRACE( "BoolValueChanged" );
    _setPropertyValue( item, value );
}
////////////////////////////////////////////////////////////////////////////////
void PropertyParser::IntValueChanged( QtProperty* item, int value )
{
    if( m_ignoreValueChanges )
    {
        return;
    }

    PS_LOG_TRACE( "IntValueChanged" );
    _setPropertyValue( item, value );
}
////////////////////////////////////////////////////////////////////////////////
void PropertyParser::StringValueChanged( QtProperty* item, const QString & value )
{
    if( m_ignoreValueChanges )
    {
        return;
    }

    PS_LOG_TRACE( "StringValueChanged" );
    std::string castValue = value.toStdString();
    _setPropertyValue( item, castValue );
}
////////////////////////////////////////////////////////////////////////////////
void PropertyParser::DoubleValueChanged( QtProperty* item, double value )
{
    if( m_ignoreValueChanges )
    {
        return;
    }

    PS_LOG_TRACE( "DoubleValueChanged: " << value );
    int index = _getItemIndex( item );
    if( index > -1 )
    {
        PropertyPtr property = mProperties[index];

        // If there are no units, or if DisplayUnits and StorageUnits are the
        // same, set the value and skip the rest of the unit conversion logic
        if( property->AttributeExists("StorageUnitSystem") )
        {
            if( property->GetAttribute( "StorageUnitSystem" )
                    ->extract<std::string>() == m_displayUnitSystem )
            {
                _setPropertyValue( item, value );
                return;
            }
        }
        else
        {
            _setPropertyValue( item, value );
            return;
        }

        // If this property has a unit conversion set on it, we need to convert
        // from display units back into storage units.
        // Must cast to float if the underlying type is really a float
        double unitHashTest = _getUnitConvertedValue( property );
        if( unitHashTest == value )
        {
            // The value we've just been handed is simply the stored
            // value converted to new units. We don't want to change the stored
            // value in this case!
            return;
        }
        double cVal = _getUnitConvertedValue( property, true, value );
        if( property->IsFloat() )
        {
            _setPropertyValue( item, static_cast < float > ( cVal ) );
        }
        else
        {
            _setPropertyValue( item, cVal );
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
void PropertyParser::Refresh()
{
    PS_LOG_TRACE( "Refresh" );
    // Request list of changed properties from set
    PropertySet::PSVectorOfStrings changes = mSet->GetChanges();
    PropertySet::PSVectorOfStrings::iterator iterator;
    PropertySet::PSVectorOfStrings::iterator end = changes.end();

    // Walk through list and update the UI items
    for( iterator = changes.begin(); iterator != end; iterator++ )
    {
        int index = _getPropertyIndexByName( ( *iterator ) );
        if( index > -1 )
        {
            _refreshItem( index );
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
int PropertyParser::_getPropertyIndex( const PropertyPtr property ) const
{
    // Get the index of this property in mProperties. The corresponding Item will
    // have the same index in mItems.
    int index = -1;
    int max = mProperties.size() - 1;
    while( index < max )
    {
        index++;
        if( mProperties[ index ] == property )
        {
            return index;
        }
    }

    return -1;
}
////////////////////////////////////////////////////////////////////////////////
int PropertyParser::_getPropertyIndexByName( const std::string name ) const
{
    // Get the index of this name in mPropertyNames. The corresponding Property will
    // have the same index in mProperties.
    int index = -1;
    int max = mPropertyNames.size() - 1;
    while( index < max )
    {
        index++;
        if( mPropertyNames[ index ] == name )
        {
            return index;
        }
    }
    
    return -1;
}
////////////////////////////////////////////////////////////////////////////////
int PropertyParser::_getItemIndex( const QtProperty* item ) const
{
    // Get the index of this property in mItems. The corresponding Property will
    // have the same index in mProperties.
    int index = -1;
    int max = mItems.size() - 1;
    while( index < max )
    {
        index++;
        if( mItems[ index ] == item )
        {
            return index;
        }
    }

    return -1;
}
////////////////////////////////////////////////////////////////////////////////
void PropertyParser::_setItemValue( QtProperty* const item, PropertyPtr property )
{
    //PS_LOG_TRACE( "_setItemValue" );
    //boost::any value = property->GetValue();

    if( property->IsBool() )
    {
        bool castValue = property->extract<bool>();
        PS_LOG_TRACE( "_setItemValue: " << castValue );
        mBooleanManager->setValue( item, castValue );
    }
    else if( property->IsEnum() )
    {
        int castValue = property->GetAttribute( "enumCurrentIndex" )->
                extract<int>();
        PS_LOG_TRACE( "_setItemValue: " << castValue );
        mEnumManager->setValue( item, castValue );
    }
    else if( property->IsInt() )
    {
        int castValue = property->extract<int>();
        PS_LOG_TRACE( "_setItemValue: " << castValue );
        mIntManager->setValue( item, castValue );
    }
    else if( property->IsFloat() )
    {
        double castValue = static_cast < double > ( property->extract<float>() );
        PS_LOG_TRACE( "_setItemValue: " << castValue );
        mDoubleManager->setValue( item, castValue );
    }
    else if( property->IsDouble() )
    {
        double castValue = _getUnitConvertedValue( property );
        PS_LOG_TRACE( "_setItemValue: " << castValue );
        mDoubleManager->setValue( item, castValue );
    }
    else if( property->IsString() )
    {
        std::string castValue = property->extract<std::string>();
        QString qCastValue = QString::fromStdString( castValue );
        bool found = false;
        std::map< std::string, ExternalStringSelectManager* >::iterator itr =
                m_customStringManagers.begin();
        while( itr != m_customStringManagers.end() )
        {
            std::string str = itr->first;
            if( (property->AttributeExists( str )) &&
                    property->GetAttribute( str )->extract<bool>() )
            {
                PS_LOG_TRACE( "_setItemValue: (" << str << ") " << castValue );
                found = true;
                itr->second->setValue( item, qCastValue );
            }
            ++itr;
        }
        if( !found )
        {
            PS_LOG_TRACE( "_setItemValue: " << castValue );
            mStringManager->setValue( item, qCastValue );
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
double PropertyParser::_getUnitConvertedValue( PropertyPtr property, bool inverse, double value )
{
    PS_LOG_TRACE( "_getUnitConvertedValue" );
    double storedValue = property->extract< double >();
    // If no units attached, short-circuit the conversion attempt
    if( !property->AttributeExists( "StorageUnitSystem" ) )
    {
        return storedValue;
    }


    std::string storageUnitSystem = property->GetAttribute( "StorageUnitSystem" )->extract<std::string>();
    if( m_displayUnitSystem == storageUnitSystem )
    {
        return storedValue;
    }

    // Pull the storage units
    std::string storageUnit;
    if( property->AttributeExists( storageUnitSystem + "DisplayUnit" ) )
    {
        storageUnit = property->GetAttribute( storageUnitSystem + "DisplayUnit" )->extract<std::string>();
    }
    else
    {
        return storedValue;
    }

    // Pull the storage units
    std::string displayUnit( storageUnit );
    if( property->AttributeExists( m_displayUnitSystem + "DisplayUnit" ) )
    {
        displayUnit = property->GetAttribute( m_displayUnitSystem + "DisplayUnit" )->extract<std::string>();
    }

    // Convert the value to the display units or storage units based on inverse
    // parameter
    try
    {
        if( !inverse )
        {
            storedValue = m_unitConverter.Convert( storedValue, storageUnit, displayUnit );
        }
        else
        {
            storedValue = m_unitConverter.Convert( value, displayUnit, storageUnit );
        }
    }
    catch( std::runtime_error& ex )
    {
        // Just rethrow the exception for now. Should we instead display an
        // error dialog? Perhaps display an error icon on the item?
        throw ex;
    }

    return storedValue;
}
////////////////////////////////////////////////////////////////////////////////
void PropertyParser::_setPropertyValue( QtProperty* const item, boost::any value )
{
    int index = _getItemIndex( item );
    PS_LOG_TRACE( "_setPropertyValue: index " << index << "(" << item->propertyName().toStdString() << ")"  );

    if( index > -1 )
    {
        if( mProperties[ index ]->SetValue( value ) )
        {
            // Setvalue was accepted; check for changes to other properties that
            // may occur due to links between properties in the set
            Refresh();
            if( !m_parseOperation )
            {
                Modified();
            }
        }
        else
        {
            // SetValue was denied; re-read this property's value
            _refreshItem( index );
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
void propertystore::PropertyParser::SetDisplayUnitSystem(DisplayUnitSystem system)
{
    if( SI_Units == system )
    {
        m_displayUnitSystem = "SI";
    }
    else if( US_Units == system )
    {
        m_displayUnitSystem = "US";
    }
}
////////////////////////////////////////////////////////////////////////////////
}
