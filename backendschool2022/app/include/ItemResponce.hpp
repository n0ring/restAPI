#pragma once 

#include <iostream>
#include <vector>
#include <pqxx/result.hxx>
#include <string>
#include "DbResponce.hpp"
#include "Db.hpp"

class Db;

#define ID 0
#define PARRENT_ID 1
#define URL 2
#define SIZE 3
#define TYPE 4
#define DATE 5
#define CHILDREN 6

class ItemResponce : public DbResponce  {

	public:
		ItemResponce(pqxx::result &, pqxx::result &, Db& db);
		ItemResponce(void);
		~ItemResponce(void) {};
		ItemResponce(ItemResponce const & other);
		ItemResponce operator=(ItemResponce const & other);

		virtual nlohmann::json toJson(void) const;
		virtual int  getStatus(void) const;
};