//README!!!!
// чтобы включить вывод промежуточной информации необходимо
// раскомментировать cтроку №14
// в main выбрать, какое задание необходимо решить и выбрать, какую функцию оставить.

#include <iostream>
#include <vector>
#include <string>
#include <iterator>
#include <thread>
#include <algorithm>
#include <set>

// #define PRINT

using namespace std;
vector<int> KMP(string t, string p, vector<int> &pi);
vector<int> getPrefix (string s) {
	int n = (int) s.length();
	vector<int> pi(n, 0);
	int i = 1, j = 0;
	#ifdef PRINT
	cout << "PREFIX FUNCTION" << endl;
//	cout << s << endl;
	#endif
	while(i < n){
	#ifdef PRINT
		for (auto x:s)
			cout << x << ' ';
		cout << endl;
		for(int h = 0; h < n; h++)
			cout << pi[h] << ' ';
		cout << endl;
	#endif
		if(s[i] == s[j]){
			#ifdef PRINT
			cout << s[i] << " == " << s[j] << " index: " << i << ' ' << j << "; pi[" << i << "] => " << j+1 << endl;
			#endif
			pi[i]= j+1;
			i++;
			j++;
		}
		else {
			if(j == 0){
				#ifdef PRINT
				cout << s[i] << " != " << s[j] << " index: " << i << ' ' << j << "; pi[" << i << "] => " << 0 << endl;
				#endif
				pi[i] = 0;
				i++;
			}
			else {
				#ifdef PRINT
				cout << s[i] << " != " << s[j] << " index: " << i << ' ' << j << "; j => " << pi[j-1] << endl;
				#endif
				j = pi[j-1];
			}
		}
	}
#ifdef PRINT
	cout << "TOTAL FUNCTION\n";
	for (auto x:s)
		cout << x << ' ';
	cout << endl;
	for(int h = 0; h < n; h++)
		cout << pi[h] << ' ';
	cout << endl;
#endif
	return pi;
}

void split(string t, string p, int k, vector<string> &s, vector<int> &currentAnswer, set<int> &allAnswers, vector<int> &pi){
//	t - text
//	p - pattern
//	k = число частй
//	s - все части texta
//  currentAnswer  - вектор ответов для данной части
//  allAnswers -  сет всех ответов
//  pi - функция

	int len_parts, flag = 0;
	int k1;
	//---------------------------------------------------
	//определяем длинну каждой части
	if(t.length() % k){
		len_parts = int(t.length()/k)+1; //длинна части строки
		flag = 1;
		k1 = k-1;
	}
	else {
		k1 = k;
		len_parts = t.length()/k;
	}
	//---------------------------------------------------
	int begin = 0;
	string part = "";
	//цикл для получения массива подстрок из текста
	while(k1 > 0){
		part = "";
		part.append(t, begin, len_parts);
		s.push_back(part);
		begin += len_parts;
		k1--;
	}
	if(flag){
		part = "";
		part.append(t, begin, (t.length()-(len_parts*(k-1))));
		s.push_back(part);
	}
#ifdef PRINT
	cout << "\nИтоговое разделение текста\n";
	for(auto x: s){
		for (auto y : x)
			cout << y << ' ';
		cout << "| ";
	}
	cout << endl;
#endif
	//цикл для получения и проверки подстрок на стыках на каждом стыке проверяется 2 стрки
	k1 = 1;
	while(k1 < k){
		part = "";
		part.append(t, (len_parts*k1)-p.length()+1, 2*p.length()-2);
		int top = (len_parts*k1)-p.length()+1;
		#ifdef PRINT
		cout << "---------------------------------------" << endl;
		cout << "Подстрака с центром на месте разреза - " << part << endl;
		cout << "Индексы в исходном тексте: ";
		for(int i = 0; i < part.size(); i++){
			cout << i+top << ' ';
		}
		cout << endl;
		cout << "Индексы:                   ";
		for(int i = 0; i < part.size(); i++){
			if(i+top > 9)
				cout << i << "  ";
			else {
				cout << i << " ";
			}
		}
		cout << endl;
		cout << "Символы подстроки:         ";
		for(int i = 0; i < part.size(); i++){
			if(i+top > 9)
				cout << part[i] << "  ";
			else {
				cout << part[i] << " ";
			}
		}
		cout << endl;
		#endif
		currentAnswer = KMP(part, p, pi);
		if(currentAnswer.size() > 0){
			for(int i = 0; i < currentAnswer.size(); i++){
				currentAnswer[i] += top; //определяем номер символа начала подстроки в исходном тексте
				allAnswers.insert(currentAnswer[i]);
			}
		}
		k1++;
	}
}

