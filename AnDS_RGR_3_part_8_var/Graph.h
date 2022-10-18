#pragma once
#include "Edge.h"
#include "Vertex.h"
#include <vector>
#include <memory>

using namespace std;
#define IterOutOfRangeEx "Iterator out of range"

template <class Vertex, class Edge>
class Graph
{
	class GraphForm;

public:

#pragma region iterators

	class VertexesIterator
	{
	public:
		VertexesIterator(const Graph& graph) : vertexes(&graph.vertexes), graph(&graph) { it = vertexes->begin(); };
		Vertex& operator *() const
		{
			if (it == vertexes->end())
				throw IterOutOfRangeEx;
			return **it;
		}
		VertexesIterator Begin() const { return VertexesIterator(*graph); }
		VertexesIterator End() const { VertexesIterator iter(*graph); iter.it = graph->vertexes.end(); return iter; }
		bool operator++(int value)
		{
			if (it == vertexes->end())
				return false;
			it++;
			return it != vertexes->end();
		}
		bool operator==(VertexesIterator iter) { return graph == iter.graph && it == iter.it; }
		bool operator!=(VertexesIterator iter) { return !(*this == iter); }
	protected:
		const Graph* graph;
		const vector<Vertex*>* vertexes;
		typename vector<Vertex*>::const_iterator it;
	};

	class OutputEdgesIterator
	{
	public:
		OutputEdgesIterator(const Graph& graph, Vertex* vertex) : graph(&graph), vertex(vertex) { graph.form->FindNextEdge(vertex, &curEdge); };
		Edge& operator *() const
		{
			if (curEdge == nullptr)
				throw IterOutOfRangeEx;
			return *curEdge;
		}
		OutputEdgesIterator Begin() const { return OutputEdgesIterator(*graph, vertex); }
		OutputEdgesIterator End() const
		{
			OutputEdgesIterator iter(*graph, vertex);
			iter.curEdge = nullptr;
			return iter;
		}
		bool operator++(int value)
		{
			graph->form->FindNextEdge(vertex, &curEdge);
			return curEdge != nullptr;
		}
		bool operator==(OutputEdgesIterator iter) { return graph == iter.graph && vertex == iter.vertex && curEdge == iter.curEdge; }
		bool operator!=(OutputEdgesIterator iter) { return !(*this == iter); }
	protected:
		const Graph* graph;
		Vertex* vertex;
		Edge* curEdge = nullptr;
	};

	class EdgesIterator
	{
	public:
		EdgesIterator(const Graph& graph) : iterVertex(graph), graph(&graph), iterOutEdge(graph, nullptr)
		{
			if (graph.vertexes.size() != 0)
			{
				iterOutEdge = OutputEdgesIterator(graph, &(*iterVertex));
				while (iterOutEdge == iterOutEdge.End())
				{
					if (!(iterVertex++))
						break;

					iterOutEdge = OutputEdgesIterator(graph, &(*iterVertex));
				}
			}
		};
		Edge& operator *() const
		{
			return *iterOutEdge;
		}
		EdgesIterator Begin() const { return EdgesIterator(*graph); }
		EdgesIterator End() const { EdgesIterator it(*graph);  it.iterVertex = iterVertex.End();  it.iterOutEdge = iterOutEdge.End(); return it; }
		bool operator++(int value)
		{
			if (graph->IsDirected())
				return FindNextEdge();

			//для неориентированного графа ищем ребро, которого до этого не было 
			//т.к. при создании ребра v1->v2  создаётся его копия v2->v1

			do
			{
				if (FindNextEdge() == false)
					return false;
			} while ((*iterOutEdge).V2()->index < (*iterVertex).index);

			return true;
		}
		bool operator==(EdgesIterator iter) { return graph == iter.graph && iterVertex == iter.iterVertex && iterOutEdge == iter.iterOutEdge; }
		bool operator!=(EdgesIterator iter) { return !(*this == iter); }
	protected:
		bool FindNextEdge()
		{
			if (iterOutEdge++)
				return true;

			do
			{
				if (!(iterVertex++))
					return false;

				iterOutEdge = OutputEdgesIterator(*graph, &(*iterVertex));
			} while (iterOutEdge == iterOutEdge.End());

			return true;
		}
		const Graph* graph;
		VertexesIterator iterVertex;
		OutputEdgesIterator iterOutEdge;
	};

#pragma endregion

