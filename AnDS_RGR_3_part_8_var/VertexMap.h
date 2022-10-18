#pragma once
#include "Graph.h"
#include <map>
#include <string>

using namespace std;

template <class Name, class Data = Vertex<Name, typename class T>*>
class VertexMap
{
public:
	VertexMap() {};
	bool Add(Name name, Data vertex);
	Data Get(Name name);
	bool SwitchName(Name oldName, Name newName);
	bool Remove(Name name);
	void Clear() { map.clear(); }

protected:
	std::map<Name, Data> map;
};

template<class Name, class Data>
inline bool VertexMap<Name, Data>::Add(Name name, Data vertex)
{
	if (Get(name) != nullptr)
		return false;

	map.insert(std::pair<Name, Data>{name, vertex});
	return true;
}

template<class Name, class Data>
inline Data VertexMap<Name, Data>::Get(Name name)
{
	auto it = map.find(name);
	return (it == map.end() ? nullptr : (*it).second);
}

template<class Name, class Data>
inline bool VertexMap<Name, Data>::SwitchName(Name oldName, Name newName)
{

	Data v1 = Get(oldName);
	Data v2 = Get(newName);
	if (v1 == nullptr || v2 != nullptr) return false;

	v1->SetName(newName);

	map.erase(map.find(oldName));
	std::pair<Name, Data> p(newName, v1);
	map.insert(p);
	return true;
}

template<class Name, class Data>
inline bool VertexMap<Name, Data>::Remove(Name name)
{
	auto it = map.find(name);
	if (it == map.end())
		return false;

	map.erase(it);
	return true;
}