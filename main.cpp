#include <iostream>
#include <map>
#include <set>
#include <stack>
#include <queue>
#include <vector>
#include <string>
#include <fstream>
#include <climits>
#include <cassert>
#include <unordered_set>
#include <unordered_map>
using namespace std;

struct Mail {
	string path;
	string from;
	int date;
	int ID;
	string subject;
	string to;
	unordered_set<string> words;
	int count;
	Mail(): count(0) {}
};
struct cmpByID {
    bool operator() (Mail *a, Mail *b) const {
        return a->ID < b->ID;
    }
};
struct cmpByCount {
    bool operator() (int a, int b) const {
        return a > b;
    }
};
struct Token {
	bool isnum() { return type == 1; };
	int type;
	string expression;
	int op;
	Token(int v): type(1), op(v) {};
	Token(string s): expression(s), type(0) {};
	Token() : type(0) {};
	Token(Token const &b): type(b.type), expression(b.expression), op(b.op) {};
	Token operator=(Token const &a) { Token b(a); return b; }
};
unordered_set<string> paths; // read before?
int N; // # of mails
unordered_set<string> added; // valid
unordered_map<int, string> ID2path;
unordered_map<string, vector<Mail *> > from2Mail;
unordered_map<string, vector<Mail *> > to2Mail;
unordered_map<string, Mail * > path2Mail;
map<int, set<Mail *, cmpByID > > date2Mail;
map<int, set<Mail *, cmpByID >, cmpByCount> count2Mail;
vector<Mail *>::iterator vitr;
map<int, set<Mail *, cmpByID > >::iterator sitr;
set<Mail *, cmpByID >::iterator ssitr;
// map<string, int> precedence;
inline bool isalnum(char c) {
	return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9');
}
inline int tolower(char c) {
	if ('A' <= c && c <= 'Z')
		return c - 'A' + 'a';
	return c;
}
int nepoch(string &s) {
	int total = 0;
	int year = (s[2] - '0') * 10 + (s[3] - '0'); // year
	total += (year - 9) * 525600;
	int month = (s[4] - '0') * 10 + (s[5] - '0');
	total += month * 46080; // month 
	total += s[6] * 14400 + s[7] * 1440; // day (minute)
	int hour = (s[8] - '0') * 10 + (s[9] - '0'); // hour
	total += hour * 60;
	int minute = (s[10] - '0') * 10 + (s[11] - '0');
	total += minute;
	return total;
}
int which_month(string &month, int index) {
	switch (month[index]) {
		case 'J':
			if (month[index + 1] == 'a')
				return 1;
			else if (month[index + 2] == 'n')
				return 6;
			else
				return 7;
		case 'F':
			return 2;
		case 'M':
			if (month[index + 2] == 'r')
				return 3;
			else
				return 5;
		case 'A':
			if (month[index + 1] == 'p')
				return 4;
			else
				return 8;
		case 'S':
			return 9;
		case 'O':
			return 10;
		case 'N':
			return 11;
		default:
			return 12;
	}
}
int epoch(string &line) {
	int total = 0;
	int index = line.rfind(':');
	if (line[7] != ' ') {
		total += which_month(line, 9) * 46080; // month 
		total += line[6] * 14400 + line[7] * 1440; // day (minute)
	} else {
		total += which_month(line, 8) * 46080; // month 
		total += line[6] * 1440; // day (minute)
	}
	int year = (line[index - 8] - '0') * 10 + (line[index - 7] - '0'); // year
	total += (year - 9) * 525600;
	int hour = (line[index - 2] - '0') * 10 + (line[index - 1] - '0'); // hour
	total += hour * 60;
	int minute = (line[index + 1] - '0') * 10 + (line[index + 2] - '0');
	total += minute;
	return total;
}
void add() {
	string path;
	cin >> path;
	if (paths.find(path) == paths.end()) {
		paths.insert(path);
		Mail *current = new Mail;
		current->path = path;
		ifstream fin;
		string line;
		fin.open(path, ifstream::in);
		getline(fin, current->from);
		current->from.erase(0, 6);
		getline(fin, line);
		current->date = epoch(line);
		getline(fin, line);
		current->ID = 0;
		int length = line.length();
		for (int i = 12; i < length; i++)
			current->ID = current->ID * 10 + (line[i] - '0');
		getline(fin, line);
		for (int i = 9; i < length; i++) {
			while (i < length && !isalnum(line[i]))
				i++;
			string temp;
			while (i < length && isalnum(line[i])) {
				temp += tolower(line[i]);
				i++;
			}
			current->words.insert(temp);
		}
		getline(fin, current->to);
		current->to.erase(0, 4);
		getline(fin, line);

		getline(fin, line, '\0');
		length = line.length();
		// cout << length << endl;
		for (int i = 0; i < length; i++) {
			while (i < length && !isalnum(line[i]))
				i++;
			string temp;
			while (i < length && isalnum(line[i])) {
				temp.push_back(tolower(line[i]));
				i++;
			}
			// cout << temp << endl;
			current->count += temp.size();
			current->words.insert(temp);
		}
		

		ID2path[current->ID] = path;
		from2Mail[current->from].push_back(current);
		to2Mail[current->to].push_back(current);
		path2Mail[path] = current;
		count2Mail[current->count].insert(current);
		date2Mail[current->date].insert(current);
		fin.close();
	} 
	if (added.find(path) == added.end()) {
		N++;
		added.insert(path);
		printf("%d\n", N);
	} else 
		printf("-\n");
}
void remove() {
	int id;
	cin >> id;
	string path = ID2path[id];
	unordered_set<string>::iterator itr = added.find(path);
	if (itr != added.end()) {
		added.erase(path);
		N--;
		printf("%d\n", N);
		Mail *ptr = path2Mail[path];
		string to = ptr->to;
		string from = ptr->from;
		int date = ptr->date;
		int count = ptr->count;
		vitr = lower_bound(from2Mail[from].begin(), from2Mail[from].end(), ptr);
		// vitr = from2Mail[from].find(ptr);
		from2Mail[from].erase(vitr);
		vitr = lower_bound(to2Mail[to].begin(), to2Mail[to].end(), ptr);
		// vitr = to2Mail[to].find(ptr);
		to2Mail[to].erase(vitr);
		date2Mail[date].erase(ptr);
		count2Mail[count].erase(ptr);
	} else 
		printf("-\n");
}

