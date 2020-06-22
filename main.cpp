#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#ifdef DEBUG
#include <ctime>
#endif
#include "mails.hpp"
using namespace std;
int main(int argc, char const *argv[])
{
	ios_base::sync_with_stdio(false);
	cin.tie(nullptr);
	const unordered_set<string> commands =
		{
			"longest",
			"add",
			"query",
			"remove"

		};
	Mails mails;
	string command, path;
	int id;
#ifdef DEBUG
	clock_t start = clock();
	ofstream out("out", ios::out);
#endif
	while (cin >> command)
	{
		auto it = commands.find(command);
		if (it == commands.cend())
			throw out_of_range(command + ": Invalid command");
		switch (it->front())
		{
		case 'a':
			cin >> path;
#ifdef DEBUG
			mails.add(path, out);
#else
			mails.add(path);
#endif
			break;
		case 'r':
			cin >> id;
#ifdef DEBUG
			mails.remove(id, out);
#else
			mails.remove(id);
#endif
			break;
		case 'q':
			getline(cin, path);
#ifdef DEBUG
			mails.query(path, out);
#else
			mails.query(path);
#endif
			break;
		default: // longest
#ifdef DEBUG
			mails.longest(out);
#else
			mails.longest();
#endif
			break;
		}
	}
#ifdef DEBUG
	clock_t end = clock();
	cout << "time elapsed: " << (double)(end - start) / CLOCKS_PER_SEC;
#endif
	return 0;
}
