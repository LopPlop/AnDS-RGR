#pragma once
#include <iostream>
#include <string>

using namespace std;

template <typename T>
class Input
{
public:
	static T Get(string hintForUser);
};

template<typename T>
inline T Input<T>::Get(string hintForUser)
{
	T value; cin >> value;
	
	return value;
}

template<>
inline int Input<int>::Get(string hintForUser)
{
	cout << hintForUser << ":";
	int input;
	cin.clear();
	while (!(cin >> input))
	{
		cin.clear(); //игнорируем всё, кроме цифр
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cout << "Only numbers are required" << endl;
	}
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	return input;
}

template<>
inline bool Input<bool>::Get(string hintForUser)
{
	int input = Input<int>::Get(hintForUser + " (0/1) ");

	while (input > 1 || input < 0)
	{
		cout << "Only 0 or 1\n";
		cin >> input;
	}

	return (bool)input;
}

template<>
inline string Input<string>::Get(string hintForUser)
{
	cout << hintForUser << ":";
	string input;
	getline(cin, input);
	
	return input;
}