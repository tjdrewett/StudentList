#include "Students.h"
#include <string>

//Member function definitions
Students::Students()
{
	sName = "(null)";
	sID = "(null)";
	sClass = "(null)";
}

//Setters
void Students::setName(string name)
{
	sName = name;
}

void Students::setID(string id)
{
	sID = id;
}

void Students::setClass(string classes)
{
	sClass = classes;
}


//Getters
string Students::getName() const
{
	return sName;
}

string Students::getID() const
{
	return sID;
}

string Students::getClass() const
{
	return sClass;
}