#include <iostream>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <map>
#include <stack>
#include <vector>
#include <cassert>
#include <climits>
#include <fstream>
using namespace std;
struct Mail {
	string path;
	string from;
	string to;
	int date;
	int ID;
	int count;
	unordered_set<string> words;
	Mail(): ID(0), count(0) {}
};
struct Token {
	int type;
	string expression;
	int op;
	Token(int v): type(1), op(v) {};
	Token(string s): expression(s), type(0) {};
	Token() : type(0) {};
	Token(Token const &b): type(b.type), expression(b.expression), op(b.op) {};
	Token operator=(Token const &a) { Token b(a); return b; }
};
int what_month(string &s, int i);
int tolower(char c);
void s_tolower(string &s);
bool isalnum(char c);
int epoch(string &s);
int nepoch(string &s, int i);
unordered_map<string, int> paths; // path was loaded, to ID
vector<Mail> mails;
unordered_set<string> valid; // path was valid
map<int, set<int> > count2ID;
unordered_map<string, unordered_set<int> > from2ID;
unordered_map<string, unordered_set<int> > to2ID;
map<int, unordered_set<int> > date2ID;
int N;
void add() {
	string path;
	cin >> path;
	if (paths.find(path) == paths.end()) {
		N++;
		printf("%d\n", N);
		ifstream fin;
		fin.open(path, ifstream::in);
		Mail current;
		current.path = path;
		getline(fin, current.from);
		assert(current.from[4] == ':');
		current.from.erase(0, 6);
		s_tolower(current.from);
		// cout << current.from << endl;
		string line;
		getline(fin, line);
		assert(line[4] == ':');
		line.erase(0, 6);
		current.date = epoch(line);
		getline(fin, line);
		assert(line[10] == ':');
		line.erase(0, 12);
		for (int i = 0; i < line.length(); i++)
			current.ID = current.ID * 10 + (line[i] - '0');
		getline(fin, line);
		assert(line[7] == ':');
		line.erase(0, 9);
		int l = line.length();
		for (int i = 0; i < l;) {
			while (i < l && !isalnum(line[i]))
				i++;
			string t;
			while (i < l && isalnum(line[i])) {
				t.push_back(tolower(line[i]));
				i++;
			}
			if (t.size() != 0) {
				// cout << "subject: " << t << " " << t.size() << endl;
				current.words.insert(t);
			}
		}
		getline(fin, current.to);
		assert(current.to[2] == ':');
		current.to.erase(0, 4);
		s_tolower(current.to);
		getline(fin, line);
		getline(fin, line, '\0');
		l = line.length();
		for (int i = 0; i < l;) {
			while (i < l && !isalnum(line[i]))
				i++;
			string t;
			while (i < l && isalnum(line[i])) {
				t.push_back(tolower(line[i]));
				current.count++;
				i++;
			}
			if (t.size() != 0) {
				// cout << "content: " << t << " " << t.size() << endl;
				current.words.insert(t);
			}
		}
		if (mails.size() <= current.ID)
			mails.resize(current.ID * 2);
		mails[current.ID] = current;
		valid.insert(path);
		paths[path] = current.ID;
		count2ID[current.count].insert(current.ID);
		from2ID[current.from].insert(current.ID);
		to2ID[current.to].insert(current.ID);
		date2ID[current.date].insert(current.ID);
		/*cout << endl << endl;
		cout << "From: " << current->from << endl;
		cout << "date: " << current->date << endl;
		cout << "ID: " << current->ID << endl;
		cout << "to: " << current->to << endl;
		cout << "subject: " << line << endl;*/
	} else {
		if (valid.find(path) == valid.end()) {
			valid.insert(path);
			Mail *current = &mails[paths[path]];
			count2ID[current->count].insert(current->ID);
			from2ID[current->from].insert(current->ID);
			to2ID[current->to].insert(current->ID);
			date2ID[current->date].insert(current->ID);
			N++;
			printf("%d\n", N);
		} else {
			printf("-\n");
		}
	}
}
void remove() {
	int id;
	cin >> id;
	string path = mails[id].path;
	unordered_set<string>::iterator itr;
	itr = valid.find(path);
	if (itr != valid.end()) {
		N--;
		Mail *current = &mails[id];
		printf("%d\n", N);
		valid.erase(itr);
		assert(count2ID[current->count].erase(id) == 1);
		assert(from2ID[current->from].erase(id) == 1);
		assert(to2ID[current->to].erase(id) == 1);
		assert(date2ID[current->date].erase(id) == 1);
	} else {
		printf("-\n");
	}
}
void longest() {
	if (N != 0) {
		map<int, set<int> >::reverse_iterator itr;
		int id;
		for (itr = count2ID.rbegin(); itr != count2ID.rend(); itr++) 
			if (itr->second.begin() != itr->second.end()) {
				id = *(itr->second.begin());
			}
		printf("%d %d\n", id, mails[id].count);
	}
	else
		printf("-\n");
}
void query() {
	string s;
	set<int> answer;
	string from, to;
	int dateStart = INT_MIN, dateEnd = INT_MAX;
	while (cin >> s) {
		if (s[0] != '-') {
			stack<Token> operators;
			vector<Token> postfix;
			int length = s.length();
			for (int i = 0; i < length;) {
				if (isalnum(s[i])) {
					Token current;
					current.type = 1;
					while (i < length && isalnum(s[i])) {
						current.expression.push_back(tolower(s[i]));
						i++;
					}
					postfix.push_back(current);
				} else {
					Token current;
					current.type = 0;
					if (s[i] == '(') {
						current.op = 6;
					} else if (s[i] == ')') {
						current.op = 5;
					} else if (s[i] == '!') {
						current.op = 2;
					} else if (s[i] == '&') {
						current.op = 3;
					} else if (s[i] == '|') 
						current.op = 4;
					if (current.op == 6) {
						operators.push(current);
					} else if (current.op == 5) {
						while (operators.top().op != 6) {
							postfix.push_back(operators.top());
							operators.pop();
						}
						operators.pop();
					} else {
						while (!operators.empty() && operators.top().op < current.op) {
							postfix.push_back(operators.top());
							operators.pop();
						}
						operators.push(current);
					}
					i++;
				}
			}
			while (!operators.empty()) {
				postfix.push_back(operators.top());
				operators.pop();
			}
			for (int i = mails.size() - 1; i >= 0; i--) {
				if (valid.find(mails[i].path) == valid.end())
					continue;
				if (!from.empty() && mails[i].from != from)
					continue;
				if (!to.empty() && mails[i].to != to)
					continue;
				if (dateStart != INT_MIN && mails[i].date < dateStart)
					continue;
				if (dateEnd != INT_MAX && mails[i].date > dateEnd)
					continue;
				vector<bool> check;
				int length = postfix.size();
				for (int j = 0; j < length; j++) {
					if (postfix[j].type == 1) {
						if (mails[i].words.find(postfix[j].expression) != mails[i].words.end())
							check.push_back(true);
						else
							check.push_back(false);
					}
					else {
						int back = check.size() - 1;
						if (postfix[j].op == 2) 
							check[back] = !check[back];
						else if (postfix[j].op == 3) {
							check[back - 1] = (check[back] && check[back - 1]);
							check.pop_back();
						} else {
							check[back - 1] = (check[back] || check[back - 1]);
							check.pop_back();
						}
					}
				}
				assert(check.size() == 1);
				if (check[0])
					answer.insert(i);
			}
			break;
		} else if (s[1] == 'f') {
			s.erase(0, 3);
			s.pop_back();
			s_tolower(s);
			from = s;
		} else if (s[1] == 't') {
			s.erase(0, 3);
			s.pop_back();
			s_tolower(s);
			to = s;
		} else if (s[1] == 'd') {
			s.erase(0, 2);
			if (s.length() > 0 && s[0] == '~') {
				dateEnd = nepoch(s, 1);
			} else if (s.length() > 12 && s[12] == '~') {
				dateStart = nepoch(s, 0);
				s.erase(0, 13);
				if (s.length() != 0)
					dateEnd = nepoch(s, 0);
			}
		}
	}
	bool start = true;
	set<int>::iterator itr;
	for (itr = answer.begin(); itr != answer.end(); itr++) {
		if (valid.find(mails[*itr].path) == valid.end())
			continue;
		if (start)
			start = false;
		else
			printf(" ");
		printf("%d", *itr);
	}
	if (start)
		printf("-\n");
	else
		printf("\n");
}
int main() {
	string instruction;
	while (cin >> instruction) {
		// cout << instruction << endl;
		if (instruction[0] == 'a') 
			add();
		else if (instruction[0] == 'r')
			remove();
		else if (instruction[0] == 'l')
			longest();
		else
			query();
	}
}
int what_month(string &s, int i) {
	switch(s[i]) {
		case 'J':
			if (s[i + 1] == 'a')
				return 1;
			else if (s[i + 2] == 'n')
				return 6;
			else
				return 7;
		case 'F':
			return 2;
		case 'M':
			if (s[i + 2] == 'r')
				return 3;
			else 
				return 5;
		case 'A':
			if (s[i + 1] == 'p')
				return 4;
			else
				return 8;
		case 'S':
			return 9;
		case 'O':
			return 10;
		case 'N':
			return 11;
		case 'D':
			return 12;
		default:
			printf("wrong month: %s\n", s.c_str());
			return 13;
	}
}
int tolower(char c) {
	if ('A' <= c && c <= 'Z')
		return c - 'A' + 'a';
	return c;
}
void s_tolower(string &s) {
	for (int i = s.length() - 1; i >= 0; i--)
		s[i] = tolower(s[i]);
}
bool isalnum(char c) {
	return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9'); 
}
int epoch(string &s) {
	// cout << s << endl << s.size() << endl;
	assert('0' <= s[0] && s[0] <= '9'); 
	int day, month, year, hour, minute;
	int month_start = 2;
	if ('0' <= s[1] && s[1] <= '9') {
		day = (s[0] - '0') * 10 + (s[1] - '0');
		// cout << day << endl;
		assert(1 <= day && day <= 31);
		month_start = 3;
	} else {
		day = (s[0] - '0');
		assert(1 <= day && day <= 31);
	}
	month = what_month(s, month_start);
	assert(1 <= month && month <= 12);
	int index = s.rfind(':');
	year = (s[index - 8] - '0') * 10 + (s[index - 7] - '0');
	assert(6 <= year && year <= 12);
	hour = (s[index - 2] - '0') * 10 + (s[index - 1] - '0');
	assert(0 <= hour && hour <= 23);
	minute = (s[index + 1] - '0') * 10 + (s[index + 2] - '0');
	assert(0 <= minute && minute <= 59);
	// cout << "time: " << minute << " " << hour << " " << day << " " << month << " " << year << endl;
 	return minute + hour * 60 + day * 1440 + month * 46080 + year * 600000;
}
int nepoch(string &s, int i) {
	int year, month, day, hour, minute;
	year = (s[i + 2] - '0') * 10 + (s[i + 3] - '0');
	month = (s[i + 4] - '0') * 10 + (s[i + 5] - '0');
	day = (s[i + 6] - '0') * 10 + (s[i + 7] - '0');
	hour = (s[i + 8] - '0') * 10 + (s[i + 9] - '0');
	minute = (s[i + 10] - '0') * 10 + (s[i + 11] - '0');
	assert(1 <= day && day <= 31);
	assert(1 <= month && month <= 12);
	assert(6 <= year && year <= 12);
	assert(0 <= hour && hour <= 23);
	assert(0 <= minute && minute <= 59);
	return minute + hour * 60 + day * 1440 + month * 46080 + year * 600000;
}