vector<int> KMP(string t, string p, vector<int> &pi){
	vector<int> ans;
#ifdef PRINT
	cout << "Префикс-функция для образца  " << p << endl;
	for(int i = 0; i < pi.size(); i++)
		cout << pi[i] << ' ';
	cout << endl;
#endif
	int n = t.length();
	int m = p.length();
	int k = 0, l = 0;
	while(k < n){
		if(t[k] == p[l]){
#ifdef PRINT
			cout << "Совпадение:   " << t[k] << "==" << p[l] <<" index: "<< k << " " << l << endl;
#endif
			k++;
			l++;
			if(l == m){
				ans.push_back(k-l);
#ifdef PRINT
				cout << "Найдена подстрака\n-----------------------" << endl;
#endif
			}
		}
		else {
			if(l == 0){
#ifdef PRINT
				cout << "Несовпадение: " << t[k] << "!=" << p[l] <<" index: "<< k << " " << l << endl;
#endif
				k++;
			}
			else {
#ifdef PRINT
				cout << "Несовпадение: " << t[k] << "!=" << p[l] <<" index: "<< k << " " << l << endl;
#endif
				l = pi[l-1];
			}
		}
	}
	return ans;
}

void KMP(){
	string p,t;
	#ifdef PRINT
	cout << "Введите образец (искомую подстроку)" << endl;
	#endif
	getline(cin, p);
	#ifdef PRINT
	cout << "Введите текст" << endl;
	#endif
	getline(cin, t);
	if(t.length() < p.length()){
		cout << "Образец не может быть больше текста!" << endl;
		cout << -1 << endl;
		return;
	}
	int max_threads = sizeof(thread); // определяем максимально возможное число потоков
	//---------------------------------------------------
	// определяем на сколько частей можно раделить строку
	double alpha = (double)t.length()/(double)p.length();
	max_threads = min(max_threads, int(alpha)-1);
	if(max_threads <= 0)
		max_threads = 1;
	int k ;//= max_threads;
	if(max_threads == 1){
		cout << "Длина исходного текста недостаточна для деления строки" << endl;
		k = 1;
	}
	else{
		cout << "Введите число от 1 до "<< max_threads << endl;
		cin >> k;
		while(k < 1 or k > max_threads){
			cout << "Введите число от 1 до "<< max_threads << endl;
			cin >> k;

		}
	}
	//---------------------------------------------------
	vector<int> pi = getPrefix(p);
	vector<int> ans, ans_current;
	vector<string> str;
	set<int> ans_all;

	#ifdef PRINT
	if(k > 1){
		cout << "--------------------------------------------" << endl;
		cout << "Строка будет разделена на " << k << " частей" << endl;
	}
	#endif
	if(k == 1){
		ans = KMP(t, p, pi);
		for(int j = 0; j < ans.size(); j++)
			ans_all.insert(ans[j]);
	}
	else {
		//---------------------------------------------------
		// определяем длинну каждой части
		int len_parts;
		if(t.length() % k){
			len_parts = int(t.length()/k)+1; //длинна части строки
		}
		else {
			len_parts = t.length()/k;
		}
		#ifdef PRINT
		cout << "Максимальная длинна части исходного текста - " << len_parts << endl;
		cout << "--------------------------------------------" << endl;
		cout << endl;
		#endif
		split(t, p, k, str, ans_current, ans_all, pi);
		//---------------------------------------------------
		//заполняем исходный массив ответов
		for(int i = 0; i< str.size(); i++){
			#ifdef PRINT
			cout << "------------------------\nЧасть исходного текста     " << str[i] << endl;
			cout << "Индексы в исходном тексте: ";
			for(int j = 0; j < str[i].size(); j++){
				cout << j+len_parts*i << ' ';
			}
			cout << endl;
			cout << "Индексы:                   ";
			for(int j = 0; j < str[i].size(); j++){
				if(j+len_parts*i > 9)
					cout << j << "  ";
				else {
					cout << j << " ";
				}
			}
			cout << endl;
			cout << "Символы подстроки:         ";
			for(int j = 0; j < str[i].size(); j++){
				if(j+len_parts*i > 9)
					cout << str[i][j] << "  ";
				else {
					cout << str[i][j] << " ";
				}
			}
			cout << endl;
			#endif
			ans_current = KMP(str[i], p, pi);
			if(ans_current.size() > 0){
				for(int j = 0; j < ans_current.size(); j++)
					ans_current[j] += (len_parts*i); // определяем номер символа начала образца в исходном тексте
				for(int j = 0; j < ans_current.size(); j++)
					ans_all.insert(ans_current[j]);
				//ans.insert(ans.end(), ans_current.begin(), ans_current.end());
			}
		}
	}
	// Ввывод ответа
	if(!ans_all.empty()){
		int end = *ans_all.rbegin();
		ans_all.erase(end);
		copy(ans_all.begin(), ans_all.end(), ostream_iterator<int>(cout, ","));
		cout << end << endl;
	}
	else {
		cout << -1 << endl;
	}
}

