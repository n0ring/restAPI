
COMPOSE_FILE	= srcs/docker-compose.yml
COMPOSE_CMD_F	= docker compose -f
DOCKER_CMD_F	= docker
UP				= up -d

DB_PATH			= ./srcs/data-db

.PHONY: all build ps down

all: up  

build:	
	$(COMPOSE_CMD_F) $(COMPOSE_FILE) build

up:
	mkdir -p $(DB_PATH)
	cp -r ../app srcs/requirements/gcc/
	@$(COMPOSE_CMD_F) $(COMPOSE_FILE) $(UP) --build

stop:
	$(COMPOSE_CMD_F) $(COMPOSE_FILE) stop

ps: 
	$(COMPOSE_CMD_F) $(COMPOSE_FILE)  ps -a

down:
	$(COMPOSE_CMD_F) $(COMPOSE_FILE) down

gcc: 
	$(DOCKER_CMD_F) exec -it gcc bash

pg: 
	$(DOCKER_CMD_F) exec -it postgres bash

clean: down  
	$(DOCKER_CMD_F) rmi -f $$($(DOCKER_CMD_F) images -q)
	$(DOCKER_CMD_F) volume rm -f $$($(DOCKER_CMD_F) volume ls -q)
	rm -rf $(DB_PATH)

	
fclean: down  
	$(DOCKER_CMD_F) volume rm -f $$($(DOCKER_CMD_F) volume ls -q)
	$(DOCKER_CMD_F) system prune -a --force
	$(DOCKER_CMD_F) system prune -f --volumes
	rm -rf $(DB_PATH)

net:
	$(DOCKER_CMD_F) network inspect srcs_app

re: down up



# docker system prune - delete all res