	enum class Form { L = 0, M = 1 };

	Graph() { form = new ListForm(); }
	Graph(int vertexes, bool isDirected, Form form);
	Graph(int vertexes, int randomEdges, bool isDirected, Form form);
	Graph(const Graph& graph);
	~Graph()
	{
		for (auto vertex : vertexes)
			delete vertex;
		delete form;
	}
	int GetVertexesCount() const { return vertexes.size(); }
	int GetEdgesCount() const { return form->GetEdgesCount(); }
	bool IsDirected() const { return form->IsDirected(); }
	float GetSaturation() const;
	Form GetForm() const { return curForm; }
	void SetForm(Form newForm);
	Vertex* AddVertex()
	{
		Vertex* newVertex = new Vertex();
		newVertex->index = vertexes.size();
		vertexes.push_back(newVertex);

		form->ReserveEdgesForVertex();
		return vertexes.back();
	}
	void RemoveVertex(Vertex* v)
	{
		form->RemoveEdgesOfVertex(v);

		auto it = std::find(vertexes.begin(), vertexes.end(), v);

		for (auto it2 = it + 1; it2 != vertexes.end(); it2++)
			(*it2)->index--;

		vertexes.erase(it);
		delete v;
	}
	Edge* AddEdge(Vertex* v1, Vertex* v2) { return (v1 == v2 ? nullptr : form->AddEdge(v1, v2)); }
	bool RemoveEdge(Vertex* v1, Vertex* v2) { return form->RemoveEdge(v1, v2); }
	Edge* GetEdge(Vertex* v1, Vertex* v2) { return form->GetEdge(v1, v2); };
	void Print() const { VertexesIterator it(*this);  form->Print(it); };

protected:

	Form curForm = Form::L;
	GraphForm* form = nullptr;
	vector<Vertex*> vertexes;

	GraphForm* CreateForm(Form form, bool isDirected);

#pragma region graphForms

	using ptrEdge = unique_ptr<Edge>;

	class GraphForm
	{
	public:
		GraphForm(bool isDirected = false) : isDirected(isDirected) {};
		bool IsDirected() const { return isDirected; }
		int GetEdgesCount() const { return edges; }
		virtual ~GraphForm() = default;
		virtual void ReserveEdgesForVertex() = 0;
		virtual void RemoveEdgesOfVertex(Vertex* v) = 0;
		virtual Edge* AddEdge(Vertex* v1, Vertex* v2) = 0;
		virtual bool RemoveEdge(Vertex* v1, Vertex* v2) = 0;
		virtual Edge* GetEdge(Vertex* v1, Vertex* v2) = 0;
		virtual void FindNextEdge(Vertex* v, Edge** current) const = 0;
		virtual void Print(VertexesIterator& iter) const = 0;
	protected:
		int vertexes = 0;
		int edges = 0;
		bool isDirected = false;
	};

	class MatrixForm : public GraphForm
	{
	public:
		MatrixForm(bool isDirected = false) : GraphForm(isDirected) {};
		~MatrixForm() {};
		virtual void ReserveEdgesForVertex();
		virtual void RemoveEdgesOfVertex(Vertex* v);
		virtual Edge* AddEdge(Vertex* v1, Vertex* v2);
		virtual bool RemoveEdge(Vertex* v1, Vertex* v2);
		virtual Edge* GetEdge(Vertex* v1, Vertex* v2) { return edges[v1->index][v2->index].get(); };
		virtual void FindNextEdge(Vertex* v, Edge** current) const;
		virtual void Print(VertexesIterator& iter) const;
	protected:
		vector<vector<ptrEdge>> edges;
		void IncreaseMatrix();
		bool IsEdgeExists(Vertex* v1, Vertex* v2);
	};

