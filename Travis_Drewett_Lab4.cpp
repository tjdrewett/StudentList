/*Travis Drewett 
CISC 192

This program allows a user to input, edit, and output a list of students.
*/


#include <iostream>
#include <string>
#include <cctype>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdio>
#include "Students.h"

using namespace std;

//Main Menu
const int READ = 1, ADD = 2, DISPLAY = 3, SORT = 4, WRITE = 5, SEARCH = 6, EXIT = 7;

//Sort/Search Menu
const int NAME = 1, ID = 2, CLASS = 3;

//Number of characters in student ID
const int IDSPACES = 7;

void emptyList(), displayVector(vector<Students>);
string valNumInput(string);
string getID(), convertToUpper(string), valAlpha(string);
vector<int> binarySearchName(const vector <Students> &, string, int, int), binarySearchID(const vector <Students> &, string, int, int);
bool sortName(const Students &, const Students &), sortID(const Students &, const Students &), sortClass (const Students &, const Students &);

int main()
{
	int iErrorCounter = 0, iStudentNum, iClassNum, iMatch, iChoice, iTemp;
	string sChoice, sTemp, sFirstName, sMiddleName, sLastName, sID, sClass, sFullName, sQuit;
	bool bListSorted = false, bTryAgain = true;
	ifstream inputFile;
	ofstream outputFile;
	vector<string> vFullName, vID, vClass, vFileName;
	vector<int> vMatches;
	vector<Students> vStudents;
	Students student;

	do
	{
		//Create a tag here so that later in the code we can go back to the menu under specific circumstances.
		BackToMenu:
		do
		{
			//Here we  set the variable that will be passed into the displayMenu function. This will allow us to dynamically monitor
			//how the student list we have in our vectors is being sorted.
			cout << "\n\n\tStudent Enrollment Application\n\n";
			cout << "1: Read an external student list\n"
				<< "2: Add student's information into a student list\n"
				<< "3: Display student list\n"
				<< "4: Sort student list\n"
				<< "5: Save student list to file\n"
				<< "6: Search the student list\n"
				<< "7: Exit application\n\n";

			cout << "Please select an option from the menu: ";

			cin >> noskipws >> sChoice;
		
			/*5/17/2012 
			
			This is where I'm stopping. I currently need to figure out why my code stores something in the keyboard buffer whenever I put 
			in three invalid options to the menu.*/
		ProcessChoice:
			sChoice = valNumInput(sChoice);

		} while (sChoice == "Invalid");

		iChoice = stoi(valNumInput(sChoice));

		//This makes sure the user can't enter multiple letters.
		if (iChoice > 7 || iChoice < 0)
		{
			iChoice = -1;
		}

		switch (iChoice)
		{
			case READ:
			{


				iErrorCounter = 0;

				//Use cin.ignore here since the menu choice was made using an integer.
				cin.ignore();

				//Set a tag here so that we don't have to start from the main menu again, if the user needs to 
				//enter a different file name.
				cout << "\n\nPlease enter a file name to read from: ";
				getline(cin, sTemp);

				/*We set the file name given to us by the user to be all upper case. This way,
				when we search the vector that this user input will be stored in, we can match up
				if the file name has already been entered.*/
				Read:
				sTemp = convertToUpper(sTemp);

				inputFile.open(sTemp.c_str());

				/*If we can't read from the file, we will ask the user for a different input.
				If their input is invalid for two more attempts, then they receive an error message 
				and are sent back to the main menu.*/
				if (!inputFile)
				{
					if (iErrorCounter == 2)
					{
						cout << "\nThis program only allows 3 attempts. Restarting program...";
						iErrorCounter = 0;
						goto BackToMenu;
					}

					/*4/29/2017
					
					I'm taking a break. I have the input validation working for the main menu. All I need to do
					is use my valNumInput function, test if it returns -1, and if it does, then go back to the previous menu.
					I could do this either with tags (easy but cheap), or possibly a do-while loop (harder but more debuggable)*/

					iErrorCounter++;
					cout << "Invalid file name! Please try again: ";
					getline(cin, sTemp);
					goto Read;
				}

				/*We start here by searching vFileName for the file that the user wants to open. If it was opened (and subsequently added to the list),
				then we will show an error and prompt if the user would like to try again. If not, they are sent back to the main menu. If they do
				then they are brought back to the tag we set up earlier.*/

				if (binary_search(vFileName.begin(), vFileName.end(), sTemp) == true)
				{
					cout << "\n\nCan't open the same file twice!";
					goto BackToMenu;					
				}

				/*If the file name was opened successfully, it will be stored into the vFileName vector, then
				sorted so that it can be searched later.*/
				
				vFileName.push_back(sTemp);
				sort(vFileName.begin(), vFileName.end());

				/*This while loop separates each line from the file into separate variables.
				It also remmoves the sentinel '|' and replaces it with a space.*/ 
				while (getline(inputFile, sTemp))
				{
					istringstream inputStringStream(sTemp);
					inputStringStream >> sFirstName >> sMiddleName >> sLastName >> sID >> sClass;

					if (sMiddleName == "|")
					{
						sFullName = sFirstName + " " + sLastName;
					}
					else
					{
						sFullName = sFirstName + " " + sMiddleName + " " + sLastName;
					}

					student.setName(sFullName);
					student.setID(sID);
					student.setClass(sClass);

					vStudents.push_back(student);

					//vFullName.push_back(sFullName);
					//vID.push_back(sID);
					//vClass.push_back(sClass);

				}

				inputFile.close();

				cout << "\nThe size of the list is: " << vStudents.size();
				cout << "\nThe file was successfully read!";

				break;
			}
			case ADD:
			{
				/*We use the bListSorted boolean variable to determine if the list
				has been sorted. Any time we add a student, we set this value to false,
				since the student list will not be sorted when a new student is added.*/
				bListSorted = false;

				Students newStudent;

				cin.ignore();
				do
				{
					cout << "\n\nThe current size of the Student List is: " << vStudents.size();
					cout << "\n\nHow many students would you like to add: ";
					cin >> noskipws >> sTemp;

					sTemp = valNumInput(sTemp);
				} while (sTemp == "Invalid");

				iStudentNum = stoi(valNumInput(sTemp));


				//We use cin.ignore() before the for loop since there is a getline() function inside the loop.
				cin.ignore();
				for (int i = 0; i < iStudentNum; i++)
				{
					cout << "\nStudent " << i + 1 << endl;
					EnterName:
					cout << "Please enter Student's first name: ";
					getline(cin, sTemp);

					/*Concatenate the first, middle and last name into a single string variable, which we can then store into a vector.
					This makes sorting much easier, since the full name of the student will stay together no matter what.*/

					//LAB 4, I will need to combine this all into one value to be entered by the user, then format it accordingly.
					sFullName = sTemp + " ";

					cout << "Please enter Student's middle name: ";
					getline(cin, sTemp);

					sFullName += sTemp + " ";

					cout << "Please enter Student's last name: ";
					getline(cin, sTemp);

					sFullName += sTemp;

					//Make sure that the full name of the student is in all capital letters.
					sFullName = convertToUpper(sFullName);

					/*We sort by the student's full name here so that we can search the vector
					and ensure that name isn't already on the list.*/
					sort(vStudents.begin(), vStudents.end(), sortName);
					bListSorted = true;

					//Check if the name provided by the user is on the list, and return an array of matching indexes.
					vMatches = binarySearchName(vStudents, sFullName, 0, vStudents.size() - 1);

					//If the vMatches array is not empty, then that means the full name is on the list.
					if (!vMatches.empty())
					{
						cout << "\nThis student is already on the list!\n";
						vMatches.clear();
						goto EnterName;
					}

					newStudent.setName(sFullName);

					//vFullName.push_back(sFullName);

					//Sort by ID so that we can ensure the ID is unique later on.
					//tripleSort(vID, vFullName, vClass, vID.size());
					sort(vStudents.begin(), vStudents.end(), sortID);

					//Since we use the tripleSort function, we can mark our student list as sorted.
					bListSorted = true;

					StudentIDMenu:
					cout << "\n\nSutdent ID:\n"
						<< "1.) Generate Student ID\n"
						<< "2.) Create Student ID\n"
						<< "Please select an option: ";
					cin >> noskipws >> sTemp;

					sTemp = valNumInput(sTemp);

					if (sTemp == "Invalid")
					{
						cout << "\n\nInvalid input entered too many times!\n\n";
						goto StudentIDMenu;
					}

					iTemp = stoi(sTemp);

					iErrorCounter = 0;

					//cin.ignore();

					while (iTemp < 1 || iTemp > 2)
					{
						if (iErrorCounter == 2)
						{
							cout << "\n\nInvalid input entered too many times!\n\n";
							cin.clear();
							cin.ignore(INT_MAX, '\n');
							goto StudentIDMenu;
						}

						cout << "Please enter within the range of 1 to 2: ";
						cin.clear();
						cin.ignore(INT_MAX, '\n');
						cin >> noskipws >> sTemp;

						iTemp = stoi(valNumInput(sTemp));
					}

					cin.ignore();
					if (iTemp == 1)
					{
						sTemp = getID();

						//We use this while loop to ensure the number that is randomly generated isn't already in the ID vector.
						while (binary_search(vID.begin(), vID.end(), sTemp))
						{
							sTemp = getID();
						}

						cout << "\nYour student ID is: " << sTemp;

						newStudent.setID(sTemp);
					}
					else if (iTemp == 2)
					{
						cout << "\nPlease enter ID: ";
						getline(cin, sTemp);

						vMatches = binarySearchID(vStudents, sTemp, 0, vStudents.size() - 1);

						/*This while loops checks that the user has entered at least 7 characters for the student ID.
						and that the name isn't already on the list.*/
						while (sTemp.length() != IDSPACES || !vMatches.empty())
						{
							cout << "\nThe user ID must conatin 7 characters and be unique!\n";
							goto StudentIDMenu;
						}

						newStudent.setID(sTemp);
					}

					ClassMenu:
					cout << "\n\nPlease enter the number of classes this student has: ";
					cin >> noskipws >> sTemp;

					sTemp = valNumInput(sTemp);

					if (sTemp == "Invalid")
					{
						cout << "\n\nInvalid input entered too many times!\n\n";
						goto ClassMenu;
					}

					iClassNum = stoi(sTemp);


					cin.ignore();

					//This while loop prevents the user from entering more than four classes
					while (!(iClassNum <= 4))
					{
						cout << "\n\nYou may only enter up to four classes: ";
						cin >> noskipws >> sTemp;

						iClassNum = stoi(valNumInput(sTemp));
						cin.ignore();
					}

					cout << endl;

					//Here we empty the sClass variable, so it can be used to concatonate a new string of student classes.
					sClass = "";

					for (int i = 0; i < iClassNum; i++)
					{
						cout << "Enter class " << i + 1 << ": ";
						getline(cin, sTemp);

						//Here we concatonate the classes that the student wishes to add together.
						sClass += sTemp + " ";
					}

					sClass = convertToUpper(sClass);

					newStudent.setClass(sClass);
					//vClass.push_back(sClass);

					vStudents.push_back(newStudent);
				}

				cout << "\nSuccessfully added " << iStudentNum << " students to the list.\n";
				break;
			}

			case DISPLAY:
			{
				//If the vector is empty, then we display an error message and return to the main menu.
				if (vStudents.empty())
				{
					cin.clear();
					cin.ignore(INT_MAX, '\n');
					emptyList();
					break;
				}

				displayVector(vStudents);

				cin.ignore();

				break;
			case SORT:

				if (vStudents.empty())
				{
					cin.clear();
					cin.ignore(INT_MAX, '\n');
					emptyList();
					break;
				}

				iErrorCounter = 0;


				cin.ignore();

				/*Prompt the user if they would like to sort by name, ID, or class. 
				Process their choice and sort accordingly.*/
				do
				{
					do
					{
						cout << "\nWhat would you like to sort by?";
						cout << "\n\n1.) Name"
							<< "\n2.) ID"
							<< "\n3.) Class"
							<< "\n\nPlease enter your choice: ";
						cin >> noskipws >> sTemp;

						ClassChoice:
						sTemp = valNumInput(sTemp);

					} while (sTemp == "Invalid");

					iTemp = stoi(sTemp);

					switch (iTemp)
					{
					case NAME:
					{
						sort(vStudents.begin(), vStudents.end(), sortName);

						cout << "\nSuccessfully sorted by name!";

						sTemp = "N";
						break;
					}
					case ID:
					{
						sort(vStudents.begin(), vStudents.end(), sortID);

						cout << "\nSuccessfully sorted by ID!";

						sTemp = "N";
						break;
					}
					case CLASS:
					{
						sort(vStudents.begin(), vStudents.end(), sortClass);

						cout << "\nSuccessfully sorted by class!";

						sTemp = "N";
						break;
					}
					default:
					{
						cout << "Please select an option within the range of 1 through 4: ";
						cin.ignore();
						cin >> noskipws >> sTemp;

						iErrorCounter++;
						if (iErrorCounter == 2)
						{
							cin.clear();
							cin.ignore(INT_MAX, '\n');
							iErrorCounter = 0;
							break;
						}
						goto ClassChoice;
					}
					}					
				} while (toupper(sTemp[0]) != 'N');

				bListSorted = true;

				//This is so the keyboard buffer doesn't mess up our main menu.
				cin.ignore();
				break;
			}
			case WRITE:
			{
				if (vStudents.empty())
				{
					cin.clear();
					cin.ignore(INT_MAX, '\n');
					emptyList();
					break;
				}

				cout << "\nPlease enter a file name to save the student list to: ";
				cin.ignore();
				getline(cin, sTemp);

				//Open file for writing
				outputFile.open(sTemp.c_str());

				outputFile << left << setw(30) << "\nName" << setw(10) << " ID" << setw(35) << " Class" << endl;
				outputFile << "----------------------------------------------------------------------\n";

				//Output vector values to an external file using the same format we use to display it.
				for (int i = 0; i < vStudents.size(); i++)
				{
					outputFile << left << setw(30) << vStudents[i].getName() << setw(10) << vStudents[i].getID() << setw(35) << vStudents[i].getClass() << endl;
				}

				outputFile.close();

				cout << "\nStudent list successfully saved to the file: " << sTemp;

				break;
			}
			case SEARCH:
			{
				if (vStudents.empty())
				{
					cin.clear();
					cin.ignore(INT_MAX, '\n');
					emptyList();
					break;
				}
				else if (bListSorted == false)
				{
					sort(vStudents.begin(), vStudents.end(), sortName);
					bListSorted = true;
				}

				cin.ignore();

				//Here we provide the user an option to search by name and search by ID.
				do
				{
					do
					{
						cout << "\nWhat would you like to search for?";
						cout << "\n\n1.) Name"
							<< "\n2.) ID"
							<< "\n\nPlease enter your choice: ";
						cin >> noskipws >> sTemp;

					ProcessSearch:
						sTemp = valNumInput(sTemp);
					} while (sTemp == "Invalid");


					iTemp = stoi(sTemp);


					switch (iTemp)
					{
					case NAME:
					{
						cin.ignore();

						//When searching by name, we sort the student list by the student name
						sort(vStudents.begin(), vStudents.end(), sortName);

						bListSorted = true;

						cout << "\nPlease enter a student name to search: ";
						getline(cin, sTemp);

						//Convert user input to upper case
						sTemp = convertToUpper(sTemp);

						//Find if that name is already in the student list
						vMatches = binarySearchName(vStudents, sTemp, 0, vStudents.size() - 1);

						if (vMatches.empty())
						{
							cout << "\nNo matches found!";

							cout << "\n\nWould you like to search again?(Y/N): ";
							getline(cin, sTemp);

							continue;
						}

						//If the student name is found in the list, we sort the index's we've gathered and output them to the command prompt
						//The sorting is done so that the values maintain a proper alphabetical order.
						sort(vMatches.begin(), vMatches.end());

						cout << left << setw(35) << "\nName" << setw(15) << " ID" << setw(25) << " Class" << endl;
						cout << "----------------------------------------------------------------------\n";

						for (int i = 0; i < vMatches.size(); i++)
						{
							iMatch = vMatches[i];
							cout << left << setw(35) << vStudents[iMatch].getName() << setw(15) << vStudents[iMatch].getID() << setw(25) << vStudents[iMatch].getClass() << endl;
						}

						vMatches.clear();

						cout << "\n\nWould you like to search again?(Y/N): ";
						getline(cin, sTemp);

						break;
					}
					case ID:
					{
						cin.ignore();

						sort(vStudents.begin(), vStudents.end(), sortID);

						bListSorted = true;

						cout << "\nPlease enter a student ID to search: ";
						getline(cin, sTemp);

						//Convert user input to upper case
						sTemp = convertToUpper(sTemp);

						vMatches = binarySearchID(vStudents, sTemp, 0, vStudents.size() - 1);

						if (vMatches.empty())
						{
							cout << "\nNo matches found!";

							vMatches.clear();

							cout << "\n\nWould you like to search again?(Y/N): ";
							getline(cin, sTemp);

							continue;
						}
						else
						{

							sort(vMatches.begin(), vMatches.end());

							cout << left << setw(35) << "\nName" << setw(15) << " ID" << setw(25) << " Class" << endl;
							cout << "----------------------------------------------------------------------\n";


							for (int i = 0; i < vMatches.size(); i++)
							{
								iMatch = vMatches[i];
								cout << left << setw(35) << vStudents[iMatch].getName() << setw(15) << vStudents[iMatch].getID() << setw(25) << vStudents[iMatch].getClass() << endl;
							}

							vMatches.clear();

							cout << "\n\nWould you like to search again?(Y/N): ";
							getline(cin, sTemp);
						}
						break;
					}
					default:
					{
						iErrorCounter++;
						if (iErrorCounter == 3)
						{
							cin.clear();
							cin.ignore(INT_MAX, '\n');
							iErrorCounter = 0;
							break;
						}
						cout << "\nPlease select option 1 or 2: ";
						cin.ignore();
						cin >> noskipws >> sTemp;

						goto ProcessSearch;
					}
					}

					sTemp = toupper(sTemp[0]);

				} while (toupper(sTemp[0]) != 'N');

				break;
			}
			case EXIT:
			{

				cout << "\nExiting application!";
				cout << "\nAre you sure you would like to exit? (Y/N): ";
				cin.ignore();
				getline(cin, sQuit);
				break;
			}
			default:
				
				cout << "Please select an option within the range of 1 through 7: ";
				cin.ignore();
				cin >> noskipws >> sChoice;
				
				iErrorCounter++;
				if (iErrorCounter == 2)
				{
					cin.clear();
					cin.ignore(INT_MAX, '\n');
					iErrorCounter = 0;
					break;
				}
				goto ProcessChoice;


	}

	} while (toupper(sQuit[0]) != 'Y');

	cout << "\nApplication is terminating.";

	return 0;

}


