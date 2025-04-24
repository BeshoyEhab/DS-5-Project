#include "datamodel.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QFileInfo>
#include <QFile>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <../assets/json.hpp>

using json = nlohmann::json;

DataModel::DataModel(){}

void DataModel::readJson(const QString& fileName) {
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Opened file:" << fileName;
    }

    if (!file.isOpen()) {
        qCritical() << fileName <<" couldn't be opened!";
        return;
    }
/*
    QByteArray jsonData = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qCritical() << "JSON Parse Error:" << parseError.errorString();
        return;
    }

    if (!doc.isObject()) {
        qCritical() << "Error: Expected JSON object!";
        return;
    }

    words.clear();
*/
    json jsonData = json::parse(file.readAll().toStdString());
    for (auto& [word, frequency] : jsonData.items()) {
        trie.insert(word, frequency);
    }

    return;
}

/*
int DataModel::getValue(const string &key){
    auto it = words.find(key);
    if(it != words.end()){
        return it->second;
    }

    auto tempIt = temp.find(key);
    if(tempIt != temp.end()){
        return tempIt->second;
    }

    return -1;
}

void DataModel::deleteWord(string key){
    words.erase(key);
}

void DataModel::addWord(string key, int frequency){
    if(words.find(key) != words.end()){
        cout << "Frequency increased" << endl;
        words[key] += frequency;
    } else {
        cout << "Added temp value" << endl;
        temp[key] += frequency; // Works whether key exists or not
    }
}
*/

// void DataModel::saveJson() {
//     QJsonObject jsonObj;
//     for (const auto& [key, value] : words) {
//         jsonObj[QString::fromStdString(key)] = value;
//     }

//     QJsonDocument jsonDoc(jsonObj);

//     QString baseDir = QCoreApplication::applicationDirPath();
//     QString assetDirPath = baseDir + "/../../assets";
//     QDir assetDir(assetDirPath);
//     if (!assetDir.exists()) {
//         if (!assetDir.mkpath(".")) {
//             qCritical() << "Failed to create directory:" << assetDirPath;
//             return;
//         }
//     }

//     QString filePath = assetDir.filePath("words.json");
//     QFile file(filePath);

//     if (file.exists()) {
//         qDebug() << "File exists. Deleting:" << filePath;
//         if (!file.remove()) {
//             qCritical() << "Failed to delete existing file:" << filePath;
//             return;
//         }
//     }

//     if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
//         qCritical() << "Error: Could not open file for writing:" << filePath;
//         return;
//     }

//     file.write(jsonDoc.toJson(QJsonDocument::Indented));
//     file.close();

//     qDebug() << "File successfully written to:" << filePath;
//     return;
// }
