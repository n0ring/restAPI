#pragma once

#include <iostream>
#include <unordered_map>
#include "Db.hpp"

#define FOLDER_TYPE "FOLDER"
#define FILE_TYPE "FILE"


class Db;

struct Item {
		std::string id;
		std::string url;
		int size;
		std::string type;
		std::string parentId;
		bool		isUpdate;
public:
		Item(void);
		Item(Item const & other);
		~Item(void) {};

		bool isValid(Db& db, std::unordered_map<std::string, Item>& models);
		bool isIdValid(Db& db);
		bool isParentValid(Db& db, std::unordered_map<std::string, Item>& models);
		bool isUrlValid(void);
		bool isSizeValid(void);
		bool isTypeValid(void);

		std::string generateQuery(std::string, Db& db,
					std::unordered_map<std::string, int>& parrentFolders,
					std::unordered_map<std::string, Item>& models);
		std::string getInsertSQL(std::string, Db& db);
		std::string getUpdateSQL(std::string, Db& db, 
					std::unordered_map<std::string, int>& parrentFolders);
		std::string getDateUpdateSQL( Db& db, std::string updateDate);
	//  id 
			//	has id
			// exist in db (if true change isUpdate)
	//	


// to sql 
	void print(void);
};