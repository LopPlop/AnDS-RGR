#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "Graph.h"
#include "VertexMap.h"
#include "Input.h"
#include "Task.h"
#include "PartOne.h"

using namespace std;

using commandView = pair<int, string>;
using _Vertex = Vertex<string, int>;
using _Edge = Edge<_Vertex, int, string>;
using RGZGraph = Graph<_Vertex, _Edge>;

#define SUCCESS_INPUT 0
#define BACK_TO_MENU -1

RGZGraph* graph = new RGZGraph();
VertexMap<string, _Vertex*> vertexMap;
RGZGraph::VertexesIterator vIterator(*graph);
RGZGraph::EdgesIterator eIterator(*graph);
RGZGraph::OutputEdgesIterator outEIterator(*graph, nullptr);

Task1v6<_Vertex, _Edge>* task1 = nullptr;
SecondTask<_Vertex, _Edge>* task2 = nullptr;





void printCommands(vector<commandView> commandsView)
{
	auto printCommand = [](commandView view)
	{
		cout << (int)view.first << " - " << view.second;
	};
	

	for (int i = 0, j = commandsView.size() / 2; i < commandsView.size() / 2; i++, j++)
	{
		printCommand(commandsView[i]);

		int k = (int)commandsView[i].first / 10;
		int len = 1;

		while (k != 0)
		{
			len++; k /= 10;
		}

		cout << string(70 - len - commandsView[i].second.length(), ' ');
		printCommand(commandsView[j]);

		cout << endl;
	}

	if (commandsView.size() % 2 != 0)
	{
		printCommand(commandsView.back());
		cout << endl;
	}

}

bool getVertex(_Vertex** vertex, string hint)
{
	*vertex = vertexMap.Get(Input<string>::Get(hint));

	if (*vertex == nullptr)
	{
		cout << "Вершина не найдена\n";
		return false;
	}
	return true;
}

bool getTwoVertixes(_Vertex** array)
{
	for (int i = 0; i < 2; i++)
	{
		if (!getVertex(&array[i], "Имя вершины #" + to_string(i + 1)))
			return false;
	}
	return true;
}

bool getEdge(_Edge** edge)
{
	_Vertex* vertexes[2];

	if (!getTwoVertixes(vertexes))
		return false;

	*edge = graph->GetEdge(vertexes[0], vertexes[1]);

	if (*edge == nullptr)
	{
		cout << "Ребро не найдено\n";
		return false;
	}

	return true;
}

void setNamesToAllVertexes()
{
	typename Graph<_Vertex, _Edge>::VertexesIterator iter(*graph);
	vector<int> code;
	code.push_back(0);

	while (iter != iter.End())
	{
		string name;

		for (int i = 0; i < code.size(); i++)
			name.insert(name.end(), (char)(code[i] + 97));

		(*iter).SetName(name);

		vertexMap.Add(name, &(*iter));

		if (code.back() == 26)
			code.push_back(0);
		else
			code.back()++;

		iter++;
	}
}

void updateGraphHelpers()
{
	vIterator = RGZGraph::VertexesIterator(*graph);
	eIterator = RGZGraph::EdgesIterator(*graph);
	outEIterator = RGZGraph::OutputEdgesIterator(*graph, nullptr);
	vertexMap.Clear();
}

class CommandsHandler
{
public:
	virtual int HandleInput() = 0;
protected:
	virtual void PrintCommands() = 0;
};

class GraphCommands : public CommandsHandler
{
public:
	GraphCommands() { PrintCommands(); }

	enum class Command {
		Create, CreateVDF, CreateVEDF, IsDirected, GetSaturation, GetForm, SetForm,
		GetEdgesCount, AddEdge, GetEdge, SetEdgeData, GetEdgeData, SetEdgeWeight, GetEdgeWeight,
		RemoveEdge, GetVertexesCount, AddVertex, SetVertexData, GetVertexData, SwitchName, RemoveVertex,
		Print, PrintCommands, ReturnToMenu
	};

