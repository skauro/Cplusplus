#include "Main.h"
#include <tuple>
#include <string>
#include <optional>
#include <variant>
#include <map>
#include <utility>

using namespace std;

Data::Data(const Data& other) {
    // Clear existing content
    for (auto& outerPair : DataStructure) {
        for (auto& innerPair : outerPair.second) {
            delete innerPair.second;
        }
    }
    DataStructure.clear();

    // Deep copy of the DataStructure
    for (const auto& outerPair : other.DataStructure) {
        for (const auto& innerPair : outerPair.second) {
            DataStructure[outerPair.first][innerPair.first] = new std::vector<Item>(*(innerPair.second));
        }
    }
}


int Data::CountItems() {
    size_t itemCount = 0;

    for (const auto& outerPair : DataStructure) {
        for (const auto& innerPair : outerPair.second) {
            itemCount += innerPair.second->size();  // Add the size of each vector to the total count
        }
    }

    return static_cast<int>(itemCount);
}

int Data::CountGroupItems(char c) {
    // Get the pointer to the group
    std::map<int, std::vector<Item>*>* group = GetGroup(c);

    if (group) {
        // Counter for the number of items in the group
        int groupItemCount = 0;

        // Iterate through the inner map and count items
        for (const auto& innerPair : *group) {
            for (const auto& item : *innerPair.second) {
                ++groupItemCount;
            }
        }

        return groupItemCount;
    }
     return 0;
}

std::vector<Item>* Data::GetSubgroup(char c, int i) {
    auto outerIt = DataStructure.find(c);

    if (outerIt != DataStructure.end()) {
        auto& innerMap = outerIt->second;
        auto innerIt = innerMap.find(i);

        if (innerIt != innerMap.end()) {
            return innerIt->second;
        }
    }
    return nullptr;  // Group or subgroup not found
}

void Data::PrintSubgroup(char c, int i)
{
    const auto subgroup = GetSubgroup(c, i);

    if (subgroup) {
        for (const auto& item : *subgroup) {
            item.ShowAll();
        }
    }
    else {
        std::cout << "Group or subgroup not found.\n" << std::endl; 
    }
}

int Data::CountSubgroupItems(char c, int i)
{
    const auto subgroup = GetSubgroup(c, i);

    if (subgroup) {
        return static_cast<int>(subgroup->size());
    }
    return 0;
}

std::optional<Item> Data::GetItem(char c, int i, std::string s) {
    auto outerIt = DataStructure.find(c);

    if (outerIt != DataStructure.end()) {
        auto& innerMap = outerIt->second;
        auto innerIt = innerMap.find(i);

        if (innerIt != innerMap.end()) {
            // Find an item with the specified name in the subgroup
            auto itemIt = std::find_if(innerIt->second->begin(), innerIt->second->end(),
                [&s](const Item& item) { return item.GetName() == s; });

            if (itemIt != innerIt->second->end()) {
                // Return the found item wrapped in std::optional
                return std::optional<Item>(*itemIt);
            }
        }
    }
    // Group, subgroup, or item not found
    return std::nullopt;
}

// Implementation of PrintItem
void Data::PrintItem(char c, int i, std::string s) {
    auto item = GetItem(c, i, s);

    if (item) {
        item.value().ShowAll(); 
    }
    else {
        std::cout << "Item not found.\n" << std::endl;
    }
}

optional<Item> Data::InsertItem(char c, int i, string s, optional<variant<Date, Time>> v) {  
    
    if (s.empty()) {
        return nullopt;
    }
    if (!isalpha(c))
    {
        return nullopt;
    }
    if (GetItem(c, i, s) != nullopt)
    {
        return nullopt;
    }
    
    auto outerIt = DataStructure.find(c);

    if (outerIt == DataStructure.end()) {
        // Create the missing group
        DataStructure.emplace(c, std::map<int, std::vector<Item>*>());

    }

    auto& innerMap = DataStructure[c];

    // Check if the subgroup does not exist
    auto innerIt = innerMap.find(i);
    if (innerIt == innerMap.end()) {
        // Subgroup does not exist
        innerMap.emplace(i, new std::vector<Item>());
    }
    Item *newItem = new Item(c, i, s, Date::CreateRandomDate(Date(1, 1, 2000), Date(31, 12, 2022)));
    if (v.has_value())
    {
        const auto& variantValue = v.value();
        if (std::holds_alternative<Date>(variantValue))
        {
            const Date& dateValue = std::get<Date>(variantValue);
            delete newItem;
            newItem = new Item(c, i, s, dateValue);  
        }
        else if (std::holds_alternative<Time>(variantValue))
        {
            const Time& timeValue = std::get<Time>(variantValue);
            delete newItem;
            newItem = new Item(c, i, s, timeValue);
        }
    }
    // Use Date::CreateRandomDate() if Timestamp is not specified
    
    // Insert the new item into the subgroup
    innerMap[i]->push_back(*newItem);

    // Return the inserted item wrapped in std::optional
    return (*newItem);
}

