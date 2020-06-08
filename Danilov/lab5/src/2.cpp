#include <iostream>
#include <string>
#include <vector>
#include <map>

struct BorNode
{
    std::map<char, int> next;//потомки вершины
    std::map<char, int> go;// путь автомата
    std::vector<int> number;// массив номеров шаблонов
    int prev = 0;// индекс предка
    int deep = 0;// глубина вершины
    int suffix = -1;// индекс суффиксного перехода
    bool isLeaf = false;// является ли вершина листом
    char prevChar = 0;// символ предка
};


class AhoCorasick// класс реализующий алгоритм Ахо-Корасик
{
private:
    std::vector<BorNode> nodes;// вектор вершин
    std::string pattern;//паттерн
    char joker;// символ джокера
    char forbidden;//запрещенный символ
    int countTermNodes;// количество терминальных вершин
    std::vector<std::string> pattenArray;// вектор подпаттернов
    int patternLen{};// длина паттерна
    std::vector<int> matchPatterns;// вектор совпадений подпаттернов
    std::vector<int> patternsLength;// вектор длин подпаттернов образца

public:
    explicit AhoCorasick(char joker, char forbidden): matchPatterns(10000000){
        BorNode root;
        root.prev = -1;
        nodes.push_back(root);
        this->joker = joker;
        this->forbidden = forbidden;
        countTermNodes = 0;
    }

    void readPattern(std::string& str){
        this->pattern = str;
        patternLen = str.size();
        split(str);
        for (const auto& pattern : pattenArray) {
            addPattern(pattern);
        }
    }

    void search(const std::string& str)
    {
        int curr = 0;
        for (int i = 0; i < str.size(); i++) {
            curr = getLink(curr, str[i]);// по каждому символу переходим в новую вершину бора
            for (int tmp = curr; tmp != 0; tmp = getSuffix(tmp)) {// также осуществляем переходы по суффиксным ссылкам
                if (nodes[tmp].isLeaf) {
                    for (int j = 0; j < nodes[tmp].number.size(); j++) {
                        if (i + 1 - patternsLength[nodes[tmp].number[j] - 1] - nodes[tmp].deep >= 0 &&
                            i + 1 - patternsLength[nodes[tmp].number[j] - 1] - nodes[tmp].deep <= str.size() - patternLen){//если паттерн не выходит за границы (слева и справа)
                            matchPatterns[i + 1 - patternsLength[nodes[tmp].number[j] - 1] - nodes[tmp].deep]++;// добавляем индекс совпадения в вектор совпадений подпаттернов
                            break;
                        }
                    }
                }
            }
        }
    }

    void printResult(const std::string& text) const{

        std::vector<bool> cutStr(text.size());// вектор попадания символов, вошедших в паттерн
        std::string str;// входной текст без паттернов

        bool result_exist = false;

        for (int i = 0; i < matchPatterns.size(); i++) {
            if (matchPatterns[i] == patternsLength.size()) {// если число вошедших подпаттернов в индексе совпадет с числом всех подпаттнов, то это индекс вхождения паттерна
                bool is_correct = true;
                for(int k = 0; k < i + patternLen; k++){
                    if(pattern[k - i] && text[k] == forbidden && pattern[k-i] == joker){
                        is_correct = false;
                        break;
                    }
                }

                if (is_correct) {
                    std::cout << i + 1 << "\n";
                    result_exist = true;
                    for (int j = 0; j < patternLen; j++)
                        cutStr[i + j] = true;// помечаем, что символ вошел в паттерн
                }

            }
        }
        for (int i = 0; i < cutStr.size(); i++){
            if (!cutStr[i])
                str.push_back(text[i]);// заполняем строку символов, которые не вошли в паттерн
        }

        if(!result_exist){//проверка наличия ответа
            std::cout << "\nNo result. Template forbidden!";
        }
    }


private:

    void split(std::string str){// функция разделения паттернов на подпаттерны
        std::string buf;
        for (int i=0; i<str.size(); i++){
            if (str[i] == joker){
                if (!buf.empty()) {
                    pattenArray.push_back(buf);                      //заполняет массив подпаттернов
                    patternsLength.push_back(i - buf.size());       //и массив их вхождения в паттерне
                    buf = "";
                }
            }
            else {
                buf.push_back(str[i]);
                if (i == str.size() - 1){
                    pattenArray.push_back(buf);
                    patternsLength.push_back(i - buf.size() + 1);
                }
            }
        }
    }

    void addPattern(const std::string& str)// добавление символов паттерна в бор
    {
        int current = 0;
        for (char i : str) {
            if (nodes[current].next.find(i) == nodes[current].next.end()) {// если для текущей вершины нет перехода по символу
                BorNode ver;// вершина создается и добавляется в бор
                ver.suffix = -1;
                ver.prev = current;
                ver.prevChar = i;
                nodes.push_back(ver);
                nodes[current].next[i] = nodes.size() - 1;
            }
            current = nodes[current].next[i];
        }
        countTermNodes++;
        nodes[current].number.push_back(countTermNodes);   //номера подпаттернов
        nodes[current].isLeaf = true;                           // вершина объявляется терминальной
        nodes[current].deep = str.size();
    }

    int getSuffix(int index)// получение вершины перехода по суффиксной ссылке
    {
        if (nodes[index].suffix == -1) {// если суффиксная ссылка еще не определена
            if (index == 0 || nodes[index].prev == 0) {
                nodes[index].suffix = 0;// если корень или родитель корень - то суффиксная ссылка ведет в корень
            }
            else {
                nodes[index].suffix = getLink(getSuffix(nodes[index].prev), nodes[index].prevChar);// иначе переходим ищем суффикс через суффикс родителя
            }
        }
        return nodes[index].suffix;// возвращаем индекс суффиксной вершины в векторе вершин
    }

    int getLink(int index, char ch)// получить путь автомата из текущей вершины
    {
        if (nodes[index].go.find(ch) == nodes[index].go.end()) {// если пути по символу из текущей вершины нет
            if (nodes[index].next.find(ch) != nodes[index].next.end()) {
                nodes[index].go[ch] = nodes[index].next[ch];// если из вершины есть дети, то путь прокладывается через них
            }
            else {
                if (index == 0){
                    nodes[index].go[ch] = 0;
                }
                else{
                    nodes[index].go[ch] = getLink(getSuffix(index), ch);// иначе путь прокладывается через суффиксную ссылку
                }
            }
        }
        return nodes[index].go[ch];// возвращаем индекс вершины пути в векторе вершин
    }
};



int main() {
    std::string str;
    std::string pattern;
    char joker;
    char forbidden;
    std::cout << "Enter string:" << std::endl;
    std::cin >> str;
    std::cout << "Enter pattern:" << std::endl;
    std::cin >> pattern;
    std::cout << "Enter joker:" << std::endl;
    std::cin >> joker;
    std::cout << "Enter forbidden character:" << std::endl;
    std::cin >> forbidden;


    auto* ahoCorasick = new AhoCorasick(joker, forbidden);
    ahoCorasick->readPattern(pattern);
    ahoCorasick->search(str);
    ahoCorasick->printResult(str);

    return 0;
}