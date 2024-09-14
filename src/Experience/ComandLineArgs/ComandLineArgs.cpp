// ComandLineArgs.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
using namespace std;
using namespace nlohmann;

struct Data {
    string name;
    int age;
};

void exportDataToJson(const string& filePath, const vector<Data>& data) {
    ofstream outfile(filePath);
    if (outfile.is_open()) {
        json j;
        j["data"] = data;
        outfile << j << std::endl;
        outfile.close();
        cout << "Data succeed export to " << filePath << endl;
    }
    else {
        cerr << "Can't open file " << filePath << endl;
    }
}

vector<Data> importDataFromJson(const string& filePath) {
    vector<Data> data;
    ifstream infile(filePath);
    if (infile.is_open()) {
        json j;
        infile >> j;
        for (auto& entry : j["data"]) {
            Data d;
            d.name = entry["name"];
            d.age = entry["age"];
            data.push_back(d);
        }
        infile.close();
        cout << "data imported from " << filePath << endl;
    }
    else {
        cerr << "can't open file " << filePath << endl;
    }
    return data;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "please provide command line arguments." << endl;
        return 1;
    }

    string command = argv[1];

    if (command == "/export") {
        if (argc < 3) {
            cerr << "please provide file path" << endl;
            return 1;
        }

        string filePath = argv[2];

        vector<Data> data = {
          {"Alice", 30},
          {"Bob", 25},
        };

        exportDataToJson(filePath, data);

    }
    else if (command == "/import") {
        if (argc < 3) {
            cerr << "please provide file path." << endl;
            return 1;
        }

        string filePath = argv[2];

        vector<Data> data = importDataFromJson(filePath);

        for (const Data& d : data) {
            cout << "name：" << d.name << ", age：" << d.age << endl;
        }

    }
    else {
        cerr << "invalid command：" << command << endl;
        return 1;
    }

    return 0;
}