FROM ubuntu:16.04

RUN apt-get update
RUN apt-get install -y wget pwgen

ARG RPC_USER
ARG RPC_PASSWORD
ENV RPC_USER ${RPC_USER:-odin}
# Create a random password if the user doesn't set one
ENV RANDOM_PASS `pwgen -Bs1 12`
ENV RPC_PASSWORD ${RPC_PASSWORD:-$RANDOM_PASS}

# Build the project
RUN wget https://github.com/odinblockchain/Odin/releases/download/v1.4.2/odin-1.4.2-x86_64-linux-gnu.tar.gz -O odin-1.4.2.tar.gz
RUN tar -xvf odin-1.4.2.tar.gz
RUN mkdir -p /root/.odin/
RUN echo "rpcuser=$RPC_USER\nrpcpassword=$RPC_PASSWORD" > /root/.odin/odin.conf

EXPOSE 22100 22101

WORKDIR /odin-1.4.2/bin
CMD ["/odin-1.4.2/bin/odind"]
