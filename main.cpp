#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <map>
using namespace std;

struct Content {
	string 
}
struct Mail {
	string from;
	string date;
	string ID;
	string subject;
	string to;
	Content content;
};
void add() {
	string path;
	cin >> path;
	fp = open(path, "r");

}

int main() {
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