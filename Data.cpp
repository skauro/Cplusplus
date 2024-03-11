#include "Main.h"


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
    else {
        // Group not found
        return 0;
    }
}

std::vector<Item>* Data::GetSubgroup(char c, int i) {
    auto outerIt = DataStructure.find(c);

    if (outerIt != DataStructure.end()) {
        auto& innerMap = outerIt->second;
        auto innerIt = innerMap.find(i);

        if (innerIt != innerMap.end()) {
            std::vector<Item>* subgroupCopy = new std::vector<Item>(*(innerIt->second)); 
            return subgroupCopy;
            //return innerIt->second;
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
        std::cout << "Group or subgroup not found." << std::endl;
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
        item.value().ShowAll();  // Assuming you have a function to display item information
    }
    else {
        std::cout << "Item not found." << std::endl;
    }
}

optional<Item> Data::InsertItem(char c, int i, string s, optional<variant<Date, Time>> v) {
    // Check if the name is not an empty string
    if (s.empty()) {
        return nullopt;
    }

    auto outerIt = DataStructure.find(c);

    if (outerIt == DataStructure.end()) {
        // Create the missing group
        DataStructure.emplace(c, std::map<int, std::vector<Item>*>());
        DataStructure[c].emplace(i, new std::vector<Item>());

    }

    auto& innerMap = DataStructure[c];

    // Check if the subgroup does not exist
    auto innerIt = innerMap.find(i);
    if (innerIt == innerMap.end()) {
        // Subgroup does not exist, return nullopt or take appropriate action
        return nullopt;
    }


    // Use Date::CreateRandomDate() if Timestamp is not specified
    Date randomDate = Date::CreateRandomDate(Date(1, 1, 2000), Date(31, 12, 2022));
    Item newItem(c, i, s, v.value_or(randomDate));  // Use the provided variant or default to random Date

    // Insert the new item into the subgroup
    innerMap[i]->push_back(newItem);

    // Return the inserted item wrapped in std::optional
    return optional<Item>(newItem);
}

vector<Item>* Data::InsertSubgroup(char c, int i, initializer_list<tuple<string, optional<variant<Date, Time>>>> items) {

    auto outerIt = DataStructure.find(c);

    if (outerIt == DataStructure.end()) {
        // Create the missing group
        DataStructure.emplace(c, map<int, unique_ptr<vector<Item>>>{});
    }

    auto& innerMap = DataStructure.at(c);
    innerMap.emplace(i, make_unique<vector<Item>>());
    // Check if the subgroup already exists
    auto innerIt = innerMap.find(i);
    if (innerIt != innerMap.end()) {
        // Subgroup already exists, return nullptr
        return nullptr;
    }

    // Create a new subgroup with the provided items
    auto newSubgroup = make_unique<vector<Item>>();
    for (const auto& item : items) {
        string itemName = get<0>(item);
        optional<variant<Date, Time>> timestamp = get<1>(item);

        // Check if the name is not an empty string
        if (itemName.empty()) {
            return nullptr;
        }

        Date randomDate = Date::CreateRandomDate(Date(1, 1, 2000), Date(31, 12, 2022));
        Item newItem(c, i, itemName, timestamp.value_or(randomDate));
        newSubgroup->push_back(newItem);
    }

    // Insert the new subgroup into the inner map using std::move
    innerMap.emplace(i, std::move(newSubgroup));

    // Return the raw pointer to the inserted subgroup
    return innerMap[i];

}
std::map<int, std::vector<Item>*>* Data::InsertGroup(char c, std::initializer_list<int> subgroups,
    std::initializer_list<std::initializer_list<std::tuple<std::string, std::optional<std::variant<Date, Time>>>>> items)
{
    auto outerIt = DataStructure.find(c);

    if (outerIt != DataStructure.end()) {
        // Group already exists, return nullptr
        return nullptr;
    }

    // Create a new group
    DataStructure.emplace(c, map<int, unique_ptr<vector<Item>>>());

    auto& innerMap = DataStructure[c];

    // Iterate over subgroups and items to insert
    auto subgroupIt = subgroups.begin();
    auto itemIt = items.begin();

    while (subgroupIt != subgroups.end() && itemIt != items.end()) {
        int subgroupKey = *subgroupIt;
        auto& subgroup = innerMap[subgroupKey];

        // Check if the subgroup already exists
        if (subgroup) {
            return nullptr;
        }

        // Create a new subgroup with the provided items
        auto newSubgroup = make_unique<vector<Item>>();

        for (const auto& item : *itemIt) {
            string itemName = get<0>(item);
            optional<variant<Date, Time>> timestamp = get<1>(item);

            // Check if the name is not an empty string
            if (itemName.empty()) {
                return nullptr;
            }

            Date randomDate = Date::CreateRandomDate(Date(1, 1, 2000), Date(31, 12, 2022));
            Item newItem(c, subgroupKey, itemName, timestamp.value_or(randomDate));
            subgroup->push_back(newItem);
        }

        // Move to the next subgroup and items
        ++subgroupIt;
        ++itemIt;
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
    for (int i = 0; i < n; ++i) {

        Item key; //itemist constructor

        // Get the char value from the Item instance
        char keyChar = key.GetGroup();  

        // Create an empty inner map for the current item key
        std::map<int, std::vector<Item>*> innerMap;

        // Insert the inner map into the DataStructure map
        DataStructure[keyChar] = innerMap;
  
    }
    // Constructs the object and fills the container with n empty vectors.
}
Data::Data() { 
    // No need to do anything specific here since DataStructure is already initialized as an empty container.
}// Constructs the object with an empty container;

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
    // Flatten the items into a single vector for sorting
    std::vector<Item> flattenedItems;

    for (const auto& outerPair : DataStructure) {
        for (const auto& innerPair : outerPair.second) {
            for (const auto& item : *(innerPair.second)) {
                flattenedItems.push_back(item);
            }
        }
    }

    // Sort the flattenedItems vector based on group, subgroup, and item names
    std::sort(flattenedItems.begin(), flattenedItems.end(),
        [](const Item& a, const Item& b) {
            if (a.GetGroup() != b.GetGroup()) {
                return a.GetGroup() < b.GetGroup();
            }
            if (a.GetSub() != b.GetSub()) {
                return a.GetSub() < b.GetSub();
            }
            return a.GetName() < b.GetName();
        });

    // Print the sorted items in an easily readable format
    for (const auto& item : flattenedItems) {
        std::cout << item.GetGroup() << ":" << item.GetSub() << ": ";
        item.ShowAll();
        std::cout << std::endl;
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
    std::map<int, std::vector<Item>*>* group = GetGroup(c);

    if (group) {
        std::cout << "Items in Group " << c << ":" << std::endl;

        // Flatten the items into a single vector for sorting
        std::vector<Item> flattenedItems;
        for (const auto& innerPair : *group) {
            for (const auto& item : *(innerPair.second)) {
                flattenedItems.push_back(item);
            }
        }

        // Sort the flattenedItems vector based on the item names
        std::sort(flattenedItems.begin(), flattenedItems.end(),
            [](const Item& a, const Item& b) { return a.GetName() < b.GetName(); });

        // Print the sorted items
        for (const auto& item : flattenedItems) {
            item.ShowAll();
        }
    }
    else {
        throw std::invalid_argument("Group not found.");
    }
}
