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

    QStringList jsonFileNames = {"words_dictionary.json"};
    QStringList txtFileNames = {"Dictionary.txt", "words.txt"};

    QFile file;
    QString foundPath;
    bool convertedFromTxt = false;

    // First try to find and open JSON file
    for (const QString& assetPath : possibleAssetPaths) {
        for (const QString& fileName : jsonFileNames) {
            QString fullPath = QDir(assetPath).absoluteFilePath(fileName);
            qDebug() << "Trying JSON file:" << fullPath;
            file.setFileName(fullPath);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                qDebug() << "Opened JSON file:" << fullPath;
                foundPath = fullPath;
                break;
            }
        }
        if (file.isOpen()) break;
    }

    // If JSON not found, try to find and convert TXT file
    if (!file.isOpen()) {
        qDebug() << "No JSON found, searching for TXT files...";
        QString foundTxtPath;

        // Search for TXT files
        for (const QString& assetPath : possibleAssetPaths) {
            for (const QString& fileName : txtFileNames) {
                QString fullPath = QDir(assetPath).absoluteFilePath(fileName);
                qDebug() << "Checking for TXT file:" << fullPath;
                if (QFile::exists(fullPath)) {
                    foundTxtPath = fullPath;
                    qDebug() << "Found TXT file at:" << foundTxtPath;

                    // Convert TXT to JSON
                    QString jsonPath = QDir(assetPath).absoluteFilePath(jsonFileNames.first());
                    if (convertTxtToJson(foundTxtPath, jsonPath)) {
                        qDebug() << "Successfully converted TXT to JSON";
                        file.setFileName(jsonPath);
                        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                            foundPath = jsonPath;
                            convertedFromTxt = true;
                            break;
                        }
                    }
                }
                if (!foundTxtPath.isEmpty()) break;
            }
            if (convertedFromTxt) break;
        }
    }

    if (!file.isOpen()) {
        qCritical() << "Failed to find or convert any data files";
        for (const QString& assetPath : possibleAssetPaths) {
            qCritical() << "Searched in:" << assetPath;
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

    if (convertedFromTxt) {
        qDebug() << "Successfully loaded data from converted TXT file";
    } else {
        qDebug() << "Successfully loaded data from existing JSON file";
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

bool DataModel::convertTxtToJson(const QString& txtFilePath, const QString& jsonFilePath) {
    QFile inputFile(txtFilePath);
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "Failed to open input file:" << txtFilePath;
        return false;
    }

    QFileInfo outputInfo(jsonFilePath);
    QDir outputDir = outputInfo.absoluteDir();
    if (!outputDir.exists() && !outputDir.mkpath(".")) {
        qCritical() << "Failed to create output directory:" << outputDir.path();
        inputFile.close();
        return false;
    }

    QFile outputFile(jsonFilePath);
    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qCritical() << "Failed to open output file:" << jsonFilePath;
        inputFile.close();
        return false;
    }

    QTextStream in(&inputFile);
    QTextStream out(&outputFile);

    out << "{\n";

    bool firstLine = true;
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList parts = line.split(' ', Qt::SkipEmptyParts);
        if (parts.size() < 2) {
            qWarning() << "Skipping malformed line:" << line;
            continue;
        }

        QString word = parts[0];
        QString valueStr = parts[1];
        bool ok;
        int value = valueStr.toInt(&ok);

        if (!ok) {
            qWarning() << "Skipping line with invalid number:" << line;
            continue;
        }

        if (!firstLine) {
            out << ",\n";
        }
        firstLine = false;

        out << "    \"" << word << "\": " << value;
    }

    out << "\n}\n";

    inputFile.close();
    outputFile.close();

    qDebug() << "Successfully converted" << txtFilePath << "to" << jsonFilePath;
    return true;
}
