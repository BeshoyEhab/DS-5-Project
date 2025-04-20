#include "datamodel.h"
#include <iostream>
#include <fstream>
#include "../assets/json.hpp"

using namespace std;
using json = nlohmann::json;


DataModel::DataModel() {

}

bool DataModel::readJson(){

    ifstream file("DS-5-Project/assets/words.json");

    if (!file.is_open()) {
        file.open("DS-5-Project/assets/words_dictionary.json");
        if (!file.is_open()) {
            cerr << "Neither words.json nor words_dictionary.json could be opened!" << endl;
            return false;
        }
    }

    json j;

    try {
        file >> j;
        if (!j.is_object()) {
            cerr << "Error: Expected JSON object!" << endl;
            return false;
        }

        words.clear();

        for (const auto& [key, value] : j.items()) {
            words[key] = value.get<int>();
        }
    } catch (const json::exception& e) {
        cerr << "JSON Error: " << e.what() << endl;
        return false;
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

bool DataModel::saveJson(){
    json j;

    for (const auto& [key, value] : words) {
        j[key] = value;
    }

    ifstream checkFile("DS-5-Project/assets/words.json");

        if (!checkFile.is_open()) {
            cout << "File does not exist. Creating new file: DS-5-Project/assets/words.json" << endl;

            // Open the file for writing (will create it if it doesn't exist)
            ofstream outFile("DS-5-Project/assets/words.json");

            // If file couldn't be opened
            if (!outFile.is_open()) {
                cerr << "Error: Could not open file for writing: DS-5-Project/assets/words.json" << endl;
                return false;
            }

            // Write the JSON to the file (pretty print with 4 spaces)
            outFile << j.dump(4);  // 4 for pretty printing with indentations
            outFile.close();
        } else {
            cout << "File exists. Overwriting file: DS-5-Project/assets/words.json" << endl;

            // File exists, just overwrite it with new data
            ofstream outFile("DS-5-Project/assets/words.json");

            if (!outFile.is_open()) {
                cerr << "Error: Could not open file for writing: DS-5-Project/assets/words.json" << endl;
                return false;
            }

            outFile << j.dump(4);  // Pretty print the JSON
            outFile.close();
        }
    return true;
}

