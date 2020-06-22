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
		{"January", 1},
		{"February", 2},
		{"March", 3},
		{"April", 4},
		{"May", 5},
		{"June", 6},
		{"July", 7},
		{"August", 8},
		{"September", 9},
		{"October", 10},
		{"November", 11},
		{"December", 12}

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
		file.tie(nullptr);
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
	int charcount;
	bool isopen;
	int MessageID;
	Datetime datetime;
	std::string from, to;
	std::string path;
	std::set<std::string> content;
};
class Mails
{
public:
	Mails()
	{
		count = 0;
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
				maildata_longest.insert(&it->second);
				maildata_ID.insert({it->second.MessageID, &it->second});
				out << ++count << '\n';
				return;
			}
		}
		else
		{
			Mail mail(path);
			maildata_path[path] = mail;
			maildata_ID.insert({mail.MessageID, &maildata_path[path]});
			maildata_longest.insert(&maildata_path[path]);
			out << ++count << '\n';
		}
	}
	void remove(const int &id, std::ostream &out = std::cout)
	{
		auto it = maildata_ID.find(id);
		if (it == maildata_ID.end())
		{
			out << "-\n";
			return;
		}
		it->second->isopen = false;
		maildata_longest.erase(maildata_ID.at(id));
		maildata_ID.erase(id);
		out << --count << '\n';
	}
	void query(const std::string &commands, std::ostream &out = std::cout) const
	{
		std::string from, to, expression;
		std::string::const_iterator start;
		long long start_time = 0LL, end_time = 0LL;
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
					string_lower(from);
					break;
				case 't':
					it += 2;
					start = it;
					while (*it != '\"')
						++it;
					to = std::string(start, it);
					string_lower(to);
					break;
				case 'd':
					start = ++it;
					while (it < commands.end() && *it != '~')
						++it;
					if (start == it)
						start_time = ~0x7FFFFFFFFFFFFFFFLL;
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
		std::vector<int> tmp;
		if (from.empty() && to.empty() && start_time == 0LL && end_time == 0LL)
		{
			std::transform(maildata_ID.cbegin(), maildata_ID.cend(),
						   std::inserter(tmp, tmp.end()), [](const auto &pair) -> int { return pair.first; });
		}
		else if (!from.empty())
		{
			for (const auto &it : maildata_ID)
				if (it.second->from == from)
					tmp.push_back(it.first);
		}
		if (!to.empty())
		{
			std::vector<int> tmp2;
			for (const auto &it : maildata_ID)
				if (it.second->to == to)
					tmp2.push_back(it.first);
			if (tmp.empty())
				tmp = tmp2;
			else
			{
				std::vector<int> tmp3;
				std::set_intersection(tmp.cbegin(), tmp.cend(), tmp2.cbegin(),
									  tmp2.cend(), tmp3.begin());
				tmp = tmp3;
			}
		}
		if (start_time != 0LL || end_time != 0LL)
		{
			std::vector<int> tmp2;
			for (const auto &it : maildata_ID)
			{
				if (it.second->datetime.time_number >= start_time &&
					it.second->datetime.time_number <= end_time)
				{
					tmp2.push_back(it.first);
				}
			}
			if (tmp.empty())
				tmp = tmp2;
			else
			{
				std::vector<int> tmp3;
				std::set_intersection(tmp.cbegin(), tmp.cend(), tmp2.cbegin(),
									  tmp2.cend(), tmp3.begin());
				tmp = tmp3;
			}
		}
		std::vector<std::vector<int>> calculate;
		if (!expression.empty())
		{
			const std::vector<int> init = tmp;
			const std::vector<std::string> &postfix = to_postfix(expression);
			calculate.reserve(10000);
			for (const auto &it : postfix)
			{
				if (isalnum(it.front()))
				{
					std::vector<int> tmp_found;
					tmp_found.reserve(10000);
					for (const auto &it2 : tmp)
					{
						const std::set<std::string> &found = maildata_ID.at(it2)->content;
						if (found.find(it) != found.end())
							tmp_found.push_back(it2);
					}
					calculate.push_back(tmp_found);
				}
				else
				{
					if (it.front() == '!')
					{
						std::vector<int> tmp2;
						std::set_difference(init.cbegin(), init.cend(), calculate.back().cbegin(),
											calculate.back().cend(), std::inserter(tmp2, tmp2.end()));
						calculate.back() = tmp2;
					}
					else if (it.front() == '&')
					{
						if (calculate.size() < 2)
							throw std::out_of_range(std::string("calculate error: calculate.size() = ") +
													std::to_string(calculate.size()));
						std::vector<int> tmp2;
						std::set_intersection(calculate.crbegin()->cbegin(), calculate.crbegin()->cend(),
											  (calculate.crbegin() + 1)->cbegin(),
											  (calculate.crbegin() + 1)->cend(),
											  std::inserter(tmp2, tmp2.end()));
						calculate.pop_back();
						calculate.back() = tmp2;
					}
					else if (it.front() == '|')
					{
						if (calculate.size() < 2)
							throw std::out_of_range(std::string("calculate error: calculate.size() = ") +
													std::to_string(calculate.size()));
						std::vector<int> tmp2;
						std::set_union(calculate.crbegin()->cbegin(), calculate.crbegin()->cend(),
									   (calculate.crbegin() + 1)->cbegin(),
									   (calculate.crbegin() + 1)->cend(), std::inserter(tmp2, tmp2.end()));
						calculate.pop_back();
						calculate.back() = tmp2;
					}
				}
			}
		}
		else
		{
			calculate.push_back(tmp);
		}
		if (calculate.size() != 1)
			throw std::runtime_error(std::string("calculate error: calculate.size() = ") +
									 std::to_string(calculate.size()));
		if (calculate.front().empty())
		{
			out << "-\n";
			return;
		}
		out << *(calculate.front().cbegin());
		auto it = calculate.front().cbegin();
		it++;
		for (; it != calculate.front().cend(); ++it)
		{
			out << ' ' << *it;
		}
		out << "\n";
		return;
	}
	void longest(std::ostream &out = std::cout) const
	{
		if (count == 0)
		{
			out << "-\n";
			return;
		}
		out << (*maildata_longest.crbegin())->MessageID << ' '
			<< (*maildata_longest.crbegin())->charcount << '\n';
	}

