#pragma once
#include <variant>
class Item
{
private:
	char Group;     // Any from range 'A'...'Z'
	int Subgroup;   // Any from range 0...99
	string Name;    // Any, but not empty 
	variant<Date, Time> Timestamp; // Any object of class Date or class Time
	static const Date Begin;
	static const Date End;
public:
	Item();
	Item(char, int, string, Date);
	Item(char, int, string, Time);
	Item(char, int, string, variant<Date, Time>);
	Item(const Item&);
	~Item();

// To do

	char GetGroup() const { return Group; }
	int GetSub() const { return Subgroup; }
	string GetName() const  { return Name; }
	variant<Date, Time> Getdate() const { return Timestamp; }

	void ShowAll() const;
	
};
