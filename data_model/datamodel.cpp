#include "datamodel.h"
#include <iostream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QFileInfo>
#include <QFile>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <../assets/json.hpp>
#include "../trie.h"

using namespace std;

DataModel::DataModel(){}

bool DataModel::readJson(Trie* t) {
    QString baseDir = QCoreApplication::applicationDirPath();

    // قائمة بالمسارات المحتملة للملفات
    QStringList possibleAssetPaths = {
        baseDir + "/assets",                  // للمجلد الرئيسي للبناء
        baseDir + "/../assets",               // لمجلدات فرعية
        baseDir + "/../../assets",            // لهياكل مجلدات أعمق
        baseDir + "/../../../assets",         // لهياكل مجلدات أعمق جدًا
        ":/assets",                           // لمصادر Qt
        "assets"                              // للمجلد الحالي
    };

    QStringList fileNames = {"words.json", "words_dictionary.json"};

    QFile file;
    QString foundPath;

    // تجربة جميع التركيبات الممكنة للمسارات وأسماء الملفات
    for (const QString& assetPath : possibleAssetPaths) {
        for (const QString& fileName : fileNames) {
            QString fullPath = QDir(assetPath).absoluteFilePath(fileName);
            qDebug() << "محاولة فتح الملف:" << fullPath;
            file.setFileName(fullPath);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                qDebug() << "تم فتح الملف بنجاح:" << fullPath;
                foundPath = fullPath;
                break;
            }
        }
        if (file.isOpen()) break;
    }

    if (!file.isOpen()) {
        qCritical() << "تعذر فتح أي ملف قاموس! المحاولات:";
        for (const QString& assetPath : possibleAssetPaths) {
            for (const QString& fileName : fileNames) {
                qCritical() << " - " << QDir(assetPath).absoluteFilePath(fileName);
            }
        }
        return false;
    }

    // باقي كود تحليل JSON...


    QByteArray jsonData = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qCritical() << "JSON Parse Error:" << parseError.errorString();
        return false;
    }

    if (!doc.isObject()) {
        qCritical() << "Error: Expected JSON object!";
        return false;
    }

    words.clear();

    QJsonObject obj = doc.object();
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        if (it.value().isDouble())  {
            words[it.key().toStdString()] = it.value().toInt();
        } else {
            qWarning() << "Skipping key" << it.key() << "with non-integer value.";
        }
    }



    for (auto it = words.begin(); it != words.end(); ++it) {
        t->insert(it->first,it->second);
    }



    return true;
}


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

bool DataModel::saveJson() {
    QJsonObject jsonObj;
    for (const auto& [key, value] : words) {
        jsonObj[QString::fromStdString(key)] = value;
    }

    QJsonDocument jsonDoc(jsonObj);

    QString baseDir = QCoreApplication::applicationDirPath();
    QString assetDirPath = baseDir + "/../../assets";
    QDir assetDir(assetDirPath);
    if (!assetDir.exists()) {
        if (!assetDir.mkpath(".")) {
            qCritical() << "Failed to create directory:" << assetDirPath;
            return false;
        }
    }

    QString filePath = assetDir.filePath("words.json");
    QFile file(filePath);

    if (file.exists()) {
        qDebug() << "File exists. Deleting:" << filePath;
        if (!file.remove()) {
            qCritical() << "Failed to delete existing file:" << filePath;
            return false;
        }
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qCritical() << "Error: Could not open file for writing:" << filePath;
        return false;
    }

    file.write(jsonDoc.toJson(QJsonDocument::Indented));
    file.close();

    qDebug() << "File successfully written to:" << filePath;
    return true;
}
