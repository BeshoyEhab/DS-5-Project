#include "trie.h"
#include <QStyleFactory>
#include "data_model/datamodel.h"
#include <QApplication>
#include "../headers/autocompleteapp.h"
#include "data_model/datamodel.h"

using namespace std;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setStyle(QStyleFactory::create("Fusion"));
    DataModel *dataModel = new DataModel();
    Trie* t = new Trie();
    dataModel->readJson(t);

    AutoCompleteApp window(t);
    window.show();

    // ربط إشارة الإغلاق بدالة الحفظ
    QObject::connect(&window, &AutoCompleteApp::aboutToClose, [dataModel, t]() {
        dataModel->saveJson(t);  // حفظ البيانات
        delete dataModel;        // تحرير الذاكرة
        delete t;
    });
    return app.exec();
}
