#pragma once
#include <map>
#include <optional>

class Data {
private:
    std::map<char, std::map<int, std::vector<Item>*>> DataStructure;

public:
    Data(int n);
    Data();
    ~Data();

    Data(const Data& other);
    Data& operator=(const Data& other);
    bool operator==(const Data& other) const;
    // Public methods as per the requirements
    void PrintAll();
    int CountItems();
    std::map<int, std::vector<Item>*>* GetGroup(char c);
    void PrintGroup(char c);
    int CountGroupItems(char c);
    std::vector<Item>* GetSubgroup(char c, int i);
    void PrintSubgroup(char c, int i);
    int CountSubgroupItems(char c, int i);
    std::optional<Item> GetItem(char c, int i, std::string s);
    void PrintItem(char c, int i, std::string s);
    std::optional<Item> InsertItem(char c, int i, std::string s, std::optional<std::variant<Date, Time>> v = std::nullopt);
    std::vector<Item>* InsertSubgroup(char c, int i, std::initializer_list<std::tuple<std::string, std::optional<std::variant<Date, Time>>>> items);
    std::map<int, std::vector<Item>*>* InsertGroup(char c, std::initializer_list<int> subgroups, std::initializer_list<std::initializer_list<std::tuple<std::string, std::optional<std::variant<Date, Time>>>>> items);
    bool RemoveItem(char c, int i, std::string s);
    bool RemoveSubgroup(char c, int i);
    bool RemoveGroup(char c);

};