//Use this boolean function in the basic sort function to sort the list by class
bool sortClass(const Students & s1, const Students & s2)
{
	return s1.getClass() < s2.getClass();
}

//Use this boolean function in the basic sort function to sort the list by name
bool sortName(const Students & s1, const Students & s2)
{
	return s1.getName() < s2.getName();
}

//Use this boolean function in the basic sort function to sort the list by ID
bool sortID(const Students & s1, const Students & s2)
{
	return s1.getID() < s2.getID();
}


/*This function requires a vector to be searched, a string that is the search term, the minimum index of the vector to search
and the maximum index of the vector to search. It returns a vector of all of the indexes that match the string that was searched
regardless of whether the user entered the entire name or just part of the name*/
vector<int> binarySearchName(const vector <Students> & vName, string sSearch, int min, int max)
{
	int iMatchMid;
	vector<int> matches;

	//This checks if the data structure is empty
	if (max < min)
		return matches;
	else
	{
		//Calculate Midpoint to cut the array in half
		int mid = (min + max) / 2;


		//Three way comparison
		//Compare only the substring of the current index's value with the search term.
		if (vName[mid].getName().substr(0, sSearch.length()) > sSearch)
			//Search Value is in the Lower Subset. Mid - 1 becomes the new max value
			return binarySearchName(vName, sSearch, min, mid - 1);
		else if (vName[mid].getName().substr(0, sSearch.length()) < sSearch)
			//Search Value is in the Upper Subset. Mid + 1 becomes the new minimum value
			return binarySearchName(vName, sSearch, mid + 1, max);
		else
		{

			//Search Value has been found
			iMatchMid = mid;

			//Store the first index that has a matching value into the vector
			matches.push_back(mid);


			////ERROR: If the name is in the zero index, this would cause an out of bounds error.
			//vName[--iMatchMid].getName();

			/*This while loop looks at each index below the one we found as a match, and catches if the index value below it matches what the user is searching for. If it does, 
			then that index number is stored in a vector. The while loop afterwards checks the index value of every item above what we found. If they have matching values, 
			then the item is also stored in a vector.*/
			while (--iMatchMid != -1 && vName[iMatchMid].getName().substr(0, sSearch.length()) == sSearch)
			{
				matches.push_back(iMatchMid);
				//sVectorVal = vName[iMatchMid].getName();
			}

			iMatchMid = mid;
			//sVectorVal = vName[++iMatchMid].getName();

			while (++iMatchMid < vName.size() && vName[iMatchMid].getName().substr(0, sSearch.length()) == sSearch)
			{
				matches.push_back(iMatchMid);
				//++iMatchMid;
				//sVectorVal = vName[iMatchMid].getName();
			}

			return matches;
		}
	}
}

