RestAPI app on C++ std=c++11 with Postgres and adminer. 

Server provided by httplib (header only library).

Connet to Postgres with libpqxx library (install during deploy).

Work with json provided by nlohmann/json (header only library).

Deploy with docker (docker compose). Database and app in separate containers. 

For start containers: 

git clone git@github.com:n0ring/restAPI.git

cd restAPI/docker 

In file srcs/.env need to set database path DB_PATH=(PWD + /srcs/data-db). For example :

DB_PATH=/Users/username/restAPI/docker/srcs/data-db

make 



