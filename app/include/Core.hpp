#pragma once 

#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include "httplib.h"
#include "Db.hpp"
#include "ItemResponce.hpp"
#include "Item.hpp"

#define CONTENT_TYPE "application/json"

class Core {
	private:
		httplib::Server		svr;


	public: 
		Core(void);
		~Core(void);
};