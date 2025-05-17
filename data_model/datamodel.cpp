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
#include "../trie.h"

using namespace std;

DataModel::DataModel(){}

bool DataModel::readJson(Trie* trie) {
    QString baseDir = QCoreApplication::applicationDirPath();

    QStringList possibleAssetPaths = {
        baseDir + "/assets",
        baseDir + "/../assets",
        baseDir + "/../../assets",
        baseDir + "/../../../assets",
        ":/assets",
        "assets"
    };

    QStringList fileNames = {"words.json", "words_dictionary.json"};

    QFile file;
    QString foundPath;

    for (const QString& assetPath : possibleAssetPaths) {
        for (const QString& fileName : fileNames) {
            QString fullPath = QDir(assetPath).absoluteFilePath(fileName);
            qDebug() << "try to open file: " << fullPath;
            file.setFileName(fullPath);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                qDebug() << "File open succesfully" << fullPath;
                foundPath = fullPath;
                break;
            }
        }
        if (file.isOpen()) break;
    }

    if (!file.isOpen()) {
        qCritical() << "Can't open any file";
        for (const QString& assetPath : possibleAssetPaths) {
            for (const QString& fileName : fileNames) {
                qCritical() << " - " << QDir(assetPath).absoluteFilePath(fileName);
            }
        }
        return false;
    }


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

    QJsonObject obj = doc.object();
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        if (it.value().isDouble())  {
            trie->insert(it.key().toStdString(), it.value().toInt());
        } else {
            qWarning() << "Skipping key" << it.key() << "with non-integer value.";
        }
    }
    return true;
}


bool DataModel::saveJson(Trie* trie) {
    QString baseDir = QCoreApplication::applicationDirPath();
    QString assetDirPath = baseDir + "/../../assets";
    QDir assetDir(assetDirPath);

    if (!assetDir.exists() && !assetDir.mkpath(".")) {
        qCritical() << "Failed to create directory:" << assetDirPath;
        return false;
    }

    QString tempFilePath = assetDir.filePath("words_temp.json");
    QFile tempFile(tempFilePath);

    if (!tempFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qCritical() << "Error: Could not open temp file for writing:" << tempFilePath;
        return false;
    }

    tempFile.write("{\n");

    int counter = 0;
    bool firstEntry = true;
    for (const auto& [key, value] : trie->allwards) {
        if (!firstEntry) {
            tempFile.write(",\n");
        }
        firstEntry = false;

        QString entry = QString("\"%1\": %2").arg(QString::fromStdString(key)).arg(value);
        tempFile.write(entry.toUtf8());

        counter++;
        if (counter % 10000 == 0) {
            QCoreApplication::processEvents();
        }
    }

    tempFile.write("\n}");
    tempFile.close();

    QString finalFilePath = assetDir.filePath("words.json");
    if (QFile::exists(finalFilePath) && !QFile::remove(finalFilePath)) {
        qCritical() << "Failed to remove old file";
        return false;
    }

    if (!tempFile.rename(finalFilePath)) {
        qCritical() << "Failed to rename temp file";
        return false;
    }

    return true;
}