void longest() {
	if (N != 0)
		cout << (*((*(count2Mail.begin())).second.begin()))->ID << " " << (*((*(count2Mail.begin())).second.begin()))->count << '\n';
	else
		printf("-\n");
}

void query() {
	string s;
	set<Mail *, cmpByID> answer;
	set<Mail *>::iterator itr;
	bool only = true;
	while (cin >> s) {
		// cout << s << endl;
		if (s[0] != '-') {
			stack<Token> operators;
			vector<Token> postfix;
			int length = s.size();
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
			/*for (int i = 0; i < postfix.size(); i++)
				cout << postfix[i].type << " " << postfix[i].op << " " << postfix[i].expression << '\n';*/
			if (!only) {
				for (itr = answer.begin(); itr != answer.end();) {
					vector<bool> check;
					int length = postfix.size();
					for (int i = 0; i < length; i++) {
						if (postfix[i].type == 1) {
							if ((*itr)->words.find(postfix[i].expression) != (*itr)->words.end())
								check.push_back(true);
							else 
								check.push_back(false);
						} else {
							int back = check.size() - 1;
							// cout << back << endl;
							if (postfix[i].op == 2) 
								check[back] = !check[back];
							else if (postfix[i].op == 3) {
								// cout << check[back] << " " << check[back - 1] << endl;
								check[back - 1] = (check[back] & check[back - 1]);
								// cout << check[back - 1] << endl;
								check.pop_back();
							} else {
								check[back - 1] = (check[back] | check[back - 1]);
								check.pop_back();
							}
						}
					}
					assert(check.size() == 1);
					if (check[0] == false) {
						answer.erase(*(itr++));
					} else 
						itr++;
				}		
			} else {
				unordered_set<string>::iterator itr;
				for (itr = added.begin(); itr != added.end(); itr++) {
					Mail *here = path2Mail[*itr];
					/*unordered_set<string>::iterator a;
					for (a = here->words.begin(); a != here->words.end(); a++)
						cout << *a << endl;*/
					vector<bool> check;
					int length = postfix.size();
					for (int i = 0; i < length; i++) {
						if (postfix[i].type == 1)
							check.push_back((here->words.find(postfix[i].expression) != here->words.end()));
						else {
							int back = check.size() - 1;
							if (postfix[i].op == 2) 
								check[back] = !check[back];
							else if (postfix[i].op == 3) {
								check[back - 1] = (check[back] & check[back - 1]);
								check.pop_back();
							} else {
								check[back - 1] = (check[back] | check[back - 1]);
								check.pop_back();
							}
						}
					}
					assert(check.size() == 1);
					if (check[0])
						answer.insert(path2Mail[*itr]);
				}
			}
			break;
		} else if (s[1] == 'f') {
			s.erase(0, 3);
			s.pop_back();
			for (itr = answer.begin(); itr != answer.end();) {
				if ((*itr)->from != s)
					answer.erase(*(itr++));
				else
					itr++;
			}
			for (vitr = from2Mail[s].begin(); vitr != from2Mail[s].end(); vitr++)
				answer.insert(*vitr);
			only = false;
			cout << "f: " << answer.size() << endl;
		} else if (s[1] == 't') {
			s.erase(0, 3);
			s.pop_back();
			for (itr = answer.begin(); itr != answer.end();) {
				if ((*itr)->to != s)
					answer.erase(*(itr++));
				else
					itr++;
			}
			cout << "size of to2mail[s]:" << to2Mail[s].size() << endl;
			for (vitr = to2Mail[s].begin(); vitr != to2Mail[s].end(); vitr++) 
				answer.insert(*vitr);
			only = false;
			cout << "t: " << answer.size() << endl;
		} else if (s[1] == 'd') {
			s.erase(0, 2);
			int index = s.find('~');
			// cout << index << endl;
			int start = INT_MIN;
			int end = INT_MAX;
			if (index == 0 && s.length() != 1) {
				s.erase(0, 1);
				end = nepoch(s);
			} else if (index == 12) {
				start = nepoch(s);
				s.erase(0, 13);
				if (s.length() != 0) 
					end = nepoch(s);
			}
			// cout << start << " " << end << endl;
			for (itr = answer.begin(); itr != answer.end();) {
				if ((*itr)->date < start || (*itr)->date > end) 
					answer.erase(*(itr++));
				else
					itr++;
			}
			for (sitr = date2Mail.lower_bound(start); sitr != date2Mail.end(); sitr++) {
				if ((*(sitr->second.begin()))->date >= start && (*(sitr->second.begin()))->date <= end) 
					for (ssitr = (*sitr).second.begin(); ssitr != (*sitr).second.end(); ssitr++)
						answer.insert(*ssitr);
				else
					break;
			}
			only = false;
		}
	}
	bool start = true;
	// cout << "size: " << answer.size() << endl;
	// cout << "size: " << added.size() << endl;
	for (itr = answer.begin(); itr != answer.end(); itr++)
		if (added.find((*itr)->path) != added.end()) {
			if (start)
				start = false;
			else
				printf(" ");
			printf("%d", (*itr)->ID);
		}
	if (start)
		printf("-\n");
	else 
		printf("\n");
}
int main() {
	/*string k[] = {"!","&","|","("};
	int v[] =    { 0,  1,  2,  3};
	for (int i = 0; i < 4; i++)
		precedence[k[i]] = v[i];*/

	string s;
	while (cin >> s) {
		if (s[0] == 'a') {
			add();
		} else if (s[0] == 'r') {
			remove();
		} else if (s[0] == 'l') {
			longest();
		} else { 
			query();
		}
	}
	return 0;
}