	int HandleInput()
	{
		int input = Input<int>::Get("Номер команды");

		switch (input)
		{
		case (int)Command::Create:
			delete graph;
			graph = new RGZGraph();
			updateGraphHelpers();
			break;
		case (int)Command::CreateVDF:
		{
			delete graph;
			RGZGraph::Form form = (RGZGraph::Form)(Input<bool>::Get("Форма графа L/M"));
			graph = new RGZGraph(Input<int>::Get("Количество вершин"), Input<bool>::Get("Направленный ли граф"), form);
			updateGraphHelpers();
			setNamesToAllVertexes();
			break;
		}
		case (int)Command::CreateVEDF:
		{
			delete graph;
			RGZGraph::Form form = (RGZGraph::Form)(Input<bool>::Get("Форма графа L/M"));
			graph = new RGZGraph(Input<int>::Get("Количество вершин"), Input<int>::Get("Количество случайных рёбер"), Input<bool>::Get("Направленный ли граф"), form);
			updateGraphHelpers();
			setNamesToAllVertexes();
			break;
		}
		case (int)Command::PrintCommands:
			PrintCommands();
			break;
		case (int)Command::ReturnToMenu:
			return BACK_TO_MENU;
		case (int)Command::IsDirected:
			cout << graph->IsDirected() << endl;
			break;
		case (int)Command::GetForm:
			cout << (graph->GetForm() == RGZGraph::Form::L ? "L\n" : "M\n");
			break;
		case (int)Command::Print:
			graph->Print();
			break;
		case (int)Command::SetForm:
			graph->SetForm(RGZGraph::Form(Input<bool>::Get("Форма графа (L/M)")));
			break;
		case (int)Command::AddVertex:
		{
			string name = Input<string>::Get("Имя вершины");
			_Vertex* v = vertexMap.Get(name);
			if (v != nullptr)
			{
				cout << "Вершина с таким именем уже существует\n";
				break;
			}
			v = graph->AddVertex();
			v->SetName(name);
			vertexMap.Add(name, v);
			break;
		}
		case (int)Command::RemoveVertex:
		{
			string name = Input<string>::Get("Имя вершины");
			_Vertex* v = vertexMap.Get(name);
			if (v == nullptr)
			{
				cout << "Вершины с таким именем не существует\n";
				break;
			}

			graph->RemoveVertex(v);
			vertexMap.Remove(name);
			break;
		}
		case (int)Command::AddEdge:
		{
			_Vertex* vertexes[2];
			if (!getTwoVertixes(vertexes))
				break;

			_Edge* edge = graph->AddEdge(vertexes[0], vertexes[1]);
			cout << (edge == nullptr ? "Fail\n" : "Success\n");
			break;
		}
		case (int)Command::RemoveEdge:
		{
			_Vertex* vertexes[2];
			if (!getTwoVertixes(vertexes))
				break;

			cout << graph->RemoveEdge(vertexes[0], vertexes[1]) << endl;
			break;
		}
		case (int)Command::GetEdge:
		{
			_Edge* edge; if (!getEdge(&edge)) break;
			edge->Print();
			cout << endl;
			break;
		}
		case (int)Command::SetEdgeData:
		{
			_Edge* edge; if (!getEdge(&edge)) break;
			edge->SetData(Input<string>::Get("Данные"));
			break;
		}
		case (int)Command::GetEdgeData:
		{
			_Edge* edge; if (!getEdge(&edge)) break;
			if (edge->HasData())
				cout << edge->GetData() << endl;
			else
				cout << "Данные не заданы\n";
			break;
		}
		case (int)Command::SetEdgeWeight:
		{
			_Edge* edge; if (!getEdge(&edge)) break;
			edge->SetWeight(Input<int>::Get("Вес"));
			break;
		}
		case (int)Command::GetEdgeWeight:
		{
			_Edge* edge; if (!getEdge(&edge)) break;
			if (edge->HasWeight())
				cout << edge->GetWeight() << endl;
			else
				cout << "Вес не задан\n";;
			break;
		}
		case (int)Command::SetVertexData:
		{
			_Vertex* vertex;
			if (getVertex(&vertex, "Имя вершины") == false)
				break;

			vertex->SetData(Input<int>::Get("Данные"));
			break;
		}
		case (int)Command::GetVertexData:
		{
			_Vertex* vertex;
			if (getVertex(&vertex, "Имя вершины") == false)
				break;

			if (vertex->HasData())
				cout << vertex->GetData() << endl;
			else
				cout << "Данные не заданы\n";
			break;
		}
		case (int)Command::GetEdgesCount:
			cout << graph->GetEdgesCount() << endl;
			break;
		case (int)Command::GetVertexesCount:
			cout << graph->GetVertexesCount() << endl;
			break;
		case (int)Command::GetSaturation:
			cout << graph->GetSaturation() << endl;
			break;
		case (int)Command::SwitchName:
			cout << vertexMap.SwitchName(Input<string>::Get("Старое имя вершины"), Input<string>::Get("Новое имя вершины")) << endl;
			break;
		default:
			cout << "Неверный номер команды\n";
		};

		return SUCCESS_INPUT;
	}

protected:

