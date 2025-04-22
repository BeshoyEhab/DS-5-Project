#include <QApplication>
#include "../headers/autocompleteapp.h"
#include "../data_model/datamodel.h"
//#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    AutoCompleteApp window;
    DataModel *dataModel = new DataModel();
    dataModel->readJson();
    window.show();
    window.setFocus();
    dataModel->saveJson();
    return app.exec();
}
