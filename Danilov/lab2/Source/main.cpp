#include <iostream>
#include <vector>
#include <string>
#include <cfloat>
#include <cmath>
#include <algorithm>
//+-200

#define PRINT
//#define GREEDY
#define ASTAR

constexpr size_t matrixSize = 26;


//constexpr size_t matrixSize = 26;

struct Node
{
    float distance = 0;
    bool isVizited = false;
    //A*
    float gX = 0;  // len
    float hX = 0;  // heuristic
    char prevNode = '\0';
};

struct PriorityOne
{
    char next = '\0';
    char prev = '\0';
    float gX = 0;  // len
    bool operator==(const PriorityOne &tmp) const
    {
        return tmp.next == next && tmp.prev == prev && tmp.gX == gX;
    }
};

// граф
class Graph
{
    std::vector<std::vector<Node>> matrix;
    std::string result;
public:
    //init
    Graph()
    {
        matrix.resize(matrixSize);
        for (auto i = 0; i < matrixSize; ++i)
            matrix[i].resize(matrixSize);
    }
    //----------------------------------------------------------------------------------
    // жадный алгоритм true == есть решение
    bool greedyOne(char start, char end)
    {
        result.push_back(start);
        if(start == end)
            return true;  // if a->a
        #ifdef PRINT
        std::cout << "Start node is " << start << std::endl;
        #endif
        auto minNode = start;

        while (isAnyOne())
        {
            auto minDistance = FLT_MAX;
            auto tmpNode = '-';

            // поиск ребра наименьшего размера из данной вершины
            for (auto i = 0; i < matrixSize ; i++) {
                // проверка на то, была ли вершина посещена и не находится ли она в решении
                if (matrix[minNode - 'a'][i].isVizited || isInPlenty(result, i+'a'))
                    continue;
                // нашли путь оптимальнее прежнего
                if (matrix[minNode - 'a'][i].distance < minDistance &&
                    matrix[minNode - 'a'][i].distance != 0)
                {
                    minDistance = matrix[minNode - 'a'][i].distance;
                    tmpNode = i; // вершина с мин путем из текущей
                }
            }
            // если нашли мин ребро,
            // то добавляем его в решение

            if (minDistance!=FLT_MAX && tmpNode!= '-') {
                matrix[minNode - 'a'][tmpNode].isVizited = true;
                result.push_back(tmpNode+'a');

                #ifdef PRINT
                std::cout << char(tmpNode+'a') << " is added to result" << std::endl;
                std::cout << "Current result is: ";
                printResult();
                #endif

                minNode = tmpNode + 'a';
            }
                //мин ребро не найдено -> тупиковая вершина
            else
            {
                if (result.empty())
                    break; // -> false
                //throw std::length_error("There is no way");
                auto lonelyOne = result[result.length()-1];
                result.pop_back();

                #ifdef PRINT
                std::cout << "There is no way from:  " << lonelyOne <<"  to: " << end<<  std::endl;
                std::cout << "Current result is: ";
                printResult();
                #endif

                // обнуляем все пути ведущие в эту вершину
                for (auto i = 0; i < matrixSize; i++)
                    matrix[i][lonelyOne ].isVizited = true;

                if (result.empty())
                    break; // -> false

                minNode = result[result.length()-1];
            }
            if (result[result.length()-1] == end)
                return true;
        }
        return false;
    }
    //------------------------------------------------------------------------------------
    // эвристический алгоритм true == есть решение