	class ListForm : public GraphForm
	{
	public:
		ListForm(bool isDirected = false) : GraphForm(isDirected) {};
		~ListForm() {};
		virtual void ReserveEdgesForVertex();
		virtual void RemoveEdgesOfVertex(Vertex* v);
		virtual Edge* AddEdge(Vertex* v1, Vertex* v2);
		virtual bool RemoveEdge(Vertex* v1, Vertex* v2);
		virtual Edge* GetEdge(Vertex* v1, Vertex* v2);
		virtual void FindNextEdge(Vertex* v, Edge** current) const;
		virtual void Print(VertexesIterator& iter) const;
	protected:
		vector<vector<ptrEdge>> edges;
		bool IsEdgeExists(Vertex* v1, Vertex* v2);
		typename vector<ptrEdge>::iterator FindEdgeWithV2(vector<ptrEdge>& row, Vertex* v2)
		{
			return std::find_if(row.begin(), row.end(), [v2](ptrEdge& ptr) { return ptr.get()->V2() == v2; });
		}
	};

#pragma endregion

	friend class VertexesIterator;
};

template<class Vertex, class Edge>
inline Graph<Vertex, Edge>::Graph(int vertexes, bool isDirected, Form form)
{
	curForm = form;
	this->form = CreateForm(form, isDirected);

	for (int i = 0; i < vertexes; i++)
		AddVertex();
}

template<class Vertex, class Edge>
inline Graph<Vertex, Edge>::Graph(int vertexes, int randomEdges, bool isDirected, Form form) : Graph(vertexes, isDirected, form)
{
	if (vertexes <= 1)
		return;

	int maxEdges = vertexes * (vertexes - 1) / 2;
	if (randomEdges > maxEdges)
		randomEdges = maxEdges;

	int addedEdges = 0;

	while (addedEdges < randomEdges)
	{
		int i = rand() % this->vertexes.size();

		int j = i;
		while (j == i)
			j = rand() % this->vertexes.size();

		if (AddEdge(this->vertexes[i], this->vertexes[j]) != nullptr)
			addedEdges++;
	}
}

template<class Vertex, class Edge>
inline Graph<Vertex, Edge>::Graph(const Graph& graph)
{
	this->form = CreateForm(graph.GetForm(), graph.IsDirected());
	curForm = graph.GetForm();

	if (graph.GetVertexesCount() == 0)
		return;

	VertexesIterator it(graph);
	while (it != it.End())
	{
		Vertex* vertex = AddVertex();
		*vertex = *(it);
		it++;
	}

	EdgesIterator it2(graph);

	while (it2 != it2.End())
	{
		Edge& edge = (*it2);

		Vertex* v1 = vertexes[edge.V1()->index];
		Vertex* v2 = vertexes[edge.V2()->index];

		Edge* newEdge = AddEdge(v1, v2);

		newEdge->CopyDataWeight(edge);
		it2++;
	}

}

template<class Vertex, class Edge>
inline float Graph<Vertex, Edge>::GetSaturation() const
{
	if (vertexes.size() <= 1)
		return 0;

	return (float)GetEdgesCount() / ((float)vertexes.size() * (vertexes.size() - 1) / 2);
}

template<class Vertex, class Edge>
inline void Graph<Vertex, Edge>::SetForm(Form newForm)
{
	if (newForm == curForm)
		return;

	curForm = newForm;

	GraphForm* _form = CreateForm(newForm, this->IsDirected());

	for (int i = 0; i < vertexes.size(); i++)
		_form->ReserveEdgesForVertex();

	EdgesIterator it(*this);

	if (vertexes.size() == 0)
		goto swap;

	while (it != it.End())
	{
		Edge& edge = *it;

		Vertex* v1 = vertexes[edge.V1()->index];
		Vertex* v2 = vertexes[edge.V2()->index];

		Edge* edgeCopy = _form->AddEdge(v1, v2);

		edgeCopy->CopyDataWeight(edge);
		it++;
	}

swap:
	delete this->form;
	form = _form;
}

template<class Vertex, class Edge>
inline typename Graph<Vertex, Edge>::GraphForm* Graph<Vertex, Edge>::CreateForm(Form form, bool isDirected)
{
	if (form == Form::L)
		return new ListForm(isDirected);
	else
		return new MatrixForm(isDirected);
}

