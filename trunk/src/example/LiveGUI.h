#ifndef LIVEGUI_H
#define LIVEGUI_H

#include <QWidget>

#include <propertystore/PropertySet.h>
#include <crunchstore/Persistable.h>
#include <crunchstore/Datum.h>
#include <crunchstore/DataManager.h>
#include <crunchstore/NullBuffer.h>
#include <crunchstore/NullCache.h>
#include <crunchstore/DataAbstractionLayer.h>
#include <crunchstore/SQLiteStore.h>

#include <propertystore/GenericPropertyBrowser.h>


namespace Ui {
class LiveGUI;
}

class LiveGUI : public QWidget
{
    Q_OBJECT
    
public:
    explicit LiveGUI(QWidget *parent = 0);
    ~LiveGUI();

protected Q_SLOTS:
    void saveButton_clicked();
    void save();
    
private:
    Ui::LiveGUI *ui;
    propertystore::PropertySetPtr ps;
    crunchstore::DataManagerPtr manager;
    crunchstore::DataAbstractionLayerPtr cache;
    crunchstore::DataAbstractionLayerPtr buffer;
    crunchstore::SQLiteStorePtr sqstore;
    propertystore::GenericPropertyBrowser* browserDisplay;
};

#endif // LIVEGUI_H