/*This function requires a vector to be searched, a string that is the search term, the minimum index of the vector to search
and the maximum index of the vector to search. It returns a vector of all of the indexes that match the string that was searched
regardless of whether the user entered the entire name or just part of the name*/
vector<int> binarySearchID(const vector <Students> & vName, string sSearch, int min, int max)
{
	int iMatchMid;
	vector<int> matches;

	//This checks if the data structure is empty
	if (max < min)
		return matches;
	else
	{
		//Calculate Midpoint to cut the array in half
		int mid = (min + max) / 2;


		//Three way comparison
		//Compare only the substring of the current index's value with the search term.
		if (vName[mid].getID().substr(0, sSearch.length()) > sSearch)
			//Search Value is in the Lower Subset. Mid - 1 becomes the new max value
			return binarySearchID(vName, sSearch, min, mid - 1);
		else if (vName[mid].getID().substr(0, sSearch.length()) < sSearch)
			//Search Value is in the Upper Subset. Mid + 1 becomes the new minimum value
			return binarySearchID(vName, sSearch, mid + 1, max);
		else
		{

			//Search Value has been found
			iMatchMid = mid;

			//Store the first index that has a matching value into the vector
			matches.push_back(mid);


			////ERROR: If the name is in the zero index, this would cause an out of bounds error.
			//vName[--iMatchMid].getName();

			/*This while loop looks at each index below the one we found as a match, and catches if the index value below it matches what the user is searching for. If it does,
			then that index number is stored in a vector. The while loop afterwards checks the index value of every item above what we found. If they have matching values,
			then the item is also stored in a vector.*/
			while (--iMatchMid != -1 && vName[iMatchMid].getID().substr(0, sSearch.length()) == sSearch)
			{
				matches.push_back(iMatchMid);
				//sVectorVal = vName[iMatchMid].getName();
			}

			iMatchMid = mid;
			//sVectorVal = vName[++iMatchMid].getName();

			while (++iMatchMid < vName.size() && vName[iMatchMid].getID().substr(0, sSearch.length()) == sSearch)
			{
				matches.push_back(iMatchMid);
				//++iMatchMid;
				//sVectorVal = vName[iMatchMid].getName();
			}

			return matches;
		}
	}
}


