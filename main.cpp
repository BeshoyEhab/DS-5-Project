#include <QApplication>
#include "../headers/autocompleteapp.h"
#include "../data_model/datamodel.h";
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    AutoCompleteApp window;
    DataModel *dataModel = new DataModel();
    dataModel->readJson("/home/unknown/Desktop/Qt/DS-5-Project/assets/words_dictionary.json");
    cout<<dataModel->getValue("unvociferously")<<endl;
    window.show();
    window.setFocus();
    return app.exec();
}
