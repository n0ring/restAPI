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
	Item		item;

	item = this->getItem(parrentId);
	if (item.isExist()) {
		sql.append("UPDATE items SET size = " \
				+ std::to_string(item.size + changeSizeFor) + " WHERE id = " + this->setStrSQL(parrentId) + "; ");
		auto res = this->executeQuery(SQL_SELECT_PARRENT + this->setStrSQL(parrentId), false);
		if (res.empty() == false) {
			sql.append(this->generateUpdateSizeQuery(res.begin().at(ID).as(std::string()), changeSizeFor) );
		}
	}
	
	return sql;
}

DbResponce* Db::deleteItem(std::string id, std::string updateDate) {
	Item				item;
	std::string			deleteQuery;
	std::string			updateQuery;
	std::string			updateSizeQuery;

	if (!isDateValid(updateDate)) {
		return new ErrorDbResponce(400, "Validation failed");
	}
	item = this->getItem(id);
	if (item.isExist() == false) {
		return new ErrorDbResponce(404, "Item not found");
	}
	try {
		deleteQuery = generateDeleteQuery(id);
		updateQuery = generateUpdateQuery(id, updateDate, id);
		updateSizeQuery = generateUpdateSizeQuery(item.parentId, item.size * -1);
		std::cout  << "update size: " << updateSizeQuery << std::endl;
		this->executeQuery(deleteQuery + updateQuery + updateSizeQuery, true);
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

Item Db::getItem(std::string id) {
	pqxx::result	res;
	Item			item;

	try {
		res = executeQuery(SQL_SELECT_BY_ID + this->setStrSQL(id), false);
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
		return item;
	}
	if (res.size() == 1) {
		auto it = res.begin();
		item.id = it.at(ID).as(std::string());
		item.url = it.at(URL).as(std::string());
		item.type =  it.at(TYPE).as(std::string());
		item.size = it.at(SIZE).as(0);
		item.parentId = it.at(PARRENT_ID).as(std::string());
	}
	return item;
}

bool Db::isItemExist(std::string id) {
	return this->getItem(id).id.empty() == false;
}

int Db::getItemSize(std::string id) {
	return this->getItem(id).size;
}

std::string Db::getItemType(std::string id) {
	return this->getItem(id).type;
}

DbResponce*		Db::import(std::unordered_map<std::string, Item>& models,
					std::unordered_map<std::string, int>& parrentFolders, std::string updateDate) {
	std::string sql;
	auto it = models.begin();
	if (isDateValid(updateDate) == false) {
		return new ErrorDbResponce(400, "Validation failed");
	}
	while (it != models.end()) {
		if ( it->second.isValid(*this, models) == false) {
			return new ErrorDbResponce(400, "Validation failed");
		}
		if (it->second.type == FOLDER_TYPE) {
			sql.append(it->second.generateQuery(updateDate, *this, parrentFolders, models));
		}
		it++;
	}
	try {
		executeQuery(sql, true);
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
		return new ErrorDbResponce(400, "Validation failed");
	}
	sql.clear();
	it = models.begin();
	while (it != models.end()) {
		if (it->second.type == FILE_TYPE) {
			sql.append(it->second.generateQuery(updateDate, *this, parrentFolders, models));
		}
		it++;
	}
	try {
		executeQuery(sql, true);
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
		return new ErrorDbResponce(400, "Validation failed");
	}

	return new EmptySuccessDbResponce();
}