#pragma region graphFormMethods

template<class Vertex, class Edge>
inline void Graph<Vertex, Edge>::MatrixForm::ReserveEdgesForVertex()
{
	IncreaseMatrix();
	GraphForm::vertexes++;
}

template<class Vertex, class Edge>
inline void Graph<Vertex, Edge>::MatrixForm::RemoveEdgesOfVertex(Vertex* v)
{
	edges.erase(edges.begin() + v->index);

	for (auto& row : edges)
		row.erase(row.begin() + v->index);

	GraphForm::vertexes--;
}

template<class Vertex, class Edge>
inline Edge* Graph<Vertex, Edge>::MatrixForm::AddEdge(Vertex* v1, Vertex* v2)
{
	if (IsEdgeExists(v1, v2))
		return nullptr;

	ptrEdge& edge1 = edges[v1->index][v2->index];
	ptrEdge& edge2 = edges[v2->index][v1->index];

	edge1 = make_unique<Edge>(v1, v2, 1);

	if (GraphForm::isDirected == false)
	{
		ptrEdge ptr(edge1.get()->CreateReversedCopy());
		edge2 = std::move(ptr);
	}

	GraphForm::edges++;
	return edge1.get();
}

template<class Vertex, class Edge>
inline bool Graph<Vertex, Edge>::MatrixForm::RemoveEdge(Vertex* v1, Vertex* v2)
{
	if (IsEdgeExists(v1, v2) == false)
		return false;

	ptrEdge& edge1 = edges[v1->index][v2->index];
	ptrEdge& edge2 = edges[v2->index][v1->index];

	edge1.reset();

	if (GraphForm::isDirected == false)
		edge2.reset();

	GraphForm::edges--;
	return true;
}

template<class Vertex, class Edge>
inline void Graph<Vertex, Edge>::MatrixForm::FindNextEdge(Vertex* v, Edge** current) const
{
	if (v == nullptr) return;

	int startIndex = 0;
	const vector<ptrEdge>& row = edges[v->index];

	if (*current != nullptr)
	{
		auto it = std::find_if(row.begin(), row.end(), [current](const ptrEdge& ptr) { return ptr.get() == *current; });
		startIndex = std::distance(row.begin(), it) + 1;
	}

	*current = nullptr;

	for (int i = startIndex; i < row.size(); i++)
	{
		if (row[i])
		{
			*current = row[i].get();
			return;
		}
	}
}

template<class Vertex, class Edge>
inline void Graph<Vertex, Edge>::MatrixForm::Print(VertexesIterator& iter) const
{
	if (iter == iter.End())
	{
		cout << "Graph is empty\n";
		return;
	}

	cout << "  ";

	for (int i = 0; i < GraphForm::vertexes; i++)
		cout << i << " ";

	cout << endl;

	while (iter != iter.End())
	{
		cout << (*iter).index << " ";

		for (int i = 0; i < GraphForm::vertexes; i++)
		{
			const ptrEdge& edge = edges[(*iter).index][i];
			cout << (edge ? 1 : 0) << " ";
		}

		cout << endl;
		iter++;
	}

	cout << "index-name | ";
	for (iter = iter.Begin(); iter != iter.End(); iter++)
	{
		cout << (*iter).index << "-" << (*iter).GetName() << " ";
	}
	cout << endl;
}

template<class Vertex, class Edge>
inline void Graph<Vertex, Edge>::MatrixForm::IncreaseMatrix()
{
	for (auto& row : edges)
		row.push_back(ptrEdge(nullptr));

	edges.push_back(vector<ptrEdge>(edges.size() + 1));
}

template<class Vertex, class Edge>
inline bool Graph<Vertex, Edge>::MatrixForm::IsEdgeExists(Vertex* v1, Vertex* v2)
{
	ptrEdge& edge1 = edges[v1->index][v2->index];
	ptrEdge& edge2 = edges[v2->index][v1->index];

	if (GraphForm::isDirected)
		return (edge1 || edge2);
	else
		return (edge1 && edge2);
}

