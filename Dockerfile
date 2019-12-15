FROM elma

ENV SRCDIR /usr/local/src/
ENV CPDIR $SRCDIR/Chipmunk2D
ENV LD_LIBRARY_PATH $CPDIR/build/src

RUN apt-get update && \
    apt-get install -y git && \
    apt-get install -y libxmu-dev && \
    apt-get install -y freeglut3-dev && \
    apt-get install -y cmake && \
    apt-get install -y libxrandr-dev

WORKDIR $SRCDIR

RUN git clone https://github.com/slembcke/Chipmunk2D.git
RUN mkdir build
WORKDIR $CPDIR/build
RUN cmake ..
RUN make

RUN mkdir /development
WORKDIR /development