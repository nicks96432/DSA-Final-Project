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
static std::istream &myGetline(std::istream &in, std::string &str)
{
	str.clear();
	char c;
	while ((c = in.get()) <= 32 && !in.fail());
	if (c != 4)
		str.push_back(c);
	while ((c = in.get()) != '\n' && !in.fail())
		str.push_back(c);
	return in;
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
inline bool isAlnum(const char &c)
{
	if ((c >= 'a' && c <= 'z') ||
		(c >= 'A' && c <= 'Z') ||
		(c >= '0' && c <= '9'))
		return true;
	return false;
}
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
		file.sync_with_stdio(false);
		file.tie(NULL);
		if (file.fail())
			throw std::runtime_error(p + ": No such file or directory.");
		path = p;
		isopen = true;
		std::string tmp(50, '\0');
		char _;
		// From from
		std::getline(file, tmp);
		from = std::string(tmp.begin() + 6, tmp.end());
		// Date day
		std::getline(file, tmp);
		char tmp2[12];
		sscanf(tmp.c_str(), "Date: %d %s %d at %d:%d", &datetime.day,
			   tmp2, &datetime.year, &datetime.hour, &datetime.minute);
		auto it = month_to_number.find(tmp2);
		if (it == month_to_number.cend())
			throw std::out_of_range("invalid month: " + std::string(tmp2));
		datetime.month = it->second;
		// Message-ID
		std::getline(file, tmp);
		sscanf(tmp.c_str(), "Message-ID: %d", &MessageID);
		// Subject
		std::getline(file, tmp);
		for (auto iter = tmp.begin() + 8; iter <= tmp.end(); ++iter)
		{
			while (iter <= tmp.end() && !isAlnum(*iter))
				++iter;
			auto start = iter;
			while (iter <= tmp.end() && isAlnum(*iter))
				++iter;
			subject.insert(std::string(start, iter));
		}
		// To
		std::getline(file, tmp);
		to = std::string(tmp.begin() + 4, tmp.end());
		// content:
		std::getline(file, tmp);
		// real content
		charcount = 0;
		while (std::getline(file, tmp))
		{
			for (auto iter = tmp.begin(); iter <= tmp.end(); ++iter)
			{
				while (iter != tmp.end() && !isAlnum(*iter))
					++iter;
				auto start = iter;
				while (iter != tmp.end() && isAlnum(*iter))
					++iter;
				std::string ins(start, iter);
				charcount += ins.length();
				content.insert(ins);
			}
		}
		file.close();
		// std::cout << *this;
		// exit(-1);
	}
	inline int ID() const
	{
		return MessageID;
	}
	void print(std::ostream &out = std::cout) const
	{
		out << "From: " << from << '\n';
		out << "Date: " << datetime.day << ' '
			<< number_to_month[datetime.month] << ' '
			<< datetime.year << " at "
			<< datetime.hour << ':'
			<< datetime.minute << '\n';
		out << "Message-ID: " << MessageID << '\n';
		out << "Subject: ";
		for (const auto &str : subject)
			out << str << ' ';
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
	inline bool isOpen() const
	{
		return isopen;
	}
	inline void openFile()
	{
		isopen = true;
	}
	inline void closeFile()
	{
		isopen = false;
	}
	inline int ContentLength() const
	{
		return charcount;
	}

private:
	struct Datetime
	{
		int year, month, day, hour, minute;
	};
	Datetime datetime;
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
	Mails()
	{
		count = 0;
	}
	void add(const std::string &path)
	{
		auto it = maildata_path.find(path);
		if (it != maildata_path.end())
		{
			if (it->second.isOpen())
			{
				std::cout << "-\n";
				return;
			}
			else
			{
				it->second.openFile();
				maildata_heap.push_back(it);
				std::push_heap(maildata_heap.begin(), maildata_heap.begin() + ++count,
							   iterator_compare);
				std::cout << count << '\n';
				return;
			}
		}
		else
		{
			Mail mail(path);
			maildata_path[path] = mail;
			auto it = maildata_path.find(path);
			maildata_ID[mail.ID()] = it;
			maildata_heap.push_back(it);
			std::push_heap(maildata_heap.begin(), maildata_heap.begin() + ++count,
						   iterator_compare);
			// std::cout << count << '\n';
		}
	}
	void remove(const int &id)
	{
		auto it = maildata_ID.find(id);
		if (it == maildata_ID.end() || !it->second->second.isOpen())
		{
			std::cout << "-\n";
			return;
		}
		it->second->second.closeFile();
		std::pop_heap(maildata_heap.begin(), maildata_heap.begin() + count--,
					  iterator_compare);
		maildata_heap.pop_back();
		std::cout << count << '\n';
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
		std::stringstream ss;
		ss << commands;
		std::string tmp;
		while (ss >> tmp)
		{
			if (tmp.front() == '-')
				switch (tmp.at(2))
				{
				case 'f':
					break;
				case 't':
					break;
				case 'd':
					break;
				default:
					break;
				}
		}
		std::cout << commands << '\n';
		// std::cout << "-\n";
		return;
	}
	void longest(void) const
	{
		if (count == 0)
		{
			std::cout << "-\n";
			return;
		}
		std::cout << maildata_heap.front()->second.ID() << ' '
				  << maildata_heap.front()->second.ContentLength() << '\n';
	}

private:
	typedef std::unordered_map<std::string, Mail>::iterator Mail_iterator;
	int count;
	std::unordered_map<std::string, Mail> maildata_path;
	std::map<int, Mail_iterator> maildata_ID;
	std::vector<Mail_iterator> maildata_heap;
	std::function<bool(const Mail_iterator &, const Mail_iterator &)> iterator_compare =
		[](const Mail_iterator &x, const Mail_iterator &y) -> bool {
		if (x->second.ContentLength() == y->second.ContentLength())
			return x->second.ID() < y->second.ID();
		return x->second.ContentLength() < y->second.ContentLength();
	};
};

#endif