    bool aStar(char start, char end)
    {
        if(start == end)
        {
            result.push_back(start);
            return true;
        }

        std::vector<PriorityOne> que;
        std::string closed, open;
        char currentNode;
        open.push_back(start);

        // добавляем все вершины, доступные из начальной
        for (char nextNode = 0; nextNode < matrixSize; nextNode++)
        {
            currentNode = start - 'a';
            // есть путь
            if (matrix[currentNode][nextNode].distance != 0)
            {
                float tmpGX = matrix[currentNode][nextNode].distance +
                              matrix[currentNode][nextNode].gX;
                PriorityOne buf;
                buf.next = nextNode + 'a';
                buf.prev = start;
                buf.gX = tmpGX;

                matrix[currentNode][nextNode].isVizited = true;
                setGX(nextNode, start, tmpGX);
                que.push_back(buf);
            }
        }
        updateClosedNodes(open, closed, start);
        // пока есть хоть одно необработанное ребро
        while (!que.empty())
        {
            #ifdef PRINT
            std::cout << "new iteration\n";
            printDistance(que);
            #endif
            PriorityOne minNode = que[0];
            auto  iterPosition = que.begin();

            // нахождение минимального элемента в очереди и его извлечение
            for (auto iter : que)
            {
                float fXMin = minNode.gX + matrix[minNode.prev - 'a'][minNode.next - 'a'].hX;
                float fXIter = iter.gX + matrix[iter.prev - 'a'][iter.next - 'a'].hX;

                #ifdef PRINT
                std::cout << "computing priority \n";
                printf("f(x) of current min (%c)->(%c) = %g\n",
                       minNode.prev, minNode.next, fXMin);
                printf("current min f(%c) = g(%c) + h(%c)  = %g + %g = %g\n",
                       minNode.next, minNode.next, minNode.next, minNode.gX,
                       matrix[minNode.prev - 'a'][minNode.next - 'a'].hX, fXMin);
                std::cout << "--------------------------------------\n";

                printf("f(x) of current element of queue (%c)->(%c) = %g\n", minNode.prev, minNode.next, fXMin);
                printf("current element f(%c) = g(%c) + h(%c) = %g + %g = %g\n",
                        iter.next, iter.next,iter.next, iter.gX,
                        matrix[iter.prev - 'a'][iter.next - 'a'].hX, fXIter);
                std::cout << "--------------------------------------\n";
                #endif

                if (fXMin > fXIter || (fXMin == fXIter && minNode.next < iter.next))
                {
                    minNode = iter;
                    iterPosition = std::find(que.begin(), que.end(), iter);
                }
            }
            #ifdef PRINT
            printf("min value in queue: from (%c) to (%c) cost g(x) = %g\n", //  erased\n",
                    minNode.prev, minNode.next, minNode.gX);
            #endif
            // если дошли до конечной вершины, восстанавливаем путь
            if (minNode.next == end)
            {
                #ifdef PRINT
                std::cout << " set g(" << minNode.next << ") = " << minNode.gX << std::endl;
                #endif
                setGX(minNode.next - 'a', minNode.prev, minNode.gX);
                restorePath(start,end);
                return true;
            } else
            {
            // не дошли до конца
                que.erase(iterPosition);
                updateClosedNodes(open, closed, minNode.prev);
                //смотрим всех сосеедй этой вершины
                for (char i = 0; i < matrixSize; i++)
                //по аналогии со стартом
                {
                    //есть ребро
                    if (matrix[minNode.next - 'a'][i].distance != 0)
                    {
                        float tmpGX = minNode.gX + matrix[minNode.next - 'a'][i].distance;
                        PriorityOne buf;
                        buf.next = i + 'a';
                        buf.prev = minNode.next;
                        buf.gX = tmpGX;
                        //если элемент ведет в обработанную вершину -> игнорируем
                        if (isInPlenty(closed, i + 'a'))
                            continue;
                        // если эту вершину еще не трогали -> трогаем и помещаем в open
                        if(!isInPlenty(open, i + 'a'))
                        {
                            #ifdef PRINT
                            std::cout << "set g(" << buf.next << ") = " << tmpGX;
                            #endif
                            setGX(buf.next - 'a', buf.prev, tmpGX);
                            open += buf.next;
                        } else  // если уже проходили - смотрим, можем ли уменьшить путь
                        {
                            if (matrix[i][0].gX > tmpGX)
                            {
                                #ifdef PRINT
                                std::cout << "update g(x) for node: " << 'a'+i <<
                                std::endl << "old = " << matrix[i][0].gX << ", new = " <<
                                tmpGX << std::endl;
                                #endif
                                // обновляем, если меньше
                                setGX(i,minNode.next, tmpGX);
                            }
                        }
                        #ifdef PRINT
                        printf("add way from (%c) to (%c) cost g(x) = %g\n",
                                buf.prev, buf.next, buf.gX);
                        #endif
                        que.push_back(buf);
                    }
                }
            }
        }
        return false;
    }

    //-----------------------------------------------------------------------------------
    // добавление ребра
    void addEdge (char start, char end, float distance)
    {
        if(start - 'a' >= matrixSize || end - 'a' >= matrixSize || matrix[start - 'a'][end - 'a'].distance != 0)
            throw std::range_error("Node should be from a to z") ;
        matrix[start - 'a'][end - 'a'].distance = distance;
    }

    //принадлежит ли вершина множеству
    bool isInPlenty (std::string& plenty, char node) const
    {
        for (auto &iter : plenty)
            if (iter == node)
                return true;
        return false;
    }
    // есть ли непросмотренное ребро
    bool isAnyOne() const
    {
        for (auto i = 0; i < matrixSize; i++)
            for (auto j = 0; j < matrixSize; j++)
                if (!matrix[i][j].isVizited)
                    return true;
        return false;
    }

