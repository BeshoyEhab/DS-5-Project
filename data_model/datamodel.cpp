#include "datamodel.h"
#include <iostream>
#include <fstream>
#include "../assets/json.hpp"

using namespace std;
using json = nlohmann::json;


DataModel::DataModel() {

}

bool DataModel::readJson(const string &fileName){
    ifstream file(fileName);
    if(!file.is_open()){
        cerr<<fileName<<"failed to open!!"<<endl;
        return false;
    }

    json j;
    try{
        file >> j;
        if(!j.is_object()){
            cerr<<"Error: Expected JSON File"<<endl;
        }

        words.clear();

        for(const auto& [key, value] : j.items()){
             words[key] = value.get<int>();
        }

    } catch (const json::exception &e){
        cerr<<"Error Occurred: "<<e.what()<<endl;
        return false;
    }

    return true;
}

int DataModel::getValue(const string &key){
    auto it = words.find(key);
    if(it != words.end()){
        return it ->second;
    }

    return -1;
}