vector<Item>* Data::InsertSubgroup(char c, int i, initializer_list<tuple<string, optional<variant<Date, Time>>>> items) {

    if (!isalpha(c))
    {
        return nullptr;
    }
    
    auto outerIt = DataStructure.find(c);

    if (outerIt == DataStructure.end()) {
        // Group does not exist  
        DataStructure.emplace(c, map<int, vector<Item>*>());
    }
    // Create the missing group

    auto& innerMap = DataStructure.at(c);
    // Check if the subgroup already exists
    auto innerIt = innerMap.find(i);
    if (innerIt != innerMap.end()) {
        // Subgroup already exists, return nullptr
        return nullptr;
    }
    innerMap.emplace(i, new std::vector<Item>());                 //siin tegin midagi
        // Create a new subgroup with the provided items
    auto& newSubgroup = innerMap[i];
    for (const auto& item : items) {
        std::string itemName = std::get<0>(item);
        std::optional<std::variant<Date, Time>> timestamp = std::get<1>(item);

        // Check if the name is not an empty string
        if (itemName.empty()) {
            return nullptr;
        }

        Date randomDate = Date::CreateRandomDate(Date(1, 1, 2000), Date(31, 12, 2022));
        Item *newItem = new Item(c, i, itemName, timestamp.value_or(randomDate));
        newSubgroup->push_back(*newItem);
    }

    // Return the raw pointer to the inserted subgroup
    return innerMap[i];

}

std::map<int, std::vector<Item>*>* Data::InsertGroup(char c, std::initializer_list<int> subgroups,
    std::initializer_list<std::initializer_list<std::tuple<std::string, std::optional<std::variant<Date, Time>>>>> items)
{
    if (!isalpha(c))
    {
        return nullptr;

    }
    auto outerIt = DataStructure.find(c);

    if (outerIt != DataStructure.end()) {
        // Group already exists, return nullptr
        return nullptr;
    }

    // Create a new group
    DataStructure.emplace(c, map<int, vector<Item>*>());

    auto& innerMap = DataStructure[c];

    // Iterate over subgroups and items to insert
    auto subgroupIt = subgroups.begin();
    auto itemIt = items.begin();

    while (subgroupIt != subgroups.end() && itemIt != items.end()) {
        int subgroupKey = *subgroupIt;
        auto& subgroup = innerMap[subgroupKey];

        // Check if the subgroup already exists
        if (!subgroup) {
            // Subgroup does not exist, create a new one
            subgroup = new std::vector<Item>();
        }

        for (const auto& item : *itemIt) {

            string itemName = get<0>(item);
            if (itemName.empty()) {
                cout << "Name can't be empty" << endl;
                return nullptr;
            }
            optional<variant<Date, Time>> timestamp = get<1>(item);
            
            Item* newItem;
            if (timestamp.has_value())
            {
                variant<Date, Time> date = timestamp.value();
                newItem = new Item(c, subgroupKey, itemName, date);
            }
            else
            {
                Date randomDate = Date::CreateRandomDate(Date(1, 1, 2000), Date(31, 12, 2022));
                newItem =  new Item(c, subgroupKey, itemName, randomDate);
            }
            subgroup->push_back(*newItem);
        }

        // Move to the next subgroup and items
        subgroupIt++;               //enne oli pluss ees
        itemIt++;
    }

    // Return the raw pointer to the inserted group
    return &innerMap;
}

bool Data::RemoveItem(char c, int i, std::string s)
{
    auto outerIt = DataStructure.find(c);

    if (outerIt != DataStructure.end()) {
        auto& innerMap = outerIt->second;
        auto innerIt = innerMap.find(i);

        if (innerIt != innerMap.end()) {
            // Find an item with the specified name in the subgroup
            auto& items = *(innerIt->second);
            auto itemIt = std::find_if(items.begin(), items.end(),
                [&s](const Item& item) { return item.GetName() == s; });

            if (itemIt != items.end()) {
                items.erase(itemIt);

                // If the subgroup is empty, remove it
                if (items.empty()) {
                    innerMap.erase(innerIt);

                    // If the group is empty, remove it
                    if (innerMap.empty()) {
                        DataStructure.erase(outerIt);
                    }
                }
                return true;
            }
        }
    }

    return false;  // Group, subgroup, or item not found
}

