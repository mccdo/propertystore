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

/** This class is a slightly altered version of the FileEdit class shown in
 Qt Quarterly at
http://doc.qt.nokia.com/qq/qq18-propertybrowser.html#extendingtheframework
It is being used in accordance with the terms of LGPL **/

#include <qtpropertybrowser.h>
#include <QtCore/QMap>
#include <propertystore/Exports.h>

/// @file ExternalStringSelectManager.h
/// @namespace propertystore
/// @class ExternalStringSelectManager is the generic manager for any editor
/// widgets derived from ExternalStringSelect.

namespace propertystore
{

class PROPERTYSTORE_EXPORT ExternalStringSelectManager : public QtAbstractPropertyManager
{
    Q_OBJECT
public:
    ExternalStringSelectManager(QObject* parent = 0)
        : QtAbstractPropertyManager(parent)
    {  }

    QString value(const QtProperty* property) const;
    virtual QIcon valueIcon(const QtProperty* property) const;

public Q_SLOTS:
    void setValue(QtProperty* property, const QString& val);
Q_SIGNALS:
    void valueChanged(QtProperty* property, const QString& val);
protected:
    virtual QString valueText(const QtProperty* property) const { return value(property); }
    virtual void initializeProperty(QtProperty* property) { theValues[property] = Data(); }
    virtual void uninitializeProperty(QtProperty* property) { theValues.remove(property); }
private:

    struct Data
    {
        QString value;
    };

    QMap<const QtProperty*, Data> theValues;
};

}

