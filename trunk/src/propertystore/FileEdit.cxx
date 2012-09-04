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
#include <propertystore/FileEdit.h>
#include <QtGui/QHBoxLayout>
#include <QtGui/QToolButton>
#include <QtGui/QFileDialog>
#include <QtGui/QFocusEvent>
#include <iostream>

namespace propertystore
{

FileEdit::FileEdit(QWidget *parent)
    : ExternalStringSelect( parent ),
      m_fileDialog( 0 )
{

}

ExternalStringSelect* FileEdit::createNew( QWidget* parent )
{
    return new FileEdit( parent );
}

void FileEdit::buttonClicked()
{
//    ves::conductor::UITabs* tabs = ves::conductor::UITabs::instance();

    if( m_fileDialog )
    {
        m_fileDialog->raise();
        //tabs->ActivateTab( m_fileDialog );
        return;
    }

    m_fileDialog = new QFileDialog( 0 );
    m_fileDialog->setOptions( QFileDialog::DontUseNativeDialog );
    m_fileDialog->setAttribute( Qt::WA_DeleteOnClose );
    m_fileDialog->setFileMode( QFileDialog::ExistingFile );

    connect( m_fileDialog, SIGNAL(fileSelected(const QString &)),
                      this, SLOT(onFileSelected(const QString&)) );
    connect( m_fileDialog, SIGNAL(rejected()), this,
                      SLOT( onFileCancelled() ) );

    m_fileDialog->show();

//    tabs->ActivateTab( tabs->AddTab( m_fileDialog, "Select File" ) );
}

void FileEdit::onFileSelected( const QString& filePath )
{
    QDir path = QDir::current();
    QString relativePath = path.relativeFilePath( filePath );
    // Now that we've pre-processed the path, let the base class handle updating
    // everything
    onExternalStringSelected( relativePath.toStdString() );

    //ves::conductor::UITabs::instance()->RemoveTab( m_fileDialog );

    if ( m_fileDialog != 0 )
    {
        m_fileDialog->close();
        m_fileDialog = 0;
    }
}

void FileEdit::onFileCancelled()
{
    //ves::conductor::UITabs::instance()->RemoveTab( m_fileDialog );

    if ( m_fileDialog != 0 )
    {
        m_fileDialog->close();
        m_fileDialog = 0;
    }
}


}