    // необходим при работе обоих алгоритмов
    void clear()
    {
        for(auto i = 0; i < matrixSize; i++ )
            for(auto j = 0; j < matrixSize; j++ )
                matrix[i][j].isVizited = false;
        result.clear();
    }
    // необходимо для корректного прохода тестов на степике
    void setStepicHeuristics(char node)
    {
        for (auto i = 0; i < matrixSize; i++ )
            for (auto j = 0; j < matrixSize; j++ )
                matrix[i][j].hX = std::fabs(static_cast<float>(node - (j + 'a')));
    }

    // видоизменение за счет варианта; сам вводишь эвристику для каждой вершины
    void setOwnHeuristics()
    {
        constexpr double inaccuracy = 0.00000000001;
        bool flag;
        for (auto i = 0; i < matrixSize; i++)
        {
            flag = false;
            for (int j = 0; j < matrixSize ; j++)
            {
                if (matrix[i][j].distance != 0 && matrix[i][j].distance > inaccuracy )
                {
                    flag = true;
                    break;
                }
            }
            if (!flag)
            {
                for (int j = 0; j < matrixSize ; j++)
                {
                    if (matrix[j][i].distance != 0 && matrix[j][i].distance > inaccuracy)
                    {
                        flag = true;
                        break;
                    }
                }
            }
            if (!flag)
                continue;
            float val = -1;
            while (val < 0)
            {
                std::cout << "Enter heuristic for top (" << char(i + 'a') <<") : ";
                std::cin >> val;
            }
            for (auto j = 0; j < matrixSize; j++)
                matrix[j][i].hX = val;
        }
    }
    // установление расстояния от старта до текущей вершины
    void setGX(char next, char prev, float  distance)
    {
        for (char i = 0; i < matrixSize; i++){
            matrix[next][i].prevNode = prev;
            matrix[next][i].gX = distance;
        }
    }

    // восстановлениенайденного пути
    void restorePath(char start, char  end)
    {
        std::string reversedResult;
        auto prevNode = end;
        while (true)
        {
            if (prevNode == start || prevNode == '\0')
            {
                reversedResult += start;
                break;
            }
            reversedResult += prevNode;
            prevNode = matrix[prevNode - 'a'][0].prevNode;
        }
        for (auto i = reversedResult.rbegin(); i != reversedResult.rend(); i++)
            result += *i;
    }
    // добавление вершины в множесво закрытых (обработанных) вершин
    void updateClosedNodes (std::string& open, std::string& closed, char node)
    {
        for (char & iter : open)
            if (iter == node)
            {
                #ifdef PRINT
                std::cout << "all nodes connected with (" << iter << ") are visited\n";
                std::cout << "(" << iter << ") was added to closed nodes" << std::endl;
                #endif

                closed.push_back(iter);
                auto tmpIter = std::find(open.begin(), open.end(), iter);
                open.erase(tmpIter);
                break;
            }
    }

    // выввод расстояний между вершинами
    void printDistance (std::vector<PriorityOne>& que) const
    {
        std::cout << "In queue:\n";
        for (auto item : que)
            std::cout << "distance from (" << item.prev << ") to (" << item.next << ") = "
            << item.gX << std::endl;
    }
    // вывод результата
    void printResult () const
    {
        std::cout << result << std::endl;
    }

};

int main() {
    char startNode, endNode, start, end;
    float distance;
    Graph graph;

    std::cin >> start >> end;
    std::cin >> startNode;
    while (startNode != '.')
    {
        std::cin >> endNode >> distance;
        try
        {
            graph.addEdge(startNode, endNode, distance);
        }
        catch (std::range_error &e)
        {
            std::cout <<"\n ERRROR :"<< e.what() << std::endl;
            break;
        }
        std::cin >> startNode;
    }
    #ifdef GREEDY

//    #ifdef PRINT
        std::cout << "Greedy algoritm\n";
//    #endif

    if (graph.greedyOne(start, end))
    {
        #ifdef PRINT
        std::cout << "Shortest way is: ";
        #endif

        graph.printResult();
    }
    else
        std::cout << "there is no way from: " << start << "to: " << end << std::endl;
    #endif

    #ifdef ASTAR

//    #ifdef PRINT
    std::cout << "ASTAR algoritm\n";
//    #endif
    graph.clear();
//    graph.setStepicHeuristics(end);
    graph.setOwnHeuristics();
    if (graph.aStar(start, end))
    {
        #ifdef PRINT
        std::cout << "Shortest way is: ";
        #endif

        graph.printResult();
    } else
        std::cout << "there is no way from: " << start << "to: " << end << std::endl;
    #endif

    return 0;
}
