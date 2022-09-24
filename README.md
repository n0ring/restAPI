RestAPI app on C++ std=c++11 with Postgres and adminer. 

Server provided by httplib (header only library).

Connet to Postgres with libpqxx library (install during deploy).

Deploy with docker (docker compose). Database and app in separate containers. 

For start containers: 

git clone git@github.com:n0ring/backendschool2022.git

cd backendschool2022/docker 

In file srcs/.env need to set database path DB_PATH=(PWD + /srcs/data-db). For example :

DB_PATH=/Users/username/restAPI/docker/srcs/data-db

make 



