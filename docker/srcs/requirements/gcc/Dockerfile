FROM gcc:4.9

# RUN wget http://pqxx.org/download/software/libpqxx/libpqxx-4.0.tar.gz
COPY libpqxx-4.0.tar.gz .
RUN tar xvfz libpqxx-4.0.tar.gz
WORKDIR  libpqxx-4.0
RUN ./configure
RUN make
RUN make install


RUN mkdir /app
COPY app /app

RUN mkdir -p /var/app/build
WORKDIR /app
RUN make

RUN chmod +x restserver

# for deploy
ENTRYPOINT ["./restserver"]

# for develop 
# ENTRYPOINT ["tail", "-f", "/dev/null"]