bool Data::RemoveSubgroup(char c, int i)
{
    auto outerIt = DataStructure.find(c);

    if (outerIt != DataStructure.end()) {
        auto& innerMap = outerIt->second;
        auto innerIt = innerMap.find(i);

        if (innerIt != innerMap.end()) {
            innerMap.erase(innerIt);

            // If the group is empty, remove it
            if (innerMap.empty()) {
                DataStructure.erase(outerIt);
            }
            return true;
        }
    }

    return false;  // Group or subgroup not found
}

bool Data::RemoveGroup(char c)
{
    auto outerIt = DataStructure.find(c);

    if (outerIt != DataStructure.end()) {
        DataStructure.erase(outerIt);
        return true;
    }

    return false;  // Group not found
}



Data::Data(int n)
{
    for (int i = 0; i < n; i++) {
        
        Item* item = new Item();
        char groupKey = item->GetGroup();
        int subKey = item->GetSub();
        string name = item->GetName();
        variant<Date, Time> date = item->Getdate();
        InsertItem(groupKey, subKey, name, date);
    }
    // Constructs the object and fills the container with n empty vectors.
}

Data::Data() { }// Constructs the object with an empty container;

Data& Data::operator=(const Data& other) {
    if (this != &other) {  // Avoid self-assignment
        // Clear existing content
        for (auto& outerPair : DataStructure) {
            for (auto& innerPair : outerPair.second) {
                delete innerPair.second;
            }
        }
        DataStructure.clear();

        // Deep copy of the DataStructure
        for (const auto& outerPair : other.DataStructure) {
            for (const auto& innerPair : outerPair.second) {
                DataStructure[outerPair.first][innerPair.first] = new std::vector<Item>(*(innerPair.second));
            }
        }
    }
    return *this;
}

bool Data::operator==(const Data& other) const {
    return DataStructure == other.DataStructure;
}


Data::~Data() {
    // Release memory allocated for vectors
    for (auto& outerPair : DataStructure) {
        for (auto& innerPair : outerPair.second) {
            delete innerPair.second;
        }
    }
    // Clear the map
    DataStructure.clear();
}


void Data::PrintAll() {

    for (const auto& outerPair : DataStructure) {
        for (const auto& innerPair : outerPair.second) {
            for (const auto& item : *(innerPair.second)) {
                item.ShowAll();
            }
        }
    }
}

std::map<int, std::vector<Item>*>* Data::GetGroup(char c) {
    auto it = DataStructure.find(c);
    if (it != DataStructure.end()) {
        return &(it->second);
    }
    return nullptr;  // Group not found
}


void Data::PrintGroup(char c) {
    if (isalpha(c))
    {
        std::map<int, std::vector<Item>*>* group = GetGroup(c);

        if (group) {
            std::cout << "Items in Group " << c << ":" << std::endl;

            for (const auto& innerPair : *group) {
                for (const auto& item : *(innerPair.second)) {
                    item.ShowAll();
                }
            }
        }
        else
        {
            cout << "No items in group" << endl;
        }
    }
    else
    {
        cout << "Invalid group name" << endl;
    }
}

bool Data::operator<(const Data& other) const {
    // Iterate through the DataStructure map
    for (const auto& [groupKey, groupValue] : DataStructure) {
        // Check if the other object contains the current group
        auto it = other.DataStructure.find(groupKey);
        if (it == other.DataStructure.end()) {
            // Group not found in the other object, so this object is less
            return true;
        }
        // Compare subgroup keys within the group
        for (const auto& [subKey, subgroup] : groupValue) {
            auto otherIt = it->second.find(subKey);
            if (otherIt == it->second.end()) {
                // Subgroup not found in the other object, so this object is less
                return true;
            }
            
        }
        // Check if one group has fewer subgroups than the other
        if (groupValue.size() < it->second.size()) {
            // This object has fewer subgroups, so it's considered less
            return true;
        }
        else if (it->second.size() < groupValue.size()) {
            // Other object has fewer subgroups, so this object is considered greater
            return false;
        }
    }
    // All groups, subgroups, and items are equal, so this object is not less than the other
    return false;
}