private:
	const int priority(const char &s) const
	{
		enum operator_priority
		{
			NOT = 3,
			AND = 4,
			OR = 5,
		};
		if (s == '!')
			return NOT;
		else if (s == '&')
			return AND;
		else if (s == '|')
			return OR;
		else
			throw std::out_of_range(std::string("Invalid operator: ") + s);
	}
	const std::vector<std::string> to_postfix(const std::string &expression) const
	{
		std::string stack;
		std::vector<std::string> postfix;
		stack.reserve(100);
		postfix.reserve(100);
		auto it = expression.cbegin();
		while (it != expression.cend())
		{
			if (*it == '(')
			{
				stack.push_back(*it);
				++it;
			}
			else if (*it == ')')
			{
				while (stack.back() != '(')
				{
					std::string tmp(1, stack.back());
					stack.pop_back();
					postfix.push_back(tmp);
				}
				stack.pop_back();
				++it;
			}
			else if (std::isalnum(*it))
			{
				auto start = it;
				while (std::isalnum(*it))
					++it;
				std::string keyword(start, it);
				string_lower(keyword);
				postfix.push_back(keyword);
			}
			else // operators
			{
				int prior = priority(*it);
				while (!(stack.empty()) && (stack.back() != '(') &&
					   priority(stack.back()) <= prior && prior != 3)
				{
					std::string tmp(1, stack.back());
					postfix.push_back(tmp);
					stack.pop_back();
				}
				stack.push_back(*it);
				it++;
			}
		}
		while (!stack.empty())
		{
			std::string tmp(1, stack.back());
			postfix.push_back(tmp);
			stack.pop_back();
		}
		return postfix;
	}
	struct less_charcount
	{
		bool operator()(const Mail *x, const Mail *y)
		{
			if (x->charcount == y->charcount)
				return x->MessageID < y->MessageID;
			return x->charcount < y->charcount;
		}
	};
	int count;
	std::unordered_map<std::string, Mail> maildata_path;
	std::map<int, Mail *const> maildata_ID;
	std::set<Mail *, less_charcount> maildata_longest;
};

#endif