	vector<commandView> commands = {
	{(int)Command::Create, "Создать путой L-граф"},
	{(int)Command::CreateVDF, "Создать граф и задать кол-во вершин, тип, форму"},
	{(int)Command::CreateVEDF, "Создать граф и задать кол-во вершин и случ. рёбер, тип, форму"},
	{(int)Command::IsDirected, "Ориентированный ли граф"},
	{(int)Command::GetSaturation, "Коэф. насыщенности"},
	{(int)Command::GetForm, "Получить форму"},
	{(int)Command::SetForm, "Задать форму"},
	{(int)Command::GetEdgesCount, "Количество рёбер"},
	{(int)Command::AddEdge, "Добавить ребро"},
	{(int)Command::GetEdge, "Получить ребро"},
	{(int)Command::SetEdgeData, "Задать данные ребра"},
	{(int)Command::GetEdgeData, "Получить данные ребра"},
	{(int)Command::SetEdgeWeight, "Задать вес ребра"},
	{(int)Command::GetEdgeWeight, "Получить вес ребра"},
	{(int)Command::RemoveEdge, "Удалить ребро"},
	{(int)Command::GetVertexesCount, "Количество вершин"},
	{(int)Command::AddVertex, "Добавить вершину"},
	{(int)Command::SetVertexData, "Задать данные вершины"},
	{(int)Command::GetVertexData, "Получить данные вершины"},
	{(int)Command::SwitchName, "Сменить имя вершины"},
	{(int)Command::RemoveVertex, "Удалить вершину"},
	{(int)Command::Print, "Вывести граф"},
	{(int)Command::PrintCommands, "Очистить экран и вывести команды"},
	{(int)Command::ReturnToMenu, "Выйти из текущего меню"} };

	void PrintCommands() { printCommands(commands); }
};

class IteratorsCommands : public CommandsHandler
{
public:
	IteratorsCommands() { PrintCommands(); }

	enum class Command {
		VIteratorBegin, VIteratorValue, VIteratorInc, VIteratorEnd,
		EIteratorBegin, EIteratorValue, EIteratorInc, EIteratorEnd,
		OutEIteratorBegin, OutEIteratorValue, OutEIteratorInc, OutEIteratorEnd,
		PrintCommands, ReturnToMenu
	};

	int HandleInput()
	{
		int input = Input<int>::Get("Ввод>> ");

		switch (input)
		{
		case (int)Command::VIteratorBegin:
			vIterator = vIterator.Begin();
			break;
		case (int)Command::VIteratorValue:
		{
			try { (*vIterator).Print(); }
			catch (const char* ex) { cout << ex; }
			cout << endl;
			break;
		}
		case (int)Command::VIteratorInc:
			cout << vIterator++ << endl;
			break;
		case (int)Command::VIteratorEnd:
			vIterator = vIterator.End();
			break;
		case (int)Command::EIteratorBegin:
			eIterator = eIterator.Begin();
			break;
		case (int)Command::EIteratorValue:
		{
			try { (*eIterator).Print(); }
			catch (const char* ex) { cout << ex; }
			cout << endl;
			break;
		}
		case (int)Command::EIteratorInc:
			cout << eIterator++ << endl;
			break;
		case (int)Command::EIteratorEnd:
			eIterator = eIterator.End();
			break;
		case (int)Command::OutEIteratorBegin:
		{
			_Vertex* vertex;

			if (!getVertex(&vertex, "Имя вершины"))
				break;

			outEIterator = RGZGraph::OutputEdgesIterator(*graph, vertex);
			break;
		}
		case (int)Command::OutEIteratorValue:
		{
			try { (*outEIterator).Print(); }
			catch (const char* ex) { cout << ex; }
			cout << endl;
			break;
		}
		case (int)Command::OutEIteratorInc:
			cout<< outEIterator++ << endl;
			break;
		case (int)Command::OutEIteratorEnd:
			outEIterator = outEIterator.End();
			break;
		case (int)Command::PrintCommands:
			PrintCommands();
			break;
		case (int)Command::ReturnToMenu:
			return BACK_TO_MENU;
		default:
			cout << "Неверный номер команды\n";
		};

		return SUCCESS_INPUT;
	}

protected:

