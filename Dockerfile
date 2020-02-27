#
# docker run -p80:80 -p8765:8765 -v $PWD:/development -it enviro bash
#

FROM klavins/elma:latest

RUN apt-get update && \
    apt-get install -y git && \
    apt-get install -y libxmu-dev && \
    apt-get install -y freeglut3-dev && \
    apt-get install -y cmake && \
    apt-get install -y libxrandr-dev && \
    apt-get install -y cppcheck && \
    apt-get install -y graphviz && \
    apt-get install -y doxygen && \
    apt-get install -y nano && \
    apt-get install -y nodejs 

# Install chipmunk
ENV SRCDIR /usr/local/src/
ENV CPDIR $SRCDIR/Chipmunk2D
ENV LD_LIBRARY_PATH $CPDIR/build/src
WORKDIR $SRCDIR
RUN git clone https://github.com/slembcke/Chipmunk2D.git
RUN mkdir build
WORKDIR $CPDIR/build
RUN cmake ..
RUN make

# Install ws to serve a website
RUN curl https://www.npmjs.com/install.sh | sh
RUN npm install -g local-web-server

# Install uSockets
WORKDIR /tmp
RUN git clone https://github.com/uNetworking/uSockets.git
WORKDIR /tmp/uSockets
RUN make
RUN mv src /usr/local/include/uSockets
RUN mv uSockets.a /usr/local/lib/libuSockets.a
RUN rm -r /tmp/uSockets

# Install uWebSockets
WORKDIR /tmp
RUN git clone https://github.com/uNetworking/uWebSockets.git
RUN mv /tmp/uWebSockets/src /usr/local/include/uWebSockets
RUN rm -r /tmp/uWebSockets

# Install enviro
WORKDIR /tmp
WORKDIR /usr/local/src
RUN git clone https://github.com/klavinslab/enviro.git
WORKDIR /usr/local/src/enviro/server
RUN make -j 4
WORKDIR /usr/local/src/enviro/client
RUN npm install 
RUN (export NODE_ENV=production; npx babel src/enviro.js --out-file enviro.js)
RUN chmod 755 /usr/local/src/enviro/scripts/start.sh
RUN cp /usr/local/src/enviro/config/bashrc /root/.bashrc

# expose ports
EXPOSE 80
EXPOSE 8765

# start web server
CMD ["sh", "-c", "touch", "/tmp/issues"]

WORKDIR /source