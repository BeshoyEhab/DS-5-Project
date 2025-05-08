


#include<iostream>
#include<fstream>
#include<unordered_map>
#include<map>
#include<vector>
#include<string>
#include "stack"
#include "queue"
#include "trie.h"
#include "../data_model/datamodel.h"
#include <QApplication>
#include "../headers/autocompleteapp.h"
#include "../data_model/datamodel.h"
//#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    DataModel *dataModel = new DataModel();
    Trie* t =new Trie() ;
    dataModel->readJson(t);


    AutoCompleteApp window(t);
    window.show();
    window.setFocus();
    dataModel->saveJson();
    return app.exec();
}


// int main()
// {
//     Trie* t =new Trie() ;
//     DataModel *dataModel = new DataModel();
//     dataModel->readJson(t);
//     t->printSuggestions("ca");



//     map<int, string>::iterator it;

//     for (it = t->mab.begin(); it != t->mab.end(); it++)
//     {
//         cout << it->first << "  " << it->second << endl;
//     }

//     cout << endl;

//     //DFS

//     while (!t->q.empty())
//     {
//         cout << t->q.front() << "  ";
//         t->q.pop();
//     }
//     cout << endl<<endl;

//     //freq


//     //BFS
//     for (it = t->m.begin(); it != t->m.end(); it++)
//     {
//         cout << it->first << "  " << it->second << endl;
//     }









//     return 0;
// }


// #include <QApplication>

// int main(int argc, char *argv[])
// {
//     QApplication a(argc, argv);  // الإنشاء أولاً

//     Trie* t = new Trie();
//     DataModel *dataModel = new DataModel();
//     if (!dataModel->readJson(t)) {
//         qCritical() << "فشل تحميل القاموس!";
//         return 1;
//     }
//         t->printSuggestions("catego");



//         map<int, string>::iterator it;

//         //freq
//         for (it = t->mab.begin(); it != t->mab.end(); it++)
//         {
//             cout << it->first << "  " << it->second << endl;
//         }

//         // cout << endl;

//         // //DFS

//         // while (!t->q.empty())
//         // {
//         //     cout << t->q.front() << "  ";
//         //     t->q.pop();
//         // }
//         // cout << endl<<endl;

//         // //freq


//         // //BFS
//         // for (it = t->m.begin(); it != t->m.end(); it++)
//         // {
//         //     cout << it->first << "  " << it->second << endl;
//         // }









//         return 0;

// }