template<class Vertex, class Edge>
inline void Graph<Vertex, Edge>::ListForm::ReserveEdgesForVertex()
{
	edges.push_back(vector<ptrEdge>());
	GraphForm::vertexes++;
}

template<class Vertex, class Edge>
inline void Graph<Vertex, Edge>::ListForm::RemoveEdgesOfVertex(Vertex* v)
{
	if (GraphForm::isDirected == false)
	{
		for (auto& edge : edges[v->index])
		{
			auto& row = edges[edge.get()->V2()->index];
			auto it = FindEdgeWithV2(row, v);
			row.erase(it);
		}
	}
	else
	{
		for (int i = 0; i < GraphForm::vertexes; i++)
		{
			if (i == v->index) continue;

			auto it = FindEdgeWithV2(edges[i], v);
			if (it != edges[i].end())
				edges[i].erase(it);
		}
	}

	edges.erase(edges.begin() + v->index);
	GraphForm::vertexes--;
}

template<class Vertex, class Edge>
inline bool Graph<Vertex, Edge>::ListForm::IsEdgeExists(Vertex* v1, Vertex* v2)
{
	if (FindEdgeWithV2(edges[v1->index], v2) != edges[v1->index].end())
		return true;

	if (GraphForm::isDirected)
	{
		return FindEdgeWithV2(edges[v2->index], v1) != edges[v2->index].end();
	}
	return false;
}

template<class Vertex, class Edge>
inline Edge* Graph<Vertex, Edge>::ListForm::AddEdge(Vertex* v1, Vertex* v2)
{
	vector<ptrEdge>& row = edges[v1->index];

	if (IsEdgeExists(v1, v2))
		return nullptr;

	row.push_back(make_unique<Edge>(v1, v2, 1));

	if (GraphForm::isDirected == false)
	{
		ptrEdge ptr(row.back().get()->CreateReversedCopy());
		edges[v2->index].push_back(std::move(ptr));
	}

	GraphForm::edges++;
	return row.back().get();
}

template<class Vertex, class Edge>
inline bool Graph<Vertex, Edge>::ListForm::RemoveEdge(Vertex* v1, Vertex* v2)
{
	auto it = FindEdgeWithV2(edges[v1->index], v2);

	if (it == edges[v1->index].end()) return false;

	edges[v1->index].erase(it);

	if (GraphForm::isDirected == false)
	{
		auto it = FindEdgeWithV2(edges[v2->index], v1);
		edges[v2->index].erase(it);
	}

	GraphForm::edges--;
	return true;
}

template<class Vertex, class Edge>
inline Edge* Graph<Vertex, Edge>::ListForm::GetEdge(Vertex* v1, Vertex* v2)
{
	auto it = FindEdgeWithV2(edges[v1->index], v2);
	if (it == edges[v1->index].end()) return nullptr;

	return (*it).get();
}

template<class Vertex, class Edge>
inline void Graph<Vertex, Edge>::ListForm::FindNextEdge(Vertex* v, Edge** current) const
{
	if (v == nullptr) return;

	const vector<ptrEdge>& row = edges[v->index];

	if (*current == nullptr)
	{
		*current = (row.size() == 0 ? nullptr : row.front().get());
		return;
	}

	auto it = std::find_if(row.begin(), row.end(), [current](const ptrEdge& ptr) { return ptr.get() == *current;  });

	if (*it == row.back())
		*current = nullptr;
	else
	{
		it++;
		*current = (*it).get();
	}
}

template<class Vertex, class Edge>
inline void Graph<Vertex, Edge>::ListForm::Print(VertexesIterator& iter) const
{
	if (iter == iter.End())
	{
		cout << "Graph is empty\n";
		return;
	}

	while (iter != iter.End())
	{
		if ((*iter).HasName())
			cout << (*iter).GetName() << ": ";
		else
			cout << "No name: ";

		for (int i = 0; i < edges[(*iter).index].size(); i++)
		{
			const ptrEdge& ptr = edges[(*iter).index][i];
			cout << ptr.get()->V2()->GetName() << " ";
		}

		cout << endl;
		iter++;
	}
}

#pragma endregion