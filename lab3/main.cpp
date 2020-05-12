#include <iostream>
#include <algorithm>
#include <map>
#include <set>
#include <vector>
#define PRINT

using namespace std;

// ребро, содержит информацию об остаточной пропускной спобоности и потоке, который можно пустить назад
struct Rib
{
    int C;  // остаточная пропускная способность, изначальноравна пропускной
    int F;  // обратный поток
    Rib() : C(0), F(0) {}
    Rib(int C, int F) : C(C), F(F) {}
};
class FordFulkerson
{
private:
    map<char, map<char, Rib>> resNet; //остаточная сеть
    set<pair<char, char>> graph; // исходный граф
    vector<bool> visited; //  посещенные вершины
    char source = 0, sink = 0;     // исток и сток

    int dfs(char v, int Cmin);
    void printRes();

public:
    void maxFlow();;
    void input();

};


void FordFulkerson::maxFlow() {
    int flow = 0;
    int answer = 0;
    while(true)
    {
        //обнуляем все посещения
        fill(visited.begin(), visited.end(), false);
        flow = dfs(source, INT32_MAX); // пускаем максимальный поток из истока

#ifdef PRINT
        cout << "\ncurrent flow is: " << answer+flow<<endl;
//        printRes();
#endif

        // если путь не найден
        if (flow == 0 || flow == INT32_MAX)
        {
#ifdef PRINT
            cout << "New path not found\n";
#endif
            break;
        } else
#ifdef PRINT
			cout << "A new path increased flow by: " << flow <<"\n";
#endif

        answer += flow;
    }
    cout <<"MAX flow is:  "<< answer << endl;
    printRes();
}

int FordFulkerson::dfs(char v, int Cmin) // Cmin - минимальная пропускная способность
{
#ifdef PRINT
	cout << char(v + 'a');
#endif
    //если вершина была посещены-> выходим
    if (visited[v])
        return 0;
    visited[v] = true;

    //если веришна - сток -> выходим
    if (v == sink)
        return Cmin;

    // множество смежных вершин, сортированных по остаточной пропускной способности
    set<pair<int, char>> toVisit;

    for (auto u : resNet[v])
        if (!visited[u.first])
            toVisit.insert({max(u.second.C, u.second.F), u.first});

    //обходим вершины из множества в порядке убывания остаточной пропускной способности
    for (auto u : toVisit)
    {
        // если есть поток, который можем пустить обратно, делаем это с ребром мин веса
        if (resNet[v][u.second].F > 0) {
            auto delta = dfs(u.second, min(Cmin, resNet[v][u.second].F));
            if (delta > 0)
            {
                resNet[u.second][v].C += delta;
                resNet[v][u.second].F -= delta;
                return delta;
            }
        }
        // если остаточная пропускная способнсть положительна,
        // находим ребро с наименьшим весом и пускаем поток по нему
        if (resNet[v][u.second].C > 0)
        {
            auto delta = dfs(u.second, min(Cmin, resNet[v][u.second].C));
            if (delta > 0){
                resNet[u.second][v].F += delta;
                resNet[v][u.second].C -= delta;
                return delta;
            }
        }
    }
    return 0;
}

void FordFulkerson::printRes() {
    for (auto &i : graph)
    {
        cout << char(i.first + 'a') << ' ' << char(i.second + 'a') <<' '<< resNet[i.second][i.first].F;
        cout << endl;
    }
}

void FordFulkerson::input() {
    int N;
    int capacity;
    char u, v;

    cin >> N;
    cin >> source >> sink;
    source -= 'a';
    sink -= 'a';

    visited.resize(26);

    for (int i = 0; i < N; i++)
    {
        cin >> u >> v >> capacity;
        u -= 'a';
        v -= 'a';
        graph.insert({u,v});
        resNet[u][v].C = capacity;
    }
}


int main() {
    FordFulkerson F;
    F.input();
    cout << endl;
    F.maxFlow();
    return 0;
}
