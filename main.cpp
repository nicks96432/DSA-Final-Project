#include <iostream>
#include <fstream>
#include <string>
// #include "mails.hpp"
using namespace std;
int main(int argc, char const *argv[])
{
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);
	ifstream file("../final/test_data/mail1");
	string tmp;
	for (int _ = 0; _ < 7; _++)
	{
		file >> tmp;
		cout << tmp << '\n';
	}
	int n;
	file >> n;
	cout << n << '\n';
	file >> n;
	cout << n << '\n';
	file >> tmp;
	cout << tmp << '\n';
	file.close();
	return 0;
}
