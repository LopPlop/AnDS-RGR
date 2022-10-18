#pragma once
#include "Graph.h"
#include "Input.h"
#include <algorithm>

#define FirstTask Task1v3
#define SecondTask Task2v8

template <class Vertex, class Edge>
class Task
{
public:
	Task(Graph<Vertex, Edge>& graph) : graph(&graph) { }
	void Set(Graph<Vertex, Edge>& graph) { this->graph = &graph; Restart(); }
	virtual ~Task() = default;
	virtual void Restart() = 0;
	virtual void Result() = 0;
protected:
	Graph<Vertex, Edge>* graph;
};

#pragma region Task1

string task1Description = "Определение гамильтонова цикла в орграфе";

template <class Vertex, class Edge>
class Task1v3 : public Task<Vertex, Edge>
{
public:
	Task1v3(Graph<Vertex, Edge>& graph) : Task<Vertex, Edge>(graph)
	{
		if (graph.IsDirected() == false)
			throw "Граф должен быть ориентированным";
		Restart();
	};
	virtual void Restart();
	virtual void Result();
protected:

	vector<Vertex> result;
	vector<Vertex> SavePath(const vector<Vertex*>& path);
	void GetAllNeighboringVertexes(Vertex* vertex, vector<Vertex*>& neighbors);
	bool Hamilton(vector<Vertex*> curPath, vector<bool> viewed);
};

template<class Vertex, class Edge>
inline void Task1v3<Vertex, Edge>::Result()
{
	if (result.size() == 0)
	{
		cout << "Цикл не найден\n";
		return;
	}

	for (auto elem : result)
	{
		cout << elem.GetName() << "->";
	}

	cout << result.front().GetName() << endl;
}

template<class Vertex, class Edge>
inline void Task1v3<Vertex, Edge>::Restart()
{
	result.clear();

	if (Task<Vertex, Edge>::graph->GetVertexesCount() <= 1)
		return;

	vector<Vertex*> path;

	typename Graph<Vertex, Edge>::VertexesIterator iter(*Task<Vertex, Edge>::graph);
	path.push_back(&(*iter));

	vector<bool> viewed(Task<Vertex, Edge>::graph->GetVertexesCount(), false);
	Hamilton(path, viewed);
}

template<class Vertex, class Edge>
inline bool Task1v3<Vertex, Edge>::Hamilton(vector<Vertex*> curPath, vector<bool> viewed)
{
	viewed[curPath.back()->index] = true;

	vector<Vertex*> neighbors;
	GetAllNeighboringVertexes(curPath.back(), neighbors);

	for (auto curVertex : neighbors)
	{
		if (curVertex == curPath.front() && curPath.size() == viewed.size())
		{
			result = SavePath(curPath);
			return true;
		}

		if (viewed[curVertex->index]) continue;

		vector<Vertex*> newPath = curPath;
		newPath.push_back(curVertex);
		if (Hamilton(newPath, viewed))
			return true;
	}

	return false;
}

template<class Vertex, class Edge>
inline void Task1v3<Vertex, Edge>::GetAllNeighboringVertexes(Vertex* vertex, vector<Vertex*>& neighbors)
{
	typename Graph<Vertex, Edge>::OutputEdgesIterator iter(*Task<Vertex, Edge>::graph, vertex);

	while (iter != iter.End())
	{
		neighbors.push_back((*iter).V2());
		iter++;
	}
}

template<class Vertex, class Edge>
inline vector<Vertex> Task1v3<Vertex, Edge>::SavePath(const vector<Vertex*>& path)
{
	vector<Vertex> savedPath;

	for (auto elem : path)
		savedPath.push_back(*elem);

	return savedPath;
}
#pragma endregion

#pragma region Task2

string task2Description = "Определение первого по величине (по количеству вершин) кластера в неориентированном взвешенном графе, вершины которого объединены ребрами с длиной, большей d";

template <class Vertex, class Edge>
class Task2v8 : public Task<Vertex, Edge>
{
public:
	Task2v8(Graph<Vertex, Edge>& graph) : Task<Vertex, Edge>(graph)
	{
		if (graph.IsDirected())
			throw "Граф должен быть неориентированным";
		Restart();
	};
	virtual void Restart();
	virtual void Result();
protected:
	vector<string> result;
	void Prim(Vertex* start, const vector<Vertex*>& vertexes, int minWeight);
	vector<Vertex*> GetAllVertexes();
	void FindCluster(Vertex* start, vector<Edge*>& edges, vector<Edge>& cluster, int minWeight);
};

template<class Vertex, class Edge>
inline void Task2v8<Vertex, Edge>::Result()
{
	if (result.empty())
	{
		cout << "Кластер не найден\n";
		return;
	}

	cout << "Вершины кластера:\n";

	for (auto name : result)
	{
		cout << name << endl;
	}
}

