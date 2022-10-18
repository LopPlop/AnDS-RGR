#pragma once
#include <memory>

template <class Vertex, class Weight, class Data>
class Edge
{
public:
	Edge();
	Edge(Vertex* v1, Vertex* v2, Weight weight) : Edge() { this->v1 = v1; this->v2 = v2; SetWeight(weight); }
	Edge(Vertex* v1, Vertex* v2, Weight weight, Data data) : Edge(v1, v1, weight) { SetData(data); }

	Weight GetWeight() const { return *weight; };
	void SetWeight(Weight newW) { *hasWeight = true; *weight = newW; }
	bool HasWeight() const { return *hasWeight; }
	Data GetData() const { return *data; };
	void SetData(Data newD) { *hasData = true; *data = newD; }
	bool HasData() const { return *hasData; }
	Vertex* V1() const { return v1; };
	Vertex* V2() const { return v2; };
	Edge* CreateReversedCopy();
	void Print() const;
	void CopyDataWeight(const Edge& edge);

protected:
	std::shared_ptr<bool> hasWeight;
	std::shared_ptr<bool> hasData;

	std::shared_ptr<Weight> weight;
	std::shared_ptr<Data> data;

	Vertex* v1 = nullptr, * v2 = nullptr;
};

template<class Vertex, class Weight, class Data>
inline Edge<Vertex, Weight, Data>::Edge()
{
	weight = std::make_shared<Weight>();
	data = std::make_shared<Data>();
	hasWeight = std::make_shared<bool>(false);
	hasData = std::make_shared<bool>(false);
}

template<class Vertex, class Weight, class Data>
inline typename Edge<Vertex, Weight, Data>* Edge<Vertex, Weight, Data>::CreateReversedCopy()
{
	Edge* newEdge = new Edge();
	newEdge->v1 = this->v2;
	newEdge->v2 = this->v1;
	newEdge->weight = this->weight;
	newEdge->data = this->data;
	newEdge->hasWeight = this->hasWeight;
	newEdge->hasData = this->hasData;
	return newEdge;
}

template<class Vertex, class Weight, class Data>
inline void Edge<Vertex, Weight, Data>::Print() const
{
	std::cout << v1->GetName() << "->" << v2->GetName() << ": ";

	if (*hasData)
		std::cout << "data:" << *data;
	else
		std::cout << "no data ";

	if (*hasWeight)
		std::cout << "weight:" << *weight;
	else
		std::cout << "no weight ";
}

template<class Vertex, class Weight, class Data>
inline void Edge<Vertex, Weight, Data>::CopyDataWeight(const Edge& edge)
{
	if (edge.HasData())
		SetData(edge.GetData());
	if (edge.HasWeight())
		SetWeight(edge.GetWeight());
}
