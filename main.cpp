#include "trie.h"
#include <QStyleFactory>
#include "data_model/datamodel.h"
#include <QApplication>
#include "autocompleteapp.h"
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

    QObject::connect(&window, &AutoCompleteApp::aboutToClose, [dataModel, t]() {
        dataModel->saveJson(t);
        delete dataModel;
        delete t;
    });
    return app.exec();
}
