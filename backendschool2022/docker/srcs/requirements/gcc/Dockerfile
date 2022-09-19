FROM gcc:latest

RUN apt update && apt install -y cmake
# RUN wget http://pqxx.org/download/software/libpqxx/libpqxx-4.0.tar.gz
COPY libpqxx-4.0.tar.gz .
RUN tar xvfz libpqxx-4.0.tar.gz
WORKDIR  libpqxx-4.0
# RUN ./configure
# RUN make
# RUN make install



# COPY app /app

# RUN mkdir -p /var/app/build
# WORKDIR /app
# RUN git clone https://github.com/google/googletest.git
# RUN make

# RUN chmod +x restserver

# ENTRYPOINT ["./restserver"]
ENTRYPOINT ["tail", "-f", "/dev/null"]
