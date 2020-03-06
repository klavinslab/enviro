
Developing Enviro
===

Building the Docker Image
---

To build the docker image, do
```bash
docker build -t enviro .
```

Building the React Client
---

To build the client, do
```bash
cd client
npm install  
export NODE_ENV=production
npx babel --watch src/enviro.js --out-file enviro.js 
```

To start the client, do
```bash
ws -p 80 &
```

Then on the host machine, go to [http://localhost](http://localhost).

Compiling the Server
---

To compile the server, do

```bash
cd ../server
make
```

To compile an example, do 

```bash
cd ../examples/wanderers
make
```

And to run an example, do

```bash
../server/bin/enviro
```

