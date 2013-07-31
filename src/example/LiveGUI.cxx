#include "LiveGUI.h"
#include "ui_LiveGUI.h"

#include <propertystore/MakeLive.h>
#include <switchwire/EventManager.h>
#include <switchwire/OptionalMacros.h>

#include "LiveSet.h"

#include <iostream>
#include <cmath>

#include <QtCore/QTimer>

LiveGUI::LiveGUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LiveGUI)
{
    // Set up a datamanager to test persistence
    manager = crunchstore::DataManagerPtr( new crunchstore::DataManager );
    cache = crunchstore::DataAbstractionLayerPtr( new crunchstore::NullCache );
    buffer = crunchstore::DataAbstractionLayerPtr( new crunchstore::NullBuffer );
    manager->SetCache( cache );
    manager->SetBuffer( buffer );

    // Add an SQLite store
    sqstore = crunchstore::SQLiteStorePtr( new crunchstore::SQLiteStore );
    sqstore->SetStorePath( "./LiveTest.db" );
    manager->AttachStore( sqstore, crunchstore::Store::WORKINGSTORE_ROLE );

    // Get rid of the table containing the test data so we start fresh
    manager->Drop( "Test" );

    // Create a propertyset with a live property
    ps = propertystore::PropertySetPtr( new LiveSet );
    ps->EnableLiveProperties( true );
    ps->SetDataManager( manager );
    //ps->Save();

    // Create, set up, and show a GenericPropertyBrowser which can display
    // the PropertySet
    ui->setupUi(this);
    browserDisplay = new propertystore::GenericPropertyBrowser;
    ui->verticalLayout->addWidget( browserDisplay );
    browserDisplay->ParsePropertySet( ps );
    browserDisplay->resize( 350, 350 );


    connect( ui->saveButton, SIGNAL(clicked()), this, SLOT(saveButton_clicked()) );
}

LiveGUI::~LiveGUI()
{
    delete ui;
    ps->EnableLiveProperties( false );
    manager->DetachStore( sqstore );
}

void LiveGUI::saveButton_clicked()
{
    ps->SetPropertyValue( "Double", 1.1 );
    browserDisplay->RefreshAllValues();
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(save()));
    timer->start(25);
}

void LiveGUI::save()
{
    ps->Save();   
}
