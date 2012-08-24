#include "TestSet.h"

TestSet::TestSet()
{
    SetTypeName( "CAD" );

    AddProperty( "Visible", true, "Visible" );

    AddProperty( "SurfaceData", false, "Use as surface data" );

    AddProperty( "Transform", boost::any(), "Transform" );
    SetPropertyAttribute( "Transform", "isUIGroupOnly", true );
    SetPropertyAttribute( "Transform", "setExpanded", false );

    AddProperty( "Transform_Translation", boost::any(), "Translation" );
    SetPropertyAttribute( "Transform_Translation", "isUIGroupOnly", true );

    AddProperty( "Transform_Translation_X", 0.0, "x" );
    AddProperty( "Transform_Translation_Y", 0.0, "y" );
    AddProperty( "Transform_Translation_Z", 0.0, "z" );
    SetPropertyAttribute( "Transform_Translation_X", "DisplayPrecision", 4 );
    SetPropertyAttribute( "Transform_Translation_Y", "DisplayPrecision", 4 );
    SetPropertyAttribute( "Transform_Translation_Z", "DisplayPrecision", 4 );

    AddProperty( "Transform_Rotation", boost::any(), "Rotation" );
    SetPropertyAttribute( "Transform_Rotation", "isUIGroupOnly", true );

    AddProperty( "Transform_Rotation_X", 0.0, "x" );
    AddProperty( "Transform_Rotation_Y", 0.0, "y" );
    AddProperty( "Transform_Rotation_Z", 0.0, "z" );

    AddProperty( "Transform_Scale", boost::any(), "Scale" );
    SetPropertyAttribute( "Transform_Scale", "isUIGroupOnly", true );

    AddProperty( "Transform_Scale_Uniform", true, "Uniform Scaling" );

    AddProperty( "Transform_Scale_X", 1.0, "x" );
    AddProperty( "Transform_Scale_Y", 1.0, "y" );
    AddProperty( "Transform_Scale_Z", 1.0, "z" );
    SetPropertyAttribute( "Transform_Scale_X", "DisplayPrecision", 6 );
    SetPropertyAttribute( "Transform_Scale_Y", "DisplayPrecision", 6 );
    SetPropertyAttribute( "Transform_Scale_Z", "DisplayPrecision", 6 );

    AddProperty( "Physics", boost::any(), "Physics" );
    SetPropertyAttribute( "Physics", "isUIGroupOnly", true );
    SetPropertyAttribute( "Physics", "setExpanded", false );

    AddProperty( "Physics_Mass", 1.00, "Mass" );
    AddProperty( "Physics_Friction", 0.00, "Coeff. of Friction" );
    AddProperty( "Physics_Restitution", 0.00, "Coeff. of Restitution" );

    PSVectorOfStrings enumValues;

    AddProperty( "Physics_MotionType", std::string("None"), "Motion Type" );
    enumValues.push_back( "None" );
    enumValues.push_back( "Static" );
    enumValues.push_back( "Dynamic" );
    SetPropertyAttribute( "Physics_MotionType", "enumValues", enumValues );

    AddProperty( "Physics_LODType", std::string("None"), "LOD Type" );
    enumValues.clear();
    enumValues.push_back( "None" );
    enumValues.push_back( "Overall" );
    enumValues.push_back( "Compound" );
    SetPropertyAttribute( "Physics_LODType", "enumValues", enumValues );

    AddProperty( "Physics_MeshType", std::string("None"), "Mesh Type" );
    enumValues.clear();
    enumValues.push_back( "None" );
    enumValues.push_back( "Box" );
    enumValues.push_back( "Sphere" );
    enumValues.push_back( "Cylinder" );
    enumValues.push_back( "Mesh" );
    SetPropertyAttribute( "Physics_MeshType", "enumValues", enumValues );

    AddProperty( "Physics_MeshDecimation", std::string("Exact"), "Mesh Decimation" );
    enumValues.clear();
    enumValues.push_back( "Exact" );
    enumValues.push_back( "Low" );
    enumValues.push_back( "Medium" );
    enumValues.push_back( "High" );
    SetPropertyAttribute( "Physics_MeshDecimation", "enumValues", enumValues );

    AddProperty( "Physics_Enable", false, "Enable Physics" );

    AddProperty( "Culling", std::string("Off"), "Occlusion Culling" );
    enumValues.clear();
    enumValues.push_back( "Off" );
    enumValues.push_back( "Low" );
    enumValues.push_back( "Medium" );
    enumValues.push_back( "High" );
    SetPropertyAttribute( "Culling", "enumValues", enumValues );

    AddProperty( "Opacity", 1.0, "Opacity" );
    SetPropertyAttribute( "Opacity", "minimumValue", 0.0 );
    SetPropertyAttribute( "Opacity", "maximumValue", 1.0 );


    AddProperty( "TransparencyFlag", false, "Make translucent when viz is active" );

    AddProperty( "GPS", false, "Global Positioning" );
    AddProperty( "GPS_Longitude", 0.0 );
    AddProperty( "GPS_Latitude", 0.0 );

    std::string emptyString;
    AddProperty( "NodePath", emptyString, "Not visible in UI" );
    SetPropertyAttribute( "NodePath", "userVisible", false );

    AddProperty( "DynamicAnalysisData", std::string( "null" ), "Multi-body Dynamics Data" );
    SetPropertyAttribute( "DynamicAnalysisData", "isFilePath", true );

    AddProperty( "Filename", emptyString, "Filename: not visible in UI" );
    SetPropertyAttribute( "Filename", "userVisible", false );

    AddProperty( "Audio", boost::any(), "Audio" );
    SetPropertyAttribute( "Audio", "isUIGroupOnly", true );
    SetPropertyAttribute( "Audio", "setExpanded", false );
    AddProperty( "Audio_SoundFile", std::string(""), "Sound File" );
    SetPropertyAttribute( "Audio_SoundFile", "isFilePath", true );
    AddProperty( "Audio_Level", 5, "Level" );
    SetPropertyAttribute( "Audio_Level", "minimumValue", 0 );
    SetPropertyAttribute( "Audio_Level", "maximumValue", 10 );
}