template<class Vertex, class Edge>
inline void Task2v8<Vertex, Edge>::Restart()
{
	result.clear();

	vector<Vertex*> vertexes = GetAllVertexes();

	if (vertexes.empty())
		return;
	//111111111111
	Prim(vertexes.front(), vertexes, Input<int>::Get("Длина")-1);
	
}

template<class Vertex, class Edge>
inline vector<Vertex*> Task2v8<Vertex, Edge>::GetAllVertexes()
{
	vector<Vertex*> vertexes;
	typename Graph<Vertex, Edge>::VertexesIterator iter(*Task<Vertex, Edge>::graph);

	while (iter != iter.End())
	{
		vertexes.push_back(&(*iter));
		iter++;
	}

	return vertexes;
}

template<class Vertex, class Edge>
inline void Task2v8<Vertex, Edge>::FindCluster(Vertex* start, vector<Edge*>& edges, vector<Edge>& clusterEdges, int minWeight)
{
	vector<bool> cluster(Task<Vertex, Edge>::graph->GetVertexesCount(), false);
	cluster[start->index] = true;

	bool addedInCluster;

	edges.erase(remove_if(edges.begin(), edges.end(),
		[](auto x) { return x == nullptr; }), edges.end());

	do
	{
		addedInCluster = false;

		for (int i = 0; i < edges.size(); i++)
		{
			int id1 = edges[i]->V1()->index;
			int id2 = edges[i]->V2()->index;

			if (cluster[id1] && cluster[id2]) continue;
			if (!cluster[id1] && !cluster[id2]) continue;

			Edge edge = Edge(edges[i]->V1(), edges[i]->V2(), edges[i]->GetWeight());
			clusterEdges.push_back(edge);

			if (edges[i]->HasData())
				clusterEdges.back().SetData(edges[i]->GetData());

			cluster[id1] = true;
			cluster[id2] = true;
			edges.erase(edges.begin() + i);
			addedInCluster = true;
			break;
		}

	} while (addedInCluster);

	if (clusterEdges.empty()) return;

	auto edgeValid = [minWeight](Edge* edge) { if (edge == nullptr) return true; return edge->GetWeight() > minWeight; };
	auto connectionsCount = [&clusterEdges](int vIndex)
	{
		int c = 0;
		for (auto edge : clusterEdges)
			if (edge.V1()->index == vIndex || edge.V2()->index == vIndex)  c++;
		return c;
	};

	vector<Vertex*> vertexes = GetAllVertexes();

	for (int i = 0; i < cluster.size(); i++)
	{
		for (int j = 0; j < cluster.size(); j++)
		{
			if (!cluster[i] || !cluster[j]) continue;

			Edge* edge = Task<Vertex, Edge>::graph->GetEdge(vertexes[i], vertexes[j]);

			if (edgeValid(edge)) continue;

			if (connectionsCount(i) > connectionsCount(j))
				cluster[j] = false;
			else
				cluster[i] = false;
		}
	}

	vector<string> resultVertexes;

	for (int i = 0; i < cluster.size(); i++)
	{
		if (cluster[i])
		{
			resultVertexes.push_back(vertexes[i]->GetName());
		}
	}

	if (result.size() < resultVertexes.size())
		result = resultVertexes;
}

template<class Vertex, class Edge>
inline void Task2v8<Vertex, Edge>::Prim(Vertex* start, const vector<Vertex*>& vertexes, int minWeight)
{
	using keyVertex = pair<int, Vertex*>;

	vector<keyVertex> queue;
	vector<Edge*> p;

	for (int i = 0; i < vertexes.size(); i++)
	{
		queue.push_back({ INT_MAX, vertexes[i] });
		p.push_back(nullptr);
	}

	queue[start->index].first = 0;

	auto less = [](const keyVertex& p1, const keyVertex& p2) { return p1.first < p2.first;  };
	sort(queue.begin(), queue.end(), less);

	while (queue.empty() == false)
	{
		keyVertex vertex = queue.front();
		queue.erase(queue.begin());

		for (typename Graph<Vertex, Edge>::OutputEdgesIterator iter(*Task<Vertex, Edge>::graph, vertex.second); iter != iter.End(); iter++)
		{
			Edge& edge = *iter;

			if (edge.HasWeight() == false)
				throw "Граф не взвешен";

			auto findV2 = [edge](const keyVertex& p) { return p.second == edge.V2(); };

			auto it = std::find_if(queue.begin(), queue.end(), findV2);
			if (it == queue.end())
				continue;

			if ((*it).first > edge.GetWeight() && edge.GetWeight() > minWeight)
			{
				p[edge.V2()->index] = &edge;
				(*it).first = edge.GetWeight();
			}

		}

		sort(queue.begin(), queue.end(), less);

	}

	p.erase(remove_if(p.begin(), p.end(),
		[](auto x) { return x == nullptr; }), p.end());

	while (p.empty() == false)
	{
		vector<Edge> clusterEdges;
		FindCluster(p.front()->V1(), p, clusterEdges, minWeight);
	}
}

#pragma endregion