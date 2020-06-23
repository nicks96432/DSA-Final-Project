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
#include <bitset>
#include <ext/pb_ds/assoc_container.hpp>
using namespace std;
using namespace __gnu_pbds;
struct Mail {
	string path;
	string from;
	string to;
	int date;
	int count;
	gp_hash_table<string, bool> words;
	Mail(): count(0) {}
};
struct Token {
	int type;
	string expression;
	int op;
	int next_is_binary;
	Token() : type(0), next_is_binary(0) {};
	Token(Token const &b): type(b.type), expression(b.expression), op(b.op) {};
	Token operator=(Token const &a) { Token b(a); return b; }
};
int what_month(string &s, int i);
int tolower(char c);
void s_tolower(string &s);
bool isalnum(char c);
int epoch(string &s);
int nepoch(string &s);
gp_hash_table<string, int> paths; // path was loaded, to ID
gp_hash_table<string, gp_hash_table<int, bool> > from2ID;
gp_hash_table<string, gp_hash_table<int, bool> > to2ID;
Mail mails[10001];
gp_hash_table<string, bool> valid; // path was valid
map<int, set<int> > count2ID;
map<int, set<int> > date2ID;
bitset<20> check;
int N;
void add() {
	string path;
	cin >> path;
	if (paths.find(path) == paths.end()) {
		N++;
		printf("%d\n", N);
		ifstream fin;
		fin.open(path, ifstream::in);
		string from;
		getline(fin, from);
		from.erase(0, 6);
		s_tolower(from);
		// cout << current.from << endl;
		string line;
		getline(fin, line);
		// assert(line[4] == ':');
		line.erase(0, 6);
		int date = epoch(line);
		getline(fin, line);
		// assert(line[10] == ':');
		line.erase(0, 12);
		int ID = 0;
		for (int i = 0; i < line.length(); i++)
			ID = ID * 10 + (line[i] - '0');
		mails[ID].from = from;
		mails[ID].path = path;
		mails[ID].date = date;
 		getline(fin, line);
		// assert(line[7] == ':');
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
			// if (t.size() != 0) {
				// cout << "subject: " << t << " " << t.size() << endl;
				mails[ID].words[t] = true;
			// }
		}
		getline(fin, mails[ID].to);
		// assert(mails[ID].to[2] == ':');
		mails[ID].to.erase(0, 4);
		s_tolower(mails[ID].to);
		getline(fin, line);
		getline(fin, line, '\0');
		l = line.length();
		for (int i = 0; i < l;) {
			while (i < l && !isalnum(line[i]))
				i++;
			string t;
			while (i < l && isalnum(line[i])) {
				t.push_back(tolower(line[i]));
				i++;
			}
			// if (t.size() != 0) {
				// cout << "content: " << t << " " << t.size() << endl;
				mails[ID].words[t] = true;
				mails[ID].count += t.size();
			// }
		}
		
		valid[path] = true;
		paths[path] = ID;
		from2ID[mails[ID].from][ID] = true;
		to2ID[mails[ID].to][ID] = true;
		count2ID[mails[ID].count].insert(ID);
		date2ID[mails[ID].date].insert(ID);
		/*cout << endl << endl;
		cout << "From: " << current->from << endl;
		cout << "date: " << current->date << endl;
		cout << "ID: " << current->ID << endl;
		cout << "to: " << current->to << endl;
		cout << "subject: " << line << endl;*/
	} else {
		if (valid.find(path) == valid.end()) {
			valid[path] = true;
			int id = paths[path];
			Mail *current = &mails[id];
			from2ID[current->from][id] = true;
			to2ID[current->to][id] = true;
			count2ID[current->count].insert(id);
			date2ID[current->date].insert(id);
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
	if (valid.find(path) != valid.end()) {
		N--;
		printf("%d\n", N);
		count2ID[mails[id].count].erase(id);
		from2ID[mails[id].from].erase(id);
		to2ID[mails[id].to].erase(id);
		date2ID[mails[id].date].erase(id);
		valid.erase(path);
	} else {
		printf("-\n");
	}
}
void longest() {
	if (N != 0) {
		// bool ok = false;
		map<int, set<int> >::reverse_iterator itr;
		for (itr = count2ID.rbegin(); itr != count2ID.rend(); itr++) {
			set<int>::iterator a = itr->second.begin();
			if (a != itr->second.end()) {
				printf("%d %d\n", *a, mails[*a].count);
				break;
			}
		}
	}
	else
		printf("-\n");
}
void query() {
	string s;
	set<int> answer;
	string from, to;
	bool only = true;
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
					do {
						current.expression.push_back(tolower(s[i]));
						i++;
					} while (i < length && isalnum(s[i]));
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
						do {
							postfix.push_back(operators.top());
							operators.pop();
						} while (operators.top().op != 6);
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
			for (int i = postfix.size() - 2; i >= 0; i--)
				if (!postfix[i].type && !postfix[i + 1].type && postfix[i].op == 2 && postfix[i + 1].op == 2)
					postfix.erase(postfix.begin() + i, postfix.begin() + i + 2);
			int size = postfix.size() - 1;
			for (int i = 0; i < size; i++) {
				if (postfix[i].type && !postfix[i + 1].type && (postfix[i + 1].op == 3 || postfix[i + 1].op == 4))
					postfix[i].next_is_binary = postfix[i + 1].op;
				else
					postfix[i].next_is_binary = 0;
			}
			postfix[size].next_is_binary = 0;
			if (only) {
				for (auto itr = valid.begin(); itr != valid.end(); itr++) {
					int i = paths[itr->first];
					int length = postfix.size();
					int top = 0;
					for (int j = 0; j < length; j++) {
						if (postfix[j].type == 1) {
							if (postfix[j].next_is_binary == 3) {
								if (mails[i].words.find(postfix[j].expression) == mails[i].words.end())
									check.reset(top - 1);
								j++;
							} else if (postfix[j].next_is_binary == 4) {
								if (mails[i].words.find(postfix[j].expression) != mails[i].words.end())
									check.set(top - 1);
								j++;
							} else {
								if (mails[i].words.find(postfix[j].expression) != mails[i].words.end())
									check.set(top);
								else
									check.reset(top);
								top++;
							}
						} else {
							int back = check.size() - 1;
							if (postfix[j].op == 2) 
								check.flip(top - 1);
							else if (postfix[j].op == 3) {
								top--;
								if (check.test(top) & check.test(top - 1))
									check.set(top - 1);
								else
									check.reset(top - 1);
							} else {
								top--;
								if (check.test(top) | check.test(top - 1))
									check.set(top - 1);
								else
									check.reset(top - 1);
							}
						}
					}
					// assert(check.size() == 1);
					if (check[0])
						answer.insert(i);
				}
			} else {
				if (!from.empty()) {
					for (auto itr = from2ID[from].begin(); itr != from2ID[from].end(); itr++) {
						int i = itr->first;
						if (!to.empty() && mails[i].to != to)
							continue;
						if (mails[i].date < dateStart)
							continue;
						if (mails[i].date > dateEnd)
							continue;
						int length = postfix.size();
						int top = 0;
						for (int j = 0; j < length; j++) {
							if (postfix[j].type == 1) {
								if (postfix[j].next_is_binary == 3) {
									if (mails[i].words.find(postfix[j].expression) == mails[i].words.end())
										check.reset(top - 1);
									j++;
								} else if (postfix[j].next_is_binary == 4) {
									if (mails[i].words.find(postfix[j].expression) != mails[i].words.end())
										check.set(top - 1);
									j++;
								} else {
									if (mails[i].words.find(postfix[j].expression) != mails[i].words.end())
										check.set(top);
									else
										check.reset(top);
									top++;
								}
							} else {
								int back = check.size() - 1;
								if (postfix[j].op == 2) 
									check.flip(top - 1);
								else if (postfix[j].op == 3) {
									top--;
									if (check.test(top) & check.test(top - 1))
										check.set(top - 1);
									else
										check.reset(top - 1);
								} else {
									top--;
									if (check.test(top) | check.test(top - 1))
										check.set(top - 1);
									else
										check.reset(top - 1);
								}
							}
						}
						// assert(check.size() == 1);
						if (check[0])
							answer.insert(i);
					}
				} else if (!to.empty()) {
					for (auto itr = to2ID[to].begin(); itr != to2ID[to].end(); itr++) {
						int i = itr->first;
						if (mails[i].date < dateStart)
							continue;
						if (mails[i].date > dateEnd)
							continue;
						int length = postfix.size();
						int top = 0;
						for (int j = 0; j < length; j++) {
							if (postfix[j].type == 1) {
								if (postfix[j].next_is_binary == 3) {
									if (mails[i].words.find(postfix[j].expression) == mails[i].words.end())
										check.reset(top - 1);
									j++;
								} else if (postfix[j].next_is_binary == 4) {
									if (mails[i].words.find(postfix[j].expression) != mails[i].words.end())
										check.set(top - 1);
									j++;
								} else {
									if (mails[i].words.find(postfix[j].expression) != mails[i].words.end())
										check.set(top);
									else
										check.reset(top);
									top++;
								}
							} else {
								int back = check.size() - 1;
								if (postfix[j].op == 2) 
									check.flip(top - 1);
								else if (postfix[j].op == 3) {
									top--;
									if (check.test(top) & check.test(top - 1))
										check.set(top - 1);
									else
										check.reset(top - 1);
								} else {
									top--;
									if (check.test(top) | check.test(top - 1))
										check.set(top - 1);
									else
										check.reset(top - 1);
								}
							}
						}
						// assert(check.size() == 1);
						if (check[0])
							answer.insert(i);
					}
				} else { 
					map<int, set<int> >::iterator itr = date2ID.lower_bound(dateStart);
					for (; itr != date2ID.end(); itr++) {
						if (itr->first > dateEnd)
							break;
						set<int>::iterator sitr;
						for (sitr = itr->second.begin(); sitr != itr->second.end(); sitr++) {
							int i = *sitr;
							int length = postfix.size();
							int top = 0;
							for (int j = 0; j < length; j++) {
								if (postfix[j].type == 1) {
									if (postfix[j].next_is_binary == 3) {
										if (mails[i].words.find(postfix[j].expression) == mails[i].words.end())
											check.reset(top - 1);
										j++;
									} else if (postfix[j].next_is_binary == 4) {
										if (mails[i].words.find(postfix[j].expression) != mails[i].words.end())
											check.set(top - 1);
										j++;
									} else {
										if (mails[i].words.find(postfix[j].expression) != mails[i].words.end())
											check.set(top);
										else
											check.reset(top);
										top++;
									}
								} else {
									int back = check.size() - 1;
									if (postfix[j].op == 2) 
										check.flip(top - 1);
									else if (postfix[j].op == 3) {
										top--;
										if (check.test(top) & check.test(top - 1))
											check.set(top - 1);
										else
											check.reset(top - 1);
									} else {
										top--;
										if (check.test(top) | check.test(top - 1))
											check.set(top - 1);
										else
											check.reset(top - 1);
									}
								}
							}
							// assert(check.size() == 1);
							if (check.test(0))
								answer.insert(i);
						}
					}
				}
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
				s.erase(0, 1);
				dateEnd = nepoch(s);
			} else if (s.length() > 12 && s[12] == '~') {
				dateStart = nepoch(s);
				s.erase(0, 13);
				if (s.length() != 0)
					dateEnd = nepoch(s);
			}
		}
		only = false;
	}
	bool start = true;
	set<int>::iterator itr;
	for (itr = answer.begin(); itr != answer.end(); itr++) {
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
	string s;
	while (cin >> s) {
		if (s[0] == 'a') 
			add();
		else if (s[0] == 'r')
			remove();
		else if (s[0] == 'l')
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
	// assert('0' <= s[0] && s[0] <= '9'); 
	int day, month;
	int month_start = 2;
	if ('0' <= s[1] && s[1] <= '9') {
		day = (s[0] - '0') * 10 + (s[1] - '0');
		// cout << day << endl;
		// assert(1 <= day && day <= 31);
		month_start = 3;
	} else {
		day = (s[0] - '0');
		// assert(1 <= day && day <= 31);
	}
	month = what_month(s, month_start);
	// assert(1 <= month && month <= 12);
	int index = s.rfind(':');
	// year = (s[index - 8] - '0') * 10 + (s[index - 7] - '0');
	// assert(6 <= year && year <= 12);
	// hour = ((s[index - 2] - '0') * 10 + (s[index - 1] - '0'));
	// assert(0 <= hour && hour <= 23);
	// minute = ((s[index + 1] - '0') * 10 + (s[index + 2] - '0'));
	// assert(0 <= minute && minute <= 59);
	// cout << "time: " << minute << " " << hour << " " << day << " " << month << " " << year << endl;
 	return ((s[index + 1] - '0') * 10 + (s[index + 2] - '0')) 
 	+ ((s[index - 2] - '0') * 10 + (s[index - 1] - '0')) * 64 
 	+ day * 2048 
 	+ month * 65536 
 	+ ((s[index - 8] - '0') * 10 + (s[index - 7] - '0')) * 1048576;
}
int nepoch(string &s) {
	// int year, month, day, hour, minute;
	return 1048576 * ((s[2] - '0') * 10 + (s[3] - '0'))
		+ 65536 * ((s[4] - '0') * 10 + (s[5] - '0'))
		+ 2048 * ((s[6] - '0') * 10 + (s[7] - '0'))
	    + 64 * ((s[8] - '0') * 10 + (s[9] - '0'))
		+ ((s[10] - '0') * 10 + (s[11] - '0'));
	// assert(1 <= day && day <= 31);
	// assert(1 <= month && month <= 12);
	// assert(6 <= year && year <= 12);
	// assert(0 <= hour && hour <= 23);
	// assert(0 <= minute && minute <= 59);
	// return minute + hour * 60 + day * 1440 + month * 46080 + year * 600000;
}