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
#include <QtGui/QApplication>

#include <propertystore/GenericPropertyBrowser.h>
#include <propertystore/PropertySet.h>
#include <propertystore/MakeLive.h>
#include <switchwire/EventManager.h>
#include <switchwire/OptionalMacros.h>

// logging facilities.
#include <Poco/Logger.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/PatternFormatter.h>
#include <Poco/FormattingChannel.h>

#include "LiveGUI.h"


int main(int argc, char *argv[])
{
    // Set up a logger so we can see log messages from the propertystore library
    Poco::Logger::root().setLevel("debug");
    Poco::ConsoleChannel* console = new Poco::ConsoleChannel;
    Poco::PatternFormatter* formatter = new Poco::PatternFormatter;
    formatter->setProperty("pattern", "[%I]%s: %t");
    Poco::FormattingChannel* formattingChannel =
            new Poco::FormattingChannel( formatter, console );
    Poco::Logger::root().setChannel( formattingChannel );


    QApplication a(argc, argv);
    LiveGUI gui;
    gui.show();

    return a.exec();
}

