#ifndef STUDENTS_H
#define STUDENTS_H

#include <string>
using namespace std;

class Students
{
private:
	string sID, sName, sClass;
public:
	Students();
	void setName(string);
	void setID(string);
	void setClass(string);

	string getName() const;
	string getID() const;
	string getClass() const;
};


#endif