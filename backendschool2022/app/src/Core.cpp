#include "Core.hpp"

void fillItem(nlohmann::json& j, std::string fieldName, std::string& toFill) {
	if (j.count(fieldName)) {
		toFill = j[fieldName];
	}
}

void fillItem(nlohmann::json& j, std::string fieldName, int& toFill) {
	if (j.count(fieldName)) {
		toFill = j[fieldName];
	}
}

void fill(nlohmann::json& j, Item& item) {
	fillItem(j, "id", item.id);
	fillItem(j, "url", item.url);
	fillItem(j, "parentId", item.parentId);
	fillItem(j, "size", item.size);
	fillItem(j, "type", item.type);
}

void toModels(nlohmann::json& j, std::unordered_map<std::string, Item>& models,
			std::unordered_map<std::string, int>& parrentFolders) {
	
	
	auto it = j["items"].begin();
	while (it != j["items"].end()) {
		Item item;
		fill(it.value(), item);

		if (models.count(item.id)) {
			models.clear();
			return ;
		}
		if (item.parentId.empty() == false) {
			parrentFolders[item.parentId] += item.size;
		}
		models.insert(std::make_pair(item.id, item));
		it++;

	}

}

Db* openDB() {
	Db* dbObj;
	try {
		dbObj = new Db();
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
		delete dbObj;
		return NULL;
	}
	return dbObj;
}

void getById(const httplib::Request &req, httplib::Response &res) {
	Db* dbObj;
	DbResponce* result;

	dbObj = openDB();
	if (dbObj == NULL) {
		return ;
	}
	auto id = req.matches[1];

	result= dbObj->get(id);
	res.status = result->getStatus();
	res.set_content(result->toJson().dump(2),  CONTENT_TYPE);
	delete result;
	delete dbObj;
}

void deleteByIdHandler(const httplib::Request &req, httplib::Response &res) {
	DbResponce*	result;
	Db*			dbObj;

	std::string id = req.matches[1];
	dbObj = openDB();
	if (dbObj == NULL) {
		return ;
	}
	if (id.empty() || req.params.size() != 1 || req.params.begin()->first != "date") {
		result = new ErrorDbResponce(400, "Validation failed");
	} else {
		result = dbObj->deleteItem(id, req.params.begin()->second);
	}
	res.status = result->getStatus();
	if (result->toJson().empty() == false) { // TODO
		res.set_content(result->toJson().dump(2), CONTENT_TYPE);
	}
	delete dbObj;
	delete result;
}

void importItems(const httplib::Request &req, httplib::Response &res) {
	DbResponce*								result;
	Db*										dbObj;
	std::unordered_map<std::string, Item>	models;
	std::unordered_map<std::string, int>	parrentFolders;
	nlohmann::json							j;

	try {
		j = nlohmann::json::parse(req.body);
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
	dbObj = openDB();
	if (dbObj == NULL) {
		return ;
	}
	if (j.count("items") == 0 || j.count("updateDate") == 0) {
		result = new ErrorDbResponce(400, "Validation failed");
	} else {
		toModels(j, models, parrentFolders);
		if (models.empty()) {
			result = new ErrorDbResponce(400, "Validation failed");
		} else {
			result = dbObj->import(models, parrentFolders, j["updateDate"]);
		}
	}

	res.status = result->getStatus();
	if (result->toJson().empty() == false) { // TODO
		res.set_content(result->toJson().dump(2), CONTENT_TYPE);
	}
	delete result;

}

Core::Core(void) {
	// set callbacks
	svr.Get(R"(/nodes/(\d+))", getById);
	svr.Delete(R"(/delete/(.*)?.*)", deleteByIdHandler);
	svr.Post("/imports", importItems);


	
	// svr.Get("/stop", [&](const httplib::Request &req, httplib::Response &res) {
	// 	svr.stop();
	// });

	std::cout << "start listen...\n"; 
	this->svr.listen("0.0.0.0", 80);

}

Core::~Core(void) {
	// this->db_connetion.disconnect();
	svr.stop();
}