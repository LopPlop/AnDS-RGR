#pragma once

template <class Name, class Data>
class Vertex
{
public:
	Vertex() {};
	Vertex(Name name, Data data) { SetName(name); SetData(data); };

	Name GetName() const { return name; }
	void SetName(Name name) { hasName = true; this->name = name; }
	bool HasName() const { return hasName; }

	Data GetData() const { return data; }
	void SetData(Data data) { hasData = true; this->data = data; }
	bool HasData() const { return hasData; }

	void Print() const;

	int index = -1;
protected:

	bool hasName = false;
	bool hasData = false;

	Name name;
	Data data;
};

template<class Name, class Data>
inline void Vertex<Name, Data>::Print() const
{
	if (hasName)
		std::cout << name;
	else
		std::cout << "no name";

	std::cout << " |";

	if (hasData)
		std::cout << data;
	else
		std::cout << "no data";
}