	vector<commandView> commands = {
	{(int)Command::VIteratorBegin, "Итератор вершин: установить в начало"},
	{(int)Command::VIteratorValue, "Итератор вершин: получить значение"},
	{(int)Command::VIteratorInc, "Итератор вершин: следующая вершина"},
	{(int)Command::EIteratorBegin, "Итератор рёбер: установить в начало"},
	{(int)Command::EIteratorValue, "Итератор рёбер: получить значение"},
	{(int)Command::EIteratorInc, "Итератор рёбер: следующее ребро"},
	{(int)Command::OutEIteratorBegin, "Итератор исх. рёбер: установить в начало"},
	{(int)Command::OutEIteratorValue, "Итератор исх. рёбер: получить значение"},
	{(int)Command::OutEIteratorInc, "Итератор исх. рёбер: следующее ребро"},
	{(int)Command::PrintCommands, "Очистить экран и вывести команды"},
	{(int)Command::ReturnToMenu, "Выйти из текущего меню"} };

	void PrintCommands() { system("CLS"); printCommands(commands); }
};

template <class T>
class TaskCommands : public CommandsHandler
{
public:
	TaskCommands(T** task, string description) : task(task), taskDescription(description) { PrintCommands(); }

	enum class Command {
		Create, Restart, Result, PrintCommands, ReturnToMenu
	};

	int HandleInput()
	{
		int input = Input<int>::Get("Ввод>> ");


		switch (input)
		{
		case (int)Command::Create:
		{
			//111111111111111111111111111111111111111111111111111111
			delete* task;
			try {
				*task = new T(*graph);
			}
			catch (const char* ex)
			{
				cout << ex << endl;
				*task = nullptr;
			}
			break;
		}
		case (int)Command::Restart:
		{
			if (*task == nullptr)
			{
				cout << "Создайте объект задания\n";
				break;
			}

			try { static_cast<Task<_Vertex, _Edge>*>(*task)->Restart(); }
			catch (const char* ex)
			{
				cout << ex << endl;
			}

			break;
		}
		case (int)Command::Result:
			if (*task == nullptr)
			{
				cout << "Создайте объект задания\n";
				break;
			}
			static_cast<Task<_Vertex, _Edge>*>(*task)->Result();
			break;
		case (int)Command::PrintCommands:
			PrintCommands();
			break;
		case (int)Command::ReturnToMenu:
			return BACK_TO_MENU;
		default:
			cout << "Неверный номер команды\n";
		};

		return SUCCESS_INPUT;
	}

protected:

	vector<commandView> commands = {
	{(int)Command::Create, "Создать объект, привязанный к графу и решить"},
	{(int)Command::Restart, "Решить задание заново"},
	{(int)Command::Result, "Вывести результат"},
	{(int)Command::PrintCommands, "Очистить экран и вывести команды"},
	{(int)Command::ReturnToMenu, "Выйти из текущего меню"} };

	T** task;
	string taskDescription;

	void PrintCommands() { system("CLS"); cout << taskDescription << endl; printCommands(commands); }
};

class MainMenu
{
public:

	void HandleCurrentMenuInput()
	{
		if (handler != nullptr)
		{
			int code = handler->HandleInput();

			if (code == BACK_TO_MENU)
			{
				delete handler;
				handler = nullptr;
				system("CLS");
			}

			return;
		}

		cout << "1 - Граф\n";
		cout << "2 - Команды для итераторов\n";
		cout << "3 - Задание №2\n";
		cout << "5 - Задание №3\n";
		cout << "5 - Выход\n";

		int input = Input<int>::Get("Ввод>> ");

		switch (input)
		{
		case 1:
			handler = new GraphCommands();
			break;
		case 2:
			handler = new IteratorsCommands();
			break;
		case 3:
			handler = new TaskCommands<Task1v6<_Vertex, _Edge>>(&task1, task1Description);
			break;
		case 4:
			handler = new TaskCommands<SecondTask<_Vertex, _Edge>>(&task2, task2Description);
			break;
		case 5:
			exit(0);
			break;
		default:
			cout << "Неправильный номер команды\n";
			break;
		}

	}

protected:

	CommandsHandler* handler = nullptr;
};

int main()
{
	setlocale(LC_ALL, "Russian");
	srand(time(NULL));

	MainMenu menu;

	while (true)
	{
		menu.HandleCurrentMenuInput();
	}
}