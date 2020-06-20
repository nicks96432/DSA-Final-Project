#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include "mails.hpp"
using namespace std;
int main(int argc, char const *argv[])
{
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);
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
	ofstream out("out", ios::out);
	while (cin >> command)
	{
		auto it = commands.find(command);
		if (it == commands.cend())
			throw out_of_range(command + ": Invalid command");
		switch (it->front())
		{
		case 'a':
			cin >> path;
			mails.add(path,out);
			break;
		case 'r':
			cin >> id;
			mails.remove(id,out);
			break;
		case 'q':
			getline(cin, path);
			mails.query(path,out);
			break;
		default: // longest
			mails.longest(out);
			break;
		}
	}
	return 0;
}
