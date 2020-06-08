#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <queue>
#include <algorithm>


using std::string;
using std::map;
using std::vector;
using std::queue;
using std::cin;
using std::cout;
using std::endl;
using std::pair;

class BorNode;
typedef map<const char, BorNode *> LinksMap;


class BorNode {
public:
    LinksMap links;
    BorNode *fail;  // Предыдущее состояние для функции отката. Только для root равно NULL.
    BorNode *term; // Ближайшее терминальное состояние. Если отстутствует - NULL
    int out;

public:
    explicit BorNode(BorNode *fail_node = nullptr)
            : fail(fail_node)
            , term(nullptr)
            , out(-1)
    { }

    BorNode* getLink(const char c) const
    {
        auto iter = links.find(c);
        if (iter != links.cend()) {
            return iter->second;
        }
        else {
            return nullptr;
        }
    }

    bool isTerminal() const
    {
        return (out >= 0);
    }

    void showBor(){



    }
};

typedef vector< pair< int, pair< int, string > > > Answer;
typedef vector< pair< int, pair< int, string > > >::iterator AnswerIterator;
bool operator < (AnswerIterator a, AnswerIterator b){
    if (a->first < b->first)
        return true;
    else if(a->first == b->first)
        return a->second.first < b->second.first;
    else return false;
}

class AhoCorasick
{
public:
    typedef void (*Callback) (const char* substr);
    BorNode root;
    vector<string> words;
    BorNode* current_state{};
    Answer answer;

public:
    void addString(const char* const str)
    {
        BorNode *current_node = &root;
        for(int i = 0; *(str + i); ++i) {
            BorNode *child_node = current_node->getLink(*(str + i));
            if (!child_node) {
                child_node = new BorNode(&root);
                current_node->links[*(str + i)] = child_node;
            }
            current_node = child_node;
        }
        current_node->out = words.size();
        words.push_back(str);
    }



    void init()
    {
        queue<BorNode *> q;
        q.push(&root);
        while (!q.empty()) {
            BorNode *current_node = q.front();
            q.pop();
            for (auto iter = current_node->links.cbegin(); iter != current_node->links.cend(); ++iter)
            {
                const char symbol = iter->first;
                BorNode *child = iter->second;

                BorNode *temp_node = current_node->fail;
                while (temp_node) {
                    BorNode *fail_candidate = temp_node->getLink(symbol);
                    if (fail_candidate) {
                        child->fail = fail_candidate;
                        break;
                    }
                    temp_node = temp_node->fail;
                }
                if (child->fail->isTerminal()) {
                    child->term = child->fail;
                }
                else {
                    child->term = child->fail->term;
                }
                q.push(child);
            }
        }
    }

    bool step(const char c)
    {
        while (current_state) {
            BorNode *candidate = current_state->getLink(c);
            if (candidate) {
                current_state = candidate;
                return true;
            }
            current_state = current_state->fail;
        }
        current_state = &root;
        return false;
    }

    void printTermsForCurrentState(Callback callback) const
    {
        if (current_state->isTerminal()) {
            callback(words[current_state->out].c_str());
        }
        BorNode *temp_node = current_state->term;
        while (temp_node) {
            callback(words[temp_node->out].c_str());
            temp_node = temp_node->term;
        }
    }

    void search(const char* str, Callback callback)
    {
        current_state = &root;
        for(int i = 0; *(str + i); i++) {
            bool flag = step(str[i]);
            if(current_state->isTerminal()) {
                answer.push_back({i - words[current_state->out].size() + 2,{current_state->out + 1, words[current_state->out]}});
                //printTermsForCurrentState(callback);
            }
            BorNode *temp_node = current_state->term;
            while (temp_node) {
                answer.push_back({i - words[temp_node->out].size() + 2,{temp_node->out + 1, words[temp_node->out]}});
                temp_node = temp_node->term;
            }
        }
    }
};

void print(const char* str)
{
    cout << "found substring " << str << "\n";
}

int main()
{
    AhoCorasick ak;

    char* search = new char[100000];
    cin >> search;


    int n;
    cin >> n;


    for(int i = 0; i < n; i++){
        char *temp = new char[100000];
        cin >> temp;
        ak.addString(temp);
    }

    ak.init();
    ak.search(search, print);
    std::sort(ak.answer.begin(), ak.answer.end());
    for(int i = 0; i < ak.answer.size(); i++){
        cout << ak.answer[i].first << " " << ak.answer[i].second.first;
        if (i != ak.answer.size() - 1){
            cout << endl;
        }
    }
    cin.get();

    return 0;
}