#include "DbResponce.hpp"


class ErrorDbResponce : public DbResponce {

	public: 
		ErrorDbResponce(int code, std::string msg) {
			this->j = nlohmann::json({
				{"code", code},
				{"message", msg}
			});
		}
		~ErrorDbResponce(void) {};

		virtual int getStatus(void) const {
			return this->j["code"];
		}

		virtual nlohmann::json toJson(void) const {
			return this->j;
		};

};