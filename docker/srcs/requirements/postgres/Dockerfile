FROM postgres:latest

EXPOSE 5432

COPY init.sql /docker-entrypoint-initdb.d/
# COPY pg_hba.conf .
RUN  echo "host all all all $POSTGRES_HOST_AUTH_METHOD" >> pg_hba.conf
# RUN cat pg_hba.conf > /var/lib/postgresql/data/pg_hba.conf