void cycleShift(){
	string a,b;
	#ifdef PRINT
	cout << "Введите строки 1 и 2" << endl;
	#endif
	cin >> b >> a;
	vector<int> pi = getPrefix(b);
	#ifdef PRINT
	cout << "\nCycle shift begin\n";
	for (auto x:a)
		cout << x << ' ';
	cout << endl;
	for (auto x:b)
		cout << x << ' ';
	cout << endl;
	for(auto x: pi)
		cout << x << ' ';
	cout << endl;

	#endif

	if(b.length() != a.length())
	{
		cout << "Разная длинна строк!" << endl;
		cout << "-1" << endl;
		return;
	}
	if(a == b){
		cout << "Строки совпадают" << endl;
		cout << 0 << endl;
		return;
	}
	#ifdef PRINT
	cout << "it_a - указатель на текущий символ в строке 1" << endl;
	cout << "it_b - указатель на текущий символ в строке 2" << endl;
	#endif
	int it_a = 0, it_b = 0;
	int cikle = 0;
	int al = a.length();
	while(true){
		if(a[it_a] == b[it_b]){
			#ifdef PRINT
			cout << "Совпадение:   " << a[it_a] << "==" << b[it_b] <<" index: "<< it_a << " " << it_b << endl;
			#endif
			it_a++;
			it_b++;
		}
		if(it_a == al){
			it_a = 0;
			cikle++;
		}
		if(it_b == al){
			#ifdef PRINT
			cout << "Цикл: ";
			#endif
			cout << it_a << endl;
//			cout << "Ответом является текущей it_a + 1, т.к. мы прошли всю строку и it_a указывает на ее конец" << endl;
			return;
		}
		else{
			if(a[it_a] != b[it_b]){
				#ifdef PRINT
				cout << "Несовпадение: " << a[it_a] << "!=" << b[it_b] <<" index: "<< it_a << " " << it_b << endl;
				#endif
				if(it_b == 0){
					it_a++;
				#ifdef PRINT
					cout << "Увеличиваем it_a" << endl;
				#endif
				}
				else {
					it_b = pi[it_b-1];
				#ifdef PRINT
					cout << "Уменьшаем it_b" << endl;
				#endif
				}
			}
		}
		if(cikle > 1){
			cout << -1 << endl;
			return;
		}

	}
}

int main()
{
	KMP();
	// cycleShift();
	return 0;
}
//slice slice slice slice slice like a samurai cut it up real nice, i’m a samurai go karate on the mic, i’m a samurai	samurai, i-i-i’m a samurai
