#ifndef MAILS_HPP

#define MAILS_HPP

#include <set>
#include <map>
#include <ctime>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <stdexcept>
// 月份對應表
static const std::map<std::string, int> month_to_number =
	{
		{"January", 0},
		{"February", 1},
		{"March", 2},
		{"April", 3},
		{"May", 4},
		{"June", 5},
		{"July", 6},
		{"August", 7},
		{"September", 8},
		{"October", 9},
		{"November", 10},
		{"December", 11}

};
static const std::string number_to_month[12] =
	{
		std::string("January"),
		std::string("February"),
		std::string("March"),
		std::string("April"),
		std::string("May"),
		std::string("June"),
		std::string("July"),
		std::string("August"),
		std::string("September"),
		std::string("October"),
		std::string("November"),
		std::string("December")

};
// 清空stringstream
static inline void reset(std::stringstream &ss)
{
	ss.clear();
	ss.str(std::string());
}
/* 信件格式範例
 * From: Tim
 * Date: 19 May 2011 at 16:50
 * Message-ID: 1
 * Subject: Minas-Rio 
 * To: Tony
 * Content:
 * Minas-Rio is an iron ore mining project in Brazil.[1] It is one of the world's largest mining projects, and is initially expected to export 26.5 million tonnes of iron ore in 2013, through a 525km slurry pipeline to a port at Au;[2] production potential is 53 Mtpa or higher.[3] The project was bought by Anglo American PLC, which is facing high costs.[4][5] The mine has certified reserves of 4.6 billion tonnes of itabirite.[6] There have been delays in starting the project, but in December 2010, Anglo American obtained a key license needed from Brazilian government before mining could start.[7][8] 
 * I read the paragraph on http://wikipedia.org
 */
class Mail
{
public:
	Mail() {}
	Mail(const std::string &p)
	{
		std::ifstream file(p);
		if (file.fail())
			throw std::runtime_error(p + ": No such file or directory.");
		path = p;
		std::string tmp;
		// From +  Date + at + Message + ID + Subject + To + Content = 35
		charcount = 35;
		// From
		file >> tmp >> from;
		charcount += from.length();
		// Date day
		file >> tmp >> tmp;
		charcount += tmp.length();
		datetime.tm_mday = std::stoi(tmp);
		// month
		file >> tmp;
		auto it = month_to_number.find(tmp);
		if (it == month_to_number.cend())
			throw std::out_of_range("invalid month");
		charcount += tmp.length();
		datetime.tm_mon = it->second;
		// year
		file >> tmp;
		datetime.tm_year = stoi(tmp);
		charcount += tmp.length();
		// at
		file >> tmp;
		// time
		char _; // :
		file >> tmp;
		charcount += tmp.length() - 1;
		std::stringstream ss;
		ss << tmp;
		ss >> datetime.tm_hour >> _ >> datetime.tm_min;
		reset(ss);
		// Message-ID
		file >> tmp >> MessageID;
		// Subject
		std::getline(file, tmp);
		ss >> tmp;

		// To
		file >> tmp >> to;
		// content
		file >> tmp;
		while (!(file >> tmp).fail())
		{
			for (auto it = tmp.begin(); it != tmp.end(); ++it)
				if (!std::isalnum(*it))
					*it = ' ';
			ss << tmp;
			while (ss >> tmp)
			{
				content.insert(tmp);
			}
			reset(ss);
		}
	}
	void print(std::ostream &out = std::cout) const
	{
		out << "From: " << from << '\n';
		out << "Date: " << datetime.tm_mday << ' '
			<< number_to_month[datetime.tm_mon] << ' '
			<< datetime.tm_year << " at "
			<< datetime.tm_hour << ':'
			<< datetime.tm_min << '\n';
		out << "Message-ID: " << MessageID << '\n';
		out << "Subject: ";
	}

private:
	std::tm datetime;
	std::string from, to;
	int MessageID;
	std::string path;
	std::set<std::string> content, subject;
	int charcount;
	bool isopen;
};
class Mails
{
public:
	Mails() {}
	void add(const std::string &path)
	{
		const Mail &mail = Mail(path);
	}
	void remove(const int &id)
	{
	}
	/* -f <from>
	 * -t <to>
	 * -d <date>~<date>
	 * <expression>:<keyword> or (<expression>) or !<expression>
	 * or <expression>|<expression> or <expression>&<expression>
	 * 
	 * 
	 */
	void query(const std::string &commands)
	{
	}

private:
};

#endif