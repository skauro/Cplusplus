#include "Main.h"


extern default_random_engine Generator;
extern uniform_int_distribution<int> GroupDistribution;
extern uniform_int_distribution<int> SubgroupDistribution;
extern uniform_int_distribution<int> VariantDistribution;
const Date Item::Begin = Date(1, 1, 2018), Item::End = Date(31, 12, 2018);

Item::Item()
{ // creates a pseudo-random item
	Group = (char)GroupDistribution(Generator);
	Subgroup = SubgroupDistribution(Generator);
	Name = ::CreateRandomName();
	int var = VariantDistribution(Generator);
	if (var % 2)
	{
		Timestamp = Date::CreateRandomDate(Begin, End);
	}
	else
	{
		Timestamp = Time::CreateRandomTime();
	}
}
void Item::ShowAll() const {
	std::cout << "Group: " << Group << std::endl
		<< "Subgroup: " << Subgroup << std::endl
		<< "Name: " << Name << std::endl;

	std::visit([&](auto&& arg) {
		// Check the type of arg and call the appropriate ToString function
		if constexpr (std::is_same_v<decltype(arg), Date>) {
			std::cout << "Timestamp: " << arg.ToString() << std::endl;
		}
		else if constexpr (std::is_same_v<decltype(arg), Time>) {
			std::cout << "Timestamp: " << arg.ToString() << std::endl;
		}
		}, Timestamp);
}


// Constructor with Date
Item::Item(char group, int subgroup, string name, Date timestamp) :
	Group(group), Subgroup(subgroup), Name(name), Timestamp(timestamp) {
}

// Constructor with Time
Item::Item(char group, int subgroup, string name, Time timestamp){
	Group = group;
	Subgroup = subgroup;
	Name = name;
	Timestamp = timestamp;
}

// Constructor with variant<Date, Time>
Item::Item(char group, int subgroup, string name, variant<Date, Time> timestamp)
	: Group(group), Subgroup(subgroup), Name(name), Timestamp(timestamp) {	
}

// Copy constructor
Item::Item(const Item& other)
	: Group(other.Group), Subgroup(other.Subgroup), Name(other.Name), Timestamp(other.Timestamp) {
}

// Destructor
Item::~Item() {
	// Clean in main after use? or no need
}

