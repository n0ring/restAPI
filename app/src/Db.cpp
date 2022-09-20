#include "Db.hpp"

Db::Db(void) : db_connection(DB_SETUP) {
	if (!db_connection.is_open()) {
		std::cerr << "Can't open database" << std::endl;
		throw pqxx::broken_connection();
	}
}

Db::~Db(void) {
	try {
		if (this->db_connection.is_open()) {
			this->db_connection.disconnect();
		}
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
}

std::string Db::setStrSQL(std::string param) {
	return "\'" + param + "\'";
}

pqxx::result Db::executeQuery(std::string query, bool isTransaction)  {
	pqxx::result res;
	
	if (isTransaction) {
		pqxx::work trObj(this->db_connection);
		res = trObj.exec(query);
		trObj.commit();
	} else {
		pqxx::nontransaction nonTrObj(this->db_connection);
		res = nonTrObj.exec(query);
	}
	return res;
}

// void getParrent(std::string id) {}

DbResponce* Db::get(std::string id) {
	pqxx::result		itemsResult, childrenResult; 

	try {
		itemsResult = this->executeQuery(SQL_SELECT_BY_ID + this->setStrSQL(id), false);
		childrenResult = this->executeQuery(SQL_SELECT_CHILDS + this->setStrSQL(id), false);
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
		return new ErrorDbResponce(400, "Validation failed");
	}
	if (itemsResult.empty()) {
		return new ErrorDbResponce(404, "Item not found");
	}
	return new ItemResponce(itemsResult, childrenResult, *this);
}

bool isDateValid(std::string updateDate) {
	int y,M,d,h,m;
	float s;

	int ret = sscanf(updateDate.c_str(), "%d-%d-%dT%d:%d:%fZ", &y, &M, &d, &h, &m, &s);
	return ret == 6;
}

// resultVector stays empty if success for UPDATE DELETE

std::string Db::generateDeleteQuery(std::string id) {
	std::vector<std::string>	childrens_id;
	std::string					query;

	auto res = this->executeQuery(SQL_SELECT_CHILDS + setStrSQL(id), false);
	for (auto row = res.begin(); row != res.end(); row++) {
		childrens_id.push_back(row.at(ID).as(std::string()));
	}
	query.append(
		SQL_DELETE_FROM_ITEMS_BY_ID + this->setStrSQL(id) + ";" +
		SQL_DELETE_FROM_CHILDS_BY_PARRENT + this->setStrSQL(id) + ";" +
		SQL_DELETE_FROM_CHILDS_BY_CHILD + this->setStrSQL(id) + ";"
	);
	for (auto id : childrens_id) {
		query.append(this->generateDeleteQuery(id));
	}
	return query;
}

std::string Db::generateUpdateQuery(std::string id,  std::string date,
								std::string deleteId) {
	std::string parrentId;
	std::string sql;
	
	if (id != deleteId) {
		sql.append("UPDATE items SET date = " \
			+ this->setStrSQL(date) + " WHERE id = " + this->setStrSQL(id) + "; ");
	}
	auto res = this->executeQuery(SQL_SELECT_PARRENT + this->setStrSQL(id), false);
	if (res.empty() == false) {
		parrentId = res.begin().at(ID).as(std::string());
		sql.append(this->generateUpdateQuery(parrentId, date, deleteId));
	}
	return sql;
}

std::string Db::generateUpdateSizeQuery(std::string parrentId, int changeSizeFor) {
	std::string sql;

	int size = this->getItemSize(parrentId);
	sql.append("UPDATE items SET size = " \
		+ std::to_string(size + changeSizeFor) + " WHERE id = " + this->setStrSQL(parrentId) + "; ");
	auto res = this->executeQuery(SQL_SELECT_PARRENT + this->setStrSQL(parrentId), false);
	if (res.empty() == false) {
		sql.append(this->generateUpdateSizeQuery(res.begin().at(ID).as(std::string()), size + changeSizeFor) );
	}
	return sql;
}

DbResponce* Db::deleteItem(std::string id, std::string updateDate) {
	DbResponce*			item;
	std::string			deleteQuery;
	std::string			updateQuery;

	if (!isDateValid(updateDate)) {
		return new ErrorDbResponce(400, "Validation failed");
	}
	try {
		if (this->isItemExist(id) == false) {
			return new ErrorDbResponce(404, "Item not found");
		}
		deleteQuery = generateDeleteQuery(id);
		updateQuery = generateUpdateQuery(id, updateDate, id);
		this->executeQuery(deleteQuery, true);
		this->executeQuery(updateQuery, true);
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
		return new ErrorDbResponce(400, "Validation failed");
	}

	return new EmptySuccessDbResponce();
}

pqxx::result	Db::getParrent(std::string id) {
	pqxx::result res;
	try {
		res = this->executeQuery( SQL_SELECT_PARRENT + this->setStrSQL(id) , false);
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
	return res;
}

bool Db::isItemExist(std::string id) {
	try {
		return executeQuery(SQL_SELECT_BY_ID + this->setStrSQL(id), false).empty() == false;
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
		return false;
	}
}

int Db::getItemSize(std::string id) {
	int size = 0;
	try {
		auto res = this->executeQuery(SQL_SELECT_BY_ID + this->setStrSQL(id), false);
		size = res.begin().at(SIZE).as(0);
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
		return false;
	}
	return size;
}

std::string Db::getItemType(std::string id) {
	std::string		type;

	try {
		auto result = executeQuery(SQL_SELECT_BY_ID + Db::setStrSQL(id), false);
		if (result.empty() == false) {
			type = result.begin().at(TYPE).as(std::string());
		}
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
	return type;
}

DbResponce*		Db::import(std::unordered_map<std::string, Item>& models,
					std::unordered_map<std::string, int>& parrentFolders, std::string updateDate) {
	std::string sql;
	// valid part 
	auto it = models.begin();
	if (isDateValid(updateDate) == false) {
		return new ErrorDbResponce(400, "Validation failed");
	}
	while (it != models.end()) {
		if ( it->second.isValid(*this, models) == false) {
			return new ErrorDbResponce(400, "Validation failed");
		}
		sql.append(it->second.generateQuery(updateDate, *this, parrentFolders, models));
		it++;
	}
	// std::cout << sql << std::endl;

	try {
		auto result = executeQuery(sql, true);
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
	// generate sqls 
		// for insert and update
		// for update parrentFolders (from map) -> take current size, update to new
		// generateUpdateQuery() for every parrent folders

	// execute sqls

	return new EmptySuccessDbResponce();
}


