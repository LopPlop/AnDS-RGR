#pragma once
#include "Graph.h"
#include "Task.h"
#include <algorithm>
#include <queue>


	string taskOneDescription = "Построение остовного дерева орграфа в пределах заданной высоты за счёт добавления минимального числа рёбер";

	template <class Vertex, class Edge>
	class Task1v6 : public Task<Vertex, Edge>
	{
	public:
		Task1v6(Graph<Vertex, Edge>& graph) : Task<Vertex, Edge>(graph)
		{
			if (graph.IsDirected() == false)
				throw "Граф должен быть ориентированным";
			Restart();
		};
		virtual void Restart();
		virtual void Result();
	protected:
		vector<pair<string, string>> result;
		vector<Vertex*> GetAllVertexes();
		void GetAllNeighboringVertexes(Vertex* vertex, vector<Vertex*>& neighbors);
		Vertex* FindRootForTree(const vector<vector<int>>& weights, vector<Vertex*>& vertexes, int maxHeight);
		bool FindTree(Vertex* start, vector<Vertex*>& vertexes, vector<int>& vertexHeight, int maxHeight);
		bool ConnectWithChangingHeight(Vertex* vertex, vector<int>& vertexHeight);
		void Floyd(vector<vector<int>>& weights);
		vector<vector<int>> GetPathMatrix();
	};

	template<class Vertex, class Edge>
	inline void Task1v6<Vertex, Edge>::Result()
	{
		if (result.empty())
		{
			cout << "Не удалось построить дерево\n";
			return;
		}

		cout << "Рёбра дерева:\n";

		for (auto edge : result)
		{
			cout << edge.first << "->" << edge.second << endl;
		}
	}

	template<class Vertex, class Edge>
	inline void Task1v6<Vertex, Edge>::Restart()
	{
		result.clear();

		vector<Vertex*> vertexes = GetAllVertexes();
		if (vertexes.size() < 1)
			return;

		int maxHeight = Input<int>::Get("Максимальная высота");
		if (maxHeight < 1)
			return;

		vector<vector<int>> weights = GetPathMatrix();
		Floyd(weights);

		vector<int> vertexHeight(vertexes.size(), -1);

		vector<Vertex*> roots;
		vector<Vertex*> leftVertexes;

		do
		{
			leftVertexes.clear();

			if (!roots.empty())
			{
				for (int i = 0; i < vertexHeight.size(); i++)
				{
					if (vertexHeight[i] == -1)
						leftVertexes.push_back(vertexes[i]);
				}
			}
			else
			{
				leftVertexes = vertexes;
			}

			if (leftVertexes.empty()) break;

			int curMaxHeight = (roots.empty() ? maxHeight : maxHeight - 1);
			roots.push_back(FindRootForTree(weights, leftVertexes, curMaxHeight));

			if (roots.size() > 1)
			{
				vertexHeight[roots.back()->index] = vertexes.size() + 2;
				if (!ConnectWithChangingHeight(roots.back(), vertexHeight))
				{
					result.clear();
					break;
				}
			}
			else
				vertexHeight[roots.back()->index] = 0;

			if (!FindTree(roots.back(), vertexes, vertexHeight, maxHeight))
			{
				result.clear();
				break;
			}

		} while (true);

	}

	template<class Vertex, class Edge>
	inline vector<Vertex*> Task1v6<Vertex, Edge>::GetAllVertexes()
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
	inline void Task1v6<Vertex, Edge>::GetAllNeighboringVertexes(Vertex* vertex, vector<Vertex*>& neighbors)
	{
		typename Graph<Vertex, Edge>::OutputEdgesIterator iter(*Task<Vertex, Edge>::graph, vertex);

		while (iter != iter.End())
		{
			neighbors.push_back((*iter).V2());
			iter++;
		}
	}

	template<class Vertex, class Edge>
	inline Vertex* Task1v6<Vertex, Edge>::FindRootForTree(const vector<vector<int>>& weights, vector<Vertex*>& leftVertexes, int maxHeight)
	{
		int maxValidPaths = -1;
		Vertex* root = leftVertexes.front();

		vector<int> inEdges(weights.size(), 0);

		for (auto v1 : leftVertexes)
		{
			int count = 0;

			for (auto v2 : leftVertexes)
			{
				if (weights[v1->index][v2->index] == INT_MAX) continue;
				if (weights[v1->index][v2->index] <= maxHeight)
				{
					count++;
					if (maxHeight == 1) inEdges[v2->index]++;
				}
			}

			if (count > maxValidPaths)
			{
				root = v1;
				maxValidPaths = count;
			}
		}

		if (maxHeight == 1)
		{
			for (auto v : leftVertexes)
			{
				if (inEdges[v->index] == 0)
					return v;
			}
		}

		return root;
	}

	template<class Vertex, class Edge>
	inline bool Task1v6<Vertex, Edge>::FindTree(Vertex* start, vector<Vertex*>& vertexes, vector<int>& vertexHeight, int maxHeight)
	{
		if (maxHeight == 0) return true;

		vector<Vertex*> vertexesInTree;
		vector<Vertex*> vertexesToHandle;

		vertexesToHandle.push_back(start);

		bool addedEdge = false;
		do
		{
			addedEdge = false;
			vector<Vertex*> _vertexesToHandle;

			for (auto v : vertexesToHandle)
			{
				vertexesInTree.push_back(v);

				vector<Vertex*> neighbors;
				GetAllNeighboringVertexes(v, neighbors);

				neighbors.erase(std::remove_if(neighbors.begin(), neighbors.end(),
					[vertexHeight](Vertex* v) { return vertexHeight[v->index] != -1; }), neighbors.end());

				if ((vertexHeight[v->index] == maxHeight && maxHeight != 1 && neighbors.size() > 0) || vertexHeight[v->index] > maxHeight)
				{
					if (!ConnectWithChangingHeight(v, vertexHeight))
						return false;

					auto it = std::find_if(result.begin(), result.end(), [v](pair<string, string>& p) { return p.second == v->GetName();  });
					result.erase(it);
				}

				for (auto n : neighbors)
				{
					vertexHeight[n->index] = vertexHeight[v->index] + 1;
					result.push_back({ v->GetName(), n->GetName() });

					addedEdge = true;
					_vertexesToHandle.push_back(n);
				}

			}

			vertexesToHandle = _vertexesToHandle;

		} while (addedEdge);

		return true;
	}

	template<class Vertex, class Edge>
	inline bool Task1v6<Vertex, Edge>::ConnectWithChangingHeight(Vertex* vertex, vector<int>& vertexHeight)
	{
		vector<Vertex*> vertexes = GetAllVertexes();

		int minHeightId = -1;
		for (int i = 0; i < vertexHeight.size(); i++)
		{
			if (vertexHeight[i] < 0 || vertexHeight[i] + 1 >= vertexHeight[vertex->index]) continue;

			int minHeight = (minHeightId == -1 ? vertexes.size() + 2 : vertexHeight[minHeightId]);

			if (vertexHeight[i] < minHeight && Task<Vertex, Edge>::graph->GetEdge(vertex, vertexes[i]) == nullptr)
			{
				minHeightId = i;
			}
		}

		if (minHeightId < 0)
			return false;

		vertexHeight[vertex->index] = vertexHeight[minHeightId] + 1;
		result.push_back({ vertexes[minHeightId]->GetName(), vertex->GetName() });
		return true;
	}

	template<class Vertex, class Edge>
	inline void Task1v6<Vertex, Edge>::Floyd(vector<vector<int>>& weights)
	{
		for (int i = 0; i < weights.size(); i++)
		{
			for (int j = 0; j < weights.size(); j++)
			{
				for (int s = 0; s < weights.size(); s++)
				{
					int possibleMin = weights[j][i] + weights[i][s];
					if (weights[j][i] == numeric_limits<int>::max() || weights[i][s] == numeric_limits<int>::max())
						possibleMin = numeric_limits<int>::max();

					weights[j][s] = std::min(weights[j][s], possibleMin);
				}
			}
		}
	}

	template<class Vertex, class Edge>
	inline vector<vector<int>> Task1v6<Vertex, Edge>::GetPathMatrix()
	{
		int vertexes = Task<Vertex, Edge>::graph->GetVertexesCount();

		vector<vector<int>> weights;

		for (int i = 0; i < vertexes; i++)
		{
			weights.push_back(vector<int>());
			for (int j = 0; j < vertexes; j++)
				weights[i].push_back(numeric_limits<int>::max());
		}

		typename Graph<Vertex, Edge>::EdgesIterator iter(*Task<Vertex, Edge>::graph);

		while (iter != iter.End())
		{
			Vertex* v1 = (*iter).V1();
			Vertex* v2 = (*iter).V2();

			weights[v1->index][v2->index] = 1;

			if (Task<Vertex, Edge>::graph->IsDirected() == false)
				weights[v2->index][v1->index] = 1;

			iter++;
		}

		return weights;
	}

