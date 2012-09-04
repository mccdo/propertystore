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

#include <QtGui/QLineEdit>
#include <QtGui/QFileDialog>

#include <propertystore/ExternalStringSelect.h>

/// @file FileEdit.h
/// @namespace propertystore
/// @class FileEdit is an editor widget consisting of a textedit and associated
/// button. When clicked, the button brings up a QFileDialog so the user may
/// select a file. Once chosen, the file path is converted to a relative path
/// (relative to the present working directory) and is placed into the textedit.
namespace propertystore
{

class PROPERTYSTORE_EXPORT FileEdit : public ExternalStringSelect
{
    Q_OBJECT
public:
    FileEdit(QWidget *parent = 0);
    virtual ExternalStringSelect* createNew( QWidget* parent );
Q_SIGNALS:
    //void filePathChanged(const QString &filePath);

public Q_SLOTS:
    virtual void buttonClicked();
    virtual void onFileSelected( const QString& filePath );
    virtual void onFileCancelled();

private:
    QFileDialog* m_fileDialog;
};

}

