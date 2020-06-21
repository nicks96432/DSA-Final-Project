#include <iostream>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <map>
#include <cassert>
#include <fstream>
using namespace std;
struct Mail {
	string from;
	string to;
	int date;
	int ID;
	int count;
	unordered_set<string> words;
	Mail(): ID(0), count(0) {}
};
struct cmpByID {
    bool operator() (Mail *a, Mail *b) const {
        return a->ID < b->ID;
    }
};
int what_month(string &s, int i);
int tolower(char c);
void s_tolower(string &s);
bool isalnum(char c);
int epoch(string &s);
unordered_map<string, Mail *> paths; // was loaded
unordered_map<int, string> ID2path;
unordered_set<string> valid; // was valid
map<int, set<Mail *, cmpByID> > count2Mail;
int N;
void add() {
	string path;
	cin >> path;
	if (paths.find(path) == paths.end()) {
		N++;
		printf("%d\n", N);
		ifstream fin;
		fin.open(path, ifstream::in);
		Mail *current = new Mail;
		paths[path] = current;
		valid.insert(path);
		getline(fin, current->from);
		assert(current->from[4] == ':');
		current->from.erase(0, 6);
		s_tolower(current->from);
		// cout << current->from << endl;
		string line;
		getline(fin, line);
		assert(line[4] == ':');
		line.erase(0, 6);
		current->date = epoch(line);
		getline(fin, line);
		assert(line[10] == ':');
		line.erase(0, 12);
		for (int i = 0; i < line.length(); i++)
			current->ID = current->ID * 10 + (line[i] - '0');
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
				current->words.insert(t);
			}
		}
		getline(fin, current->to);
		assert(current->to[2] == ':');
		current->to.erase(0, 4);
		s_tolower(current->to);
		getline(fin, line);
		getline(fin, line, '\0');
		l = line.length();
		for (int i = 0; i < l;) {
			while (i < l && !isalnum(line[i]))
				i++;
			string t;
			while (i < l && isalnum(line[i])) {
				t.push_back(tolower(line[i]));
				current->count++;
				i++;
			}
			if (t.size() != 0) {
				// cout << "content: " << t << " " << t.size() << endl;
				current->words.insert(t);
			}
		}
		ID2path[current->ID] = path;
		count2Mail[current->count].insert(current);
		/*cout << endl << endl;
		cout << "From: " << current->from << endl;
		cout << "date: " << current->date << endl;
		cout << "to: " << current->to << endl;
		cout << "ID: " << current->ID << endl;
		cout << "subject: " << line << endl;*/
	} else {
		if (valid.find(path) == valid.end()) {
			valid.insert(path);
			Mail *current = paths[path];
			count2Mail[current->count].insert(current);
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
	string path = ID2path[id];
	unordered_set<string>::iterator itr;
	itr = valid.find(path);
	if (itr != valid.end()) {
		N--;
		Mail *ptr = paths[path];
		printf("%d\n", N);
		valid.erase(itr);
		// set<Mail *, cmpByID>::iterator tr = count2Mail[id]
		assert(count2Mail[ptr->count].erase(ptr) == 1);
	} else {
		printf("-\n");
	}
}
void longest() {
	if (N != 0) {
		set<Mail *, cmpByID>::iterator itr = (count2Mail.rbegin()->second).begin();
		printf("%d %d\n", (*itr)->ID, (*itr)->count);
	}
	else
		printf("-\n");
}
void query() {

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
