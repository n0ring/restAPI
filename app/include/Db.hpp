#pragma once

#include <iostream>
#include <pqxx/pqxx>
#include <vector>
#include <stdio.h>
#include "ItemResponce.hpp"
#include "ErrorDbResponce.hpp"
#include "EmptySuccessDbResponce.hpp"
#include "Item.hpp"

#define DB_SETUP "dbname = restapi_db user = n0ring password = password host = postgres port = 5432"
#define SQL_SELECT_BY_ID "SELECT * FROM items where id="
#define SQL_SELECT_CHILDS "SELECT childrens.child_id FROM childrens where parrent_id="
#define SQL_SELECT_PARRENT "SELECT childrens.parrent_id FROM childrens where child_id="
#define SQL_DELETE_FROM_CHILDS_BY_PARRENT "DELETE FROM childrens WHERE parrent_id="
#define SQL_DELETE_FROM_CHILDS_BY_CHILD "DELETE FROM childrens WHERE child_id="
#define SQL_DELETE_FROM_ITEMS_BY_ID "DELETE FROM items WHERE id="

#define CHILD_ID_FIELD 1

class ItemResponce;
struct Item;

class Db {
	private: 
		pqxx::connection db_connection;

		std::string generateDeleteQuery(std::string id);

	public:
		Db(void);
		~Db(void);
		DbResponce*		get(std::string);
		DbResponce*		deleteItem(std::string, std::string);
		DbResponce*		import(std::unordered_map<std::string, Item>& models,
							std::unordered_map<std::string, int>& parrentFolders,
							std::string updateDate);

		pqxx::result	executeQuery(std::string query, bool isTransaction);
		Item			getItem(std::string id);
		std::string		setStrSQL(std::string param);
		pqxx::result	getParrent(std::string id);
		bool			isItemExist(std::string id);
		std::string		getItemType(std::string id);
		std::string 	generateUpdateQuery(std::string id,  std::string date, std::string deleteId);
		int				getItemSize(std::string id);
		std::string		generateUpdateSizeQuery(std::string parrentId, int changeSizeFor);
};