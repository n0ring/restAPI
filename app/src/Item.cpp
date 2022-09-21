#include "Item.hpp"


Item::Item(void) {
	size = 0;
	isUpdate = false;
}

Item::Item(Item const & other) {
	this->id = other.id;
	this->url = other.url;
	this->size = other.size;
	this->type = other.type;
	this->parentId = other.parentId;
	this->isUpdate = other.isUpdate;
}

std::string s(std::string in) {
	return in.empty() ? "null" : in;
}

int s(int in) {
	return in == -1 ? -1 : in;
}

bool Item::isIdValid(Db& db) {
	if (this->id.empty() || this->id == "null") {
		std::cerr << "id is null" << std::endl;
		return false;
	}
	if (db.isItemExist(this->id)) {
		if (db.getItemType(this->id) != this->type) {
			std::cerr << this->id << ": attemp to change type" << std::endl;
			return false;
		}
		this->isUpdate = true;
	}
	return true;
}

bool Item::isParentValid(Db& db, std::unordered_map<std::string, Item>& models) {
	if (this->parentId.empty() || this->parentId == "null") {
		return true;
	}

	if (models.count(this->parentId)) {
		if ( models[this->parentId].type != FOLDER_TYPE) {
			std::cerr << this->id << ": parrent is not folder" << std::endl;
			return false;
		}
	} else if (db.getItemType(this->parentId) != FOLDER_TYPE) {
		std::cerr << this->id << ": parrent (in db: " << this->parentId << " ) is not folder->>" << db.getItemType(this->parentId) << std::endl;
		return false;
	}
	
	return true;
}

bool Item::isUrlValid(void) {
	if (this->type == FOLDER_TYPE && (this->url.empty() == false && this->url != "null")) {
		std::cerr << this->id << ": not null url in folder" << std::endl;
		return false;
	}
	if (this->type == FILE_TYPE && this->url.length() > 255) {
		std::cerr << this->id << ": too big url " << std::endl;
		return false;
	}
	return true;
}

bool Item::isSizeValid(void) {
	if (this->type == FOLDER_TYPE && this->size > 0) {
		std::cerr << this->id << ": size of folder > 0" << std::endl;
		return false;
	}
	if (this->type == FILE_TYPE &&  this->size <= 0) {
		std::cerr << this->id << ": size of file <= 0" << std::endl;
		return false;
	}
	return true;
}

bool Item::isTypeValid(void) {
	if (this->type.empty()) {
		std::cerr << this->id << ": type is null" << std::endl;
		return false;
	}
	if (this->type != FOLDER_TYPE && this->type != FILE_TYPE) {
		std::cerr << this->id << ": type name is incorrect" << std::endl;
		return false;
	}
	return true;
}


bool Item::isValid(Db& db, std::unordered_map<std::string, Item>& models) {
	return	this->isIdValid(db) &&
			this->isParentValid(db, models) &&
			this->isUrlValid() &&
			this->isSizeValid() &&
			this->isTypeValid();
}

std::string setStrSQL(std::string param) {
	return "\'" + param + "\'";
}

std::string Item::getInsertSQL(std::string updateDate, Db& db) {
	std::string sqlChildresInsert;
	std::string sqlColumns("INSERT INTO items (id, type, date");
	std::string updateDateSQL;
	std::string updateSizeSQL;

	std::string sqlValues("VALUES (" + setStrSQL(this->id) + ", " 
			+ setStrSQL(this->type) + ", " + setStrSQL(updateDate));
	if (this->type == FILE_TYPE) {
		sqlColumns.append(", size");
		sqlValues.append(", " + std::to_string(this->size));
	}
	if (this->url.empty() == false) {
		sqlColumns.append(", url");
		sqlValues.append(", " + setStrSQL(this->url));
	}
	if (this->parentId.empty() == false) {
		sqlColumns.append(", parentid");
		sqlValues.append(", " + setStrSQL(this->parentId));
		sqlChildresInsert.append("INSERT INTO childrens (parrent_id, child_id) VALUES (");
		sqlChildresInsert.append(setStrSQL(this->parentId) + ", " + setStrSQL(this->id) + ");\n");
		try {
			updateDateSQL = db.generateUpdateQuery(this->parentId, updateDate, this->id);
			updateSizeSQL = db.generateUpdateSizeQuery(this->parentId, this->size);
		}
		catch(const std::exception& e) {
			std::cerr << e.what() << '\n';
		}
	}
	sqlColumns.append(") \n");
	sqlValues.append("); \n");
	return sqlColumns + sqlValues + sqlChildresInsert + updateDateSQL + updateSizeSQL;
}

std::string Item::getUpdateSQL(std::string updateDate, Db& db,
				std::unordered_map<std::string, int>& parrentFolders) {
	// delete from children connections
	std::string sqlUpdateChilds;
	std::string sqlItems("UPDATE items SET ");
	std::string updateDateSQL;
	Item		item = db.getItem(this->id);
	int			sizeChange = 0;
	std::string updateSizeSql;

	if (item.type == FILE_TYPE) {
		sizeChange = this->size - item.size;
		std::cout << item.id << ": " << item.size << "-" << this->size << std::endl;
	}
	sqlItems.append("parentid=" + (this->parentId.empty() ? "null" : setStrSQL(this->parentId)) );
	sqlItems.append(", url=" + (this->url.empty() ? "null" : setStrSQL(this->url)) );
	sqlItems.append(", size="  + (this->type == FOLDER_TYPE ? std::to_string(item.size) : std::to_string(this->size)) );
	sqlItems.append(", date="  + setStrSQL(updateDate));
	sqlItems.append(" WHERE id=" + setStrSQL(this->id) + ";\n");
	auto parent = db.getParrent(this->id);
	// parrentFolders[this->parentId] -= db.getItemSize(this->id);
	if (parent.empty() && this->parentId.empty() == false) {
		sqlUpdateChilds.append("INSERT INTO childrens (parrent_id, child_id) VALUES (");
		sqlUpdateChilds.append(setStrSQL(this->parentId) + ", " + setStrSQL(this->id) + ");\n");
	} else if (parent.empty() == false) {
		if (this->parentId.empty() == false) {
			sqlUpdateChilds.append("UPDATE childrens SET parrent_id=" + setStrSQL(this->parentId));
			sqlUpdateChilds.append("WHERE child_id=" + setStrSQL(this->id) + ";\n");
		} else {
			sqlUpdateChilds.append("DELETE FROM childrens WHERE child_id=" + setStrSQL(this->id) + ";\n");
		}
	}
	try {
		updateDateSQL = db.generateUpdateQuery(this->parentId, updateDate, this->id);
		updateSizeSql = db.generateUpdateSizeQuery(this->parentId, sizeChange);
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
	
	return sqlItems + sqlUpdateChilds + updateDateSQL + updateSizeSql;
}


std::string Item::generateQuery(std::string updateDate, Db& db,
	std::unordered_map<std::string, int>& parrentFolders,
	std::unordered_map<std::string, Item>& models) {
	std::string sql;
	Item* currentItem	= NULL;
	
	if (this->isUpdate) {
		sql = this->getUpdateSQL(updateDate, db, parrentFolders);
	} else {
		sql = this->getInsertSQL(updateDate, db);
	}
	return sql;
}
