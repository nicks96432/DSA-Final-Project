#include <iostream>
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
	for (int i = 1; i <= 10000; i++)
	{
		
		string str = "../final/test_data/mail";
		str.append(to_string(i));
		mails.add(str);
	}
	// while (cin >> command)
	// {
	// 	auto it = commands.find(command);
	// 	if (it == commands.cend())
	// 		throw out_of_range(command + ": Invalid command");
	// 	switch (it->front())
	// 	{
	// 	case 'a':
	// 		cin >> path;
	// 		mails.add(path);
	// 		break;
	// 	case 'r':
	// 		cin >> id;
	// 		mails.remove(id);
	// 		break;
	// 	case 'q':
	// 		getline(cin, path);
	// 		mails.query(path);
	// 		break;
	// 	default: // longest
	// 		mails.longest();
	// 		break;
	// 	}
	// }
	return 0;
}
