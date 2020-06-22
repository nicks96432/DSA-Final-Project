#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
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
	while (cin >> command)
	{
		auto it = commands.find(command);
		if (it == commands.cend())
			throw out_of_range(command + ": Invalid command");
		switch (it->front())
		{
		case 'a':
			cin >> path;
			mails.add(path);
			break;
		case 'r':
			cin >> id;
			mails.remove(id);
			break;
		case 'q':
			getline(cin, path);
			mails.query(path);
			break;
		default: // longest
			mails.longest();
			break;
		}
	}
	return 0;
}