/*This function simply display's the formatted contents of three vectors.*/
void displayVector(vector<Students> vector)
{
	cout << "\nStudent List:\n\n";
	cout << "The size of the student list is: " << vector.size() << endl;
	cout << left << setw(30) << "\nName" << setw(10) << " ID" << setw(35) << " Class" << endl;
	cout << "----------------------------------------------------------------------\n";

	for (int i = 0; i < vector.size(); i++)
	{
		cout << left << setw(30) << vector[i].getName() << setw(10) << vector[i].getID() << setw(35) << vector[i].getClass() << endl;
	}
}

/*This function converts a string value to all uppercase.*/
string convertToUpper(string sVal)
{
	for (int i = 0; i < sVal.length(); i++)
	{
		sVal[i] = toupper(sVal[i]);
	}

	return sVal;
}

/*This function simply displays the main menu of the program.*/
void displayMenu(string sortStatus)
{
	cout << "\n\n\tStudent Enrollment Application\n\n";
	cout << "1: Read an external student list\n"
		<< "2: Add student's information into a student list\n"
		<< "3: Display student list\n"
		<< "4: Sort student list\n"
		<< "5: Save student list to file\n"
		<< "6: Search the student list\n"
		<< "7: Exit application\n\n";

	cout << "Please select an option from the menu: ";
}

