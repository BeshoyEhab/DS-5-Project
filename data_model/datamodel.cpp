#include "datamodel.h"
#include <iostream>
#include <fstream>
#include "../assets/json.hpp"

using namespace std;
using json = nlohmann::json;


DataModel::DataModel() {

}

bool DataModel::readJson(){

    std::ifstream file("DS-5-Project/assets/words.json");

    if (!file.is_open()) {
        file.open("DS-5-Project/assets/words_dictionary.json");
        if (!file.is_open()) {
            std::cerr << "Neither words.json nor words_dictionary.json could be opened!" << std::endl;
            return false;
        }
    }

    json j;

    try {
        file >> j;
        if (!j.is_object()) {
            std::cerr << "Error: Expected JSON object!" << std::endl;
            return false;
        }

        words.clear();

        for (const auto& [key, value] : j.items()) {
            words[key] = value.get<int>();
        }
    } catch (const json::exception& e) {
        std::cerr << "JSON Error: " << e.what() << std::endl;
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

void DataModel::addWord(std::string key, int frequency){
    if(words.find(key) != words.end()){
        std::cout << "Frequency increased" << std::endl;
        words[key] += frequency;
    } else {
        std::cout << "Added temp value" << std::endl;
        temp[key] += frequency; // Works whether key exists or not
    }
}

