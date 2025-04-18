#include <QApplication>
#include "autocompleteapp.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    AutoCompleteApp window;
    window.show();
    return app.exec();
}
