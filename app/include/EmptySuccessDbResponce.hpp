#pragma once 

#include "DbResponce.hpp"

class EmptySuccessDbResponce : public DbResponce {

	public:
		EmptySuccessDbResponce(void) {
			this->status = 200;
		} 

		~EmptySuccessDbResponce(void) {}
		virtual nlohmann::json	toJson(void) const {
			return nlohmann::json();
		};
		virtual int				getStatus(void) const {
			return this->status;
		};
};