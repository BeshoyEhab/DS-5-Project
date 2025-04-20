#include <QApplication>
#include "../headers/autocompleteapp.h"
#include "../data_model/datamodel.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    AutoCompleteApp window;
    DataModel *dataModel = new DataModel();
    dataModel->readJson();
    cout<<dataModel->getValue("unvociferously")<<endl;
    dataModel->addWord("unvociferously");
    dataModel->addWord("unvociferously");
    dataModel->addWord("unvociferously");
    cout<<dataModel->getValue("unvociferously")<<endl;
    window.show();
    window.setFocus();
    dataModel->saveJson();
    return app.exec();
}