/*This function accepts a string argument, ensures that string only contains numbers, then returns an integer*/
string valNumInput(string sNumber)
{
	int iReturn;
	int iCount = 0;

	//In this for loop, we are testing if the user has entered numbers AND letters, in which they will receive an error message
	//and be asked to enter a different input.
	ValAlpha:
	for (int i = 0; i < sNumber.length(); i++)
	{

		if (!(isdigit(sNumber[i])))
		{
			iCount++;
			//If the user has entered invalid input 3 times, our function will return the value -1
			if (iCount == 3)
			{
				cin.clear();
				cin.ignore(INT_MAX, '\n');
				return "Invalid";
			}

			cout << "\nPlease enter a valid number: ";
			cin.clear();
			cin.ignore(INT_MAX, '\n');
			i = -1;

			cin >> noskipws >> sNumber;
			continue;
		}
		else
		{
			continue;
		}
	}


	/*In addition to the previous for loop, this while loop will catch any aditional errors there may be with the user input (such as whitespaces). 
	If an issue is found, then the user is prompted to re-enter their input, the input is stored, then we use a goto statement to begin the whole
	process again and validate input.*/
	while (cin.fail())
	{
		iCount++;
		if (iCount == 3)
		{
			cin.clear();
			cin.ignore(INT_MAX, '\n');
			return "Invalid";
		}

		cout << "\nPlease enter a valid number: ";
		cin.clear();
		cin.ignore(INT_MAX, '\n');

		cin >> noskipws >> sNumber;
		goto ValAlpha;
	}

	return sNumber;
}

/*This function generates random numbers, with the amount characters specified in the constant IDSPACES. 
It returns those numbers as a string*/
string getID()
{
	int iTemp;
	string sTemp;

	ostringstream outputStringStream;

	unsigned int seed = time(0);
	srand(seed);

	for (int i = 1; i <= IDSPACES; i++)
	{
		iTemp = rand() % (9 - 1) + 1;
		outputStringStream << iTemp;
		sTemp += outputStringStream.str();
		outputStringStream.str("");
	}

	return sTemp;
}

/*This is simply to output the message that the list is empty We have multiple areas 
where we output this, so I created a function for it.*/
void emptyList()
{
		cout << "\n\nThis list is empty! Perhaps you need to load information into the list.";
}


