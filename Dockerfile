FROM ubuntu:16.04

ARG RPC_USER
ARG RPC_PASSWORD

RUN apt-get update
RUN apt-get install -y wget pwgen

ENV RPC_USER ${RPC_USER:-phore}
ENV RANDOM_PASS `pwgen -Bs1 12`
RUN echo $RANDOM_PASS
ENV RPC_PASSWORD ${RPC_PASSWORD:-$RANDOM_PASS}
RUN wget https://github.com/phoreproject/Phore/releases/download/v1.2.0.0/phore-1.1.0-x86_64-linux-gnu.tar.gz -O phore-1.1.0.tar.gz
RUN tar -xvf phore-1.1.0.tar.gz
RUN mkdir -p /root/.phore/
RUN echo echo "rpcuser=$RPC_USER\nrpcpassword=$RPC_PASSWORD"
RUN echo "rpcuser=$RPC_USER\nrpcpassword=$RPC_PASSWORD" > /root/.phore/phore.conf

EXPOSE 8332 8333 18332 18333

WORKDIR /phore-1.1.0/bin
CMD ["/phore-1.1.0/bin/phored"]

