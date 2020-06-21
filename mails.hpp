#ifndef MAILS_HPP

#define MAILS_HPP

#pragma once

#include <set>
#include <map>
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
}
/* 信件格式範例
 * From: Tim
 * Date: 19 May 2011 at 16:50
 * Message-ID: 1
 * Subject: blah-blah
 * To: Tony
 * Content:
 * blah
 * blah-blah
 */
class Mail
{
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
		from.reserve(1000);
		to.reserve(1000);
		path = p;
		isopen = true;
		std::string tmp;
		tmp.reserve(100000);
		// From from
		std::getline(file, tmp);
		from = &tmp.at(6);
		string_lower(from);
		// Date
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
		maildata_ID.reserve(100000);
		maildata_path.reserve(100000);
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
				maildata_heap.insert(&it->second);
				out << ++count << '\n';
				return;
			}
		}
		else
		{
			Mail mail(path);
			maildata_path[path] = mail;
			maildata_ID.insert(std::pair<int, Mail *const>(mail.MessageID, &maildata_path[path]));
			maildata_heap.insert(&maildata_path[path]);
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
		out << --count << '\n';
	}
	/* 
	 * -f <from>
	 * -t <to>
	 * -d <date>~<date>
	 * <expression>:<keyword> or (<expression>) or !<expression>
	 * or <expression>|<expression> or <expression>&<expression>
	 */
	void query(const std::string &commands, std::ostream &out = std::cout)
	{
		for (int i = 0; i < 3; i++)
		{
			if (answers[i].command == commands)
			{
				auto it = answers[i].answer.cbegin();
				for (; it != answers[i].answer.cend() - 1; ++it)
					cout << *it << ' ';
				cout << *it;
				return;
			}
		}
		std::string from, to, expression;
		std::string::const_iterator start;
		long long start_time, end_time;
		for (auto it = commands.cbegin() + 1; it < commands.cend(); ++it)
		{
			if (*it == ' ')
				continue;
			else if (*it == '-')
			{
				++it;
				switch (*it)
				{
				case 'f':
					it += 2;
					start = it;
					while (*it != '\"')
						++it;
					from = std::string(start, it);
					break;
				case 't':
					it += 2;
					start = it;
					while (*it != '\"')
						++it;
					to = std::string(start, it);
					break;
				case 'd':
					start = ++it;
					while (it < commands.end() && *it != '~')
						++it;
					if (start == it)
						start_time = 0;
					else
						start_time = std::stoll(std::string(start, it));
					start = ++it;
					while (it < commands.end() && *it != ' ')
						++it;
					if (start == it)
						end_time = 0x7FFFFFFFFFFFFFFFLL;
					else
						end_time = std::stoll(std::string(start, it));
					break;
				default:
					throw std::out_of_range("unknown option: -" + *it);
					break;
				}
			}
			else
			{
				expression = std::string(it, commands.cend());
				break;
			}
		}
		parseExpression();
		return;
	}
	void longest(std::ostream &out = std::cout) const
	{
		if (count == 0)
		{
			out << "-\n";
			return;
		}
		out << (*maildata_heap.crbegin())->MessageID << ' '
			<< (*maildata_heap.crbegin())->charcount << '\n';
	}

private:
	void parseExpression()
	{
	}
	struct Answer
	{
		Answer()
		{
			answer.reserve(10000);
			command.reserve(5000);
			for (auto &it : answer)
				it.reserve(5000);
		}
		std::vector<int> answer;
		std::string command;
	};
	int count;
	std::unordered_map<std::string, Mail> maildata_path;
	std::unordered_map<int, Mail *const> maildata_ID;
	std::set<Mail *, std::less<Mail *>> maildata_heap;
	Answer answers[3];
};

#endif