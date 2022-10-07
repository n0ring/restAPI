#pragma once

#include <iostream>
#include "json.hpp"

class DbResponce {
	protected: 
		int				status;
		nlohmann::json	j;
	public:
		virtual ~DbResponce() {};
		virtual nlohmann::json	toJson(void) const {};
		virtual int				getStatus(void) const {};
};