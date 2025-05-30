// Copyright 2025 Justin Greisiger Frost
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <memory>

// Include the header for our Fact system
#include "myrmidon/base_fact.hpp"

// Use the namespace for convenience
using namespace Myrmidon;

// Define a simple struct to demonstrate RefFact
struct MyDataObject {
    int id;
    std::string description;
    bool active;
};

int main() {
    std::cout << "--- Myrmidon Rule Engine Fact Demonstration ---" << std::endl;

    // 1. Create a ListFact (vector of strings)
    std::vector<std::string> userList = {"Alice", "Bob", "Charlie"};
    ListFact<std::string> users("active_users", userList);
    std::cout << "\nCreated Fact: '" << users.getName() << "' (ListFact<string>)" << std::endl;
    std::cout << "  Values: ";
    for (const auto& user : users.getValues()) {
        std::cout << user << " ";
    }
    std::cout << std::endl;

    // 2. Create a MapFact (map string to int)
    std::map<std::string, int> itemCounts = {{"widget", 10}, {"gadget", 5}};
    MapFact<std::string, int> inventory("item_counts", itemCounts);
    std::cout << "\nCreated Fact: '" << inventory.getName() << "' (MapFact<string, int>)" << std::endl;
    std::cout << "  Values: ";
    for (const auto& pair : inventory.getValues()) {
        std::cout << "{" << pair.first << ": " << pair.second << "} ";
    }
    std::cout << std::endl;

    // 3. Create a SetFact (unordered_set of integers)
    std::unordered_set<int> processedIds = {101, 205, 300, 101}; // Note: 101 duplicate ignored
    SetFact<int> processed("processed_ids", processedIds);
    std::cout << "\nCreated Fact: '" << processed.getName() << "' (SetFact<int>)" << std::endl;
    std::cout << "  Values: ";
    for (const auto& id : processed.getValues()) {
        std::cout << id << " ";
    }
    std::cout << std::endl;

    // 4. Create a ValueFact (single double and single bool)
    ValueFact<double> temperature("current_temp", {25.5});
    ValueFact<bool> systemStatus("is_online", {true});
    std::cout << "\nCreated Fact: '" << temperature.getName() << "' (ValueFact<double>)" << std::endl;
    if (!temperature.getValues().empty()) {
        std::cout << "  Value: " << temperature.getValues()[0] << std::endl;
    }
    std::cout << "Created Fact: '" << systemStatus.getName() << "' (ValueFact<bool>)" << std::endl;
    if (!systemStatus.getValues().empty()) {
        std::cout << "  Value: " << (systemStatus.getValues()[0] ? "true" : "false") << std::endl;
    }

    // 5. Create a RefFact (pointer to a local object)
    MyDataObject dataObject = { 999, "Sensor Data", true };
    RefFact<MyDataObject> dataRef("sensor_data_ref", &dataObject);
    std::cout << "\nCreated Fact: '" << dataRef.getName() << "' (RefFact<MyDataObject>)" << std::endl;
    MyDataObject* pData = dataRef.getValues();
    if (pData) {
        std::cout << "  Ref points to object with ID: " << pData->id
                  << ", Description: '" << pData->description << "'"
                  << ", Active: " << (pData->active ? "true" : "false") << std::endl;
    } else {
        std::cout << "  Ref points to null." << std::endl;
    }

    dataObject.active = false;
    std::cout << "  Original object modified. RefFact now points to object with Active: "
              << (pData ? (pData->active ? "true" : "false") : "null") << std::endl;


    std::cout << "\n--- End of Demonstration ---" << std::endl;

    return 0;
}
