#include "ItemResponce.hpp"


ItemResponce::ItemResponce(void) {
	this->status = 200;
}

ItemResponce::ItemResponce(ItemResponce const & other) {
	*this = other;
}

ItemResponce ItemResponce::operator=(ItemResponce const & other) {
	if (this != &other) {
		this->j = other.j;
	}
	return *this;
}



ItemResponce::ItemResponce(pqxx::result & items, pqxx::result & childrens, Db& db) {
	this->status = 200;
	auto it = items.begin();
	// maybe change parrent logic

	this->j = nlohmann::json({
		{"id", it.at(ID).as(std::string())},
		{"url", it.at(URL).as(std::string())},
		{"type", it.at(TYPE).as(std::string())},
		{"date", it.at(DATE).as(std::string())},
		{"size", it.at(SIZE).as(0)}
	});
	auto res = db.getParrent(it.at(ID).as(std::string()));
	if (res.empty()) {
		j["parentID"] = NULL;
	} else {
		j["parentID"] = res.begin().at(ID).as(std::string());
	}
	if (j["type"] == "FOLDER") {
		j["children"] = nlohmann::json::array();
	} else {
		j["children"] = "null"; // TODO
	}
	for (auto row = childrens.begin(); row != childrens.end(); row++) {
		DbResponce* item = db.get(std::to_string(row.at(ID).as(0)));
		j["children"].push_back(item->toJson());
		delete item;
	}
}

nlohmann::json ItemResponce::toJson(void) const {
	return this->j;
}

int ItemResponce::getStatus(void) const {return 200; };