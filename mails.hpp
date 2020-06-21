#ifndef MAILS_HPP

#define MAILS_HPP

#pragma once

#include <set>
#include <map>
#include <queue>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <functional>
#include <unordered_map>
// 月份對應表
static const std::unordered_map<std::string, int> month_to_number =
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
inline void string_lower(std::string &str)
{
	for (auto &it : str)
		if (std::isupper(it))
			it = std::tolower(it);
	return;
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
	friend class Mails;

public:
	Mail()
	{
		charcount = 0;
		isopen = false;
	}
	Mail(const std::string &p)
	{
		std::ifstream file(p);
		if (file.fail())
			throw std::runtime_error(p + ": No such file or directory.");
		file.sync_with_stdio(false);
		file.tie(NULL);
		path = p;
		isopen = true;
		std::string tmp(50, '\0');
		// From from
		std::getline(file, tmp);
		from = &tmp.at(6);
		string_lower(from);
		// Date day
		std::getline(file, tmp);
		auto it_str = tmp.begin() + 6;
		for (; it_str < tmp.end(); ++it_str)
		{
			if (*it_str == ' ')
			{
				++it_str;
				break;
			}
			datetime.day = datetime.day * 10 + (*it_str - '0');
		}
		auto month_start = it_str;
		std::string tmp2(12, '\0');
		for (; it_str < tmp.end(); ++it_str)
		{
			if (*it_str == ' ')
			{
				*it_str = '\0';
				++it_str;
				break;
			}
		}
		tmp2 = &(*month_start);
		auto it = month_to_number.find(tmp2);
		if (it == month_to_number.cend())
			throw std::out_of_range("invalid month: " + tmp2);
		datetime.month = it->second;
		for (; it_str < tmp.end(); ++it_str)
		{
			if (*it_str == ' ')
			{
				++it_str;
				break;
			}
			datetime.year = datetime.year * 10 + (*it_str - '0');
		}
		it_str += 3;
		datetime.hour = (*it_str - '0') * 10 + (*(it_str + 1) - '0');
		datetime.minute = (*(it_str + 3) - '0') * 10 + (*(it_str + 4) - '0');
		// Message-ID
		datetime.to_number();
		std::getline(file, tmp);
		MessageID = std::stoi(std::string(tmp.begin() + 12, tmp.end()));
		// Subject
		std::getline(file, tmp);
		for (auto iter = tmp.begin() + 8; iter < tmp.end(); ++iter)
		{
			while (iter < tmp.end() && !std::isalnum(*iter))
				++iter;
			auto start = iter;
			while (iter < tmp.end() && std::isalnum(*iter))
			{
				if (std::isupper(*iter))
					*iter = std::tolower(*iter);
				++iter;
			}
			content.insert(std::string(start, iter));
		}
		// To
		std::getline(file, tmp);
		to = std::string(tmp.begin() + 4, tmp.end());
		string_lower(to);
		// content:
		std::getline(file, tmp);
		// real content
		charcount = 0;
		while (std::getline(file, tmp))
		{
			for (auto iter = tmp.begin(); iter < tmp.end(); ++iter)
			{
				while (iter < tmp.end() && !std::isalnum(*iter))
					++iter;
				auto start = iter;
				while (iter < tmp.end() && std::isalnum(*iter))
				{
					if (isupper(*iter))
						*iter = std::tolower(*iter);
					++iter;
				}
				std::string ins(start, iter);
				charcount += ins.length();
				content.insert(ins);
			}
		}
		file.close();
	}
	void print(std::ostream &out = std::cout) const
	{
		const std::string number_to_month[12] =
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
		out << "From: " << from << '\n';
		out << "Date: " << datetime.day << ' '
			<< number_to_month[datetime.month] << ' '
			<< datetime.year << " at "
			<< datetime.hour << ':'
			<< datetime.minute << '\n';
		out << "Message-ID: " << MessageID << '\n';
		out << "Subject: ";
		out << "\nTo: " << to << '\n';
		out << "Content:\n";
		for (const auto &str : content)
			out << str << ' ';
	}
	friend std::ostream &operator<<(std::ostream &out, const Mail &mail)
	{
		mail.print(out);
		return out;
	}
	int charcount;
	bool isopen;
	int MessageID;

private:
	struct Datetime
	{
		Datetime()
		{
			year = 0;
			month = 0;
			day = 0;
			hour = 0;
			minute = 0;
			time_number = 0;
		}
		inline void to_number()
		{
			time_number = year * 100000000LL + month * 1000000LL +
						  day * 10000LL + hour * 100LL + minute * 1LL;
		}
		int year, month, day, hour, minute;
		long long time_number;
	};
	Datetime datetime;
	std::string from, to;
	std::string path;
	std::set<std::string> content;
};
template <>
struct std::less<Mail *>
{
	bool operator()(const Mail *x, const Mail *y)
	{
		if (x->charcount == y->charcount)
			return x->MessageID < y->MessageID;
		return x->charcount < y->charcount;
	}
};
class Mails
{
public:
	Mails()
	{
		count = 0;
		maildata_ID.reserve(10000);
		maildata_path.reserve(10000);
	}
	void add(const std::string &path, std::ostream &out = std::cout)
	{
		auto it = maildata_path.find(path);
		if (it != maildata_path.end())
		{
			if (it->second.isopen)
			{
				out << "-\n";
				return;
			}
			else
			{
				it->second.isopen = true;
				maildata_heap.push(&it->second);
				out << ++count << '\n';
				return;
			}
		}
		else
		{
			Mail mail(path);
			maildata_path[path] = mail;
			maildata_ID[mail.MessageID] = &maildata_path[path];
			maildata_heap.push(&maildata_path[path]);
			out << ++count << '\n';
		}
	}
	void remove(const int &id, std::ostream &out = std::cout)
	{
		auto it = maildata_ID.find(id);
		if (it == maildata_ID.end() || !it->second->isopen)
		{
			out << "-\n";
			return;
		}
		it->second->isopen = false;
		maildata_heap.pop();
		out << --count << '\n';
	}
	/* -f <from>
	 * -t <to>
	 * -d <date>~<date>
	 * <expression>:<keyword> or (<expression>) or !<expression>
	 * or <expression>|<expression> or <expression>&<expression>
	 * 
	 * 
	 */
	void query(const std::string &commands, std::ostream &out = std::cout)
	{
		std::cout << commands << '\n';
		return;
		std::string from, to;
		long long start_time, end_time;
		std::string::const_iterator start;
		for (auto it = commands.cbegin() + 1; it < commands.cend(); ++it)
		{
			if (*(it++) == '-')
			{
				switch (*it)
				{
				case 'f':
					start = ++it;
					while (*(++it) != '\"')
						;
					from = std::string(start + 1, it - 1);
					break;
				case 't':
					start = ++it;
					while (*(++it) != '\"')
						;
					to = std::string(start + 1, it - 1);
					break;
				case 'd':
					start = ++it;
					while (it < commands.end() && *it != ' ')
						++it;
					sscanf(std::string(start, it - 1).c_str(), "%lld~%lld", &start_time, &end_time);
					break;
				default:
					throw std::out_of_range("unknown option: -" + *it);
					break;
				}
			}
			else
			{
			}
		}
		out << commands << '\n';
		out << "-\n";
		return;
	}
	void longest(std::ostream &out = std::cout) const
	{
		if (count == 0)
		{
			out << "-\n";
			return;
		}
		out << maildata_heap.top()->MessageID << ' '
			<< maildata_heap.top()->charcount << '\n';
	}

private:
	int count;
	std::unordered_map<std::string, Mail> maildata_path;
	std::unordered_map<int, Mail *> maildata_ID;
	std::priority_queue<Mail *> maildata_heap;
};

#endif