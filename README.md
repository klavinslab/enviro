ENVIRO: The multi-agent, multi-user, multi-everything simulator
===

Dockerhub Images
===

To start the docker image environment, do
```bash
docker run -p80:80 -p8765:8765 -v $PWD:/source -it klavins/enviro:v1.1 bash
```
This will start a bash promt from which you can build new projects and run the enviro server.

Creating a Project
===

To create a new project, use `esm` as follows:

```bash
mkdir my_project
cd my_project
esm init
```

This will create the following directories and files
```
Makefile
config.json
defs/
    README.md
lib/
    README.md
src/
    Makefile
```

Make the project and start the enviro sever as follows.
```bash
make
enviro
```

Then navigate to `http://localhost` you should see a rectangular walled area. 

You can press `Ctrl-C` at the `bash` interface to stop the enviro server. 

Adding a Robot
===

To add a robot to your project, you have to create some new files and edit the `config.json` file. First, do
```bash
esm generate MyRobot
```

This will create the files
```
defs/
  my_robot.json         // Defines the shape, mass, and other parameters of the robot
src/ 
  my_robot.h            // Contains classes inheriting from elma::Process that define the behavior
  my_robot.cc           // Contains the implementation of the classes in my_robot.h
```

Note that `esm generate` only makes directional robots. For an omni-directional robot, you need to subsequently edit the `defs/my_robot.json` file and change "shape" to "omni" and add a "radius" field (see below). &#x246A; New in 1.1.

To compile the robot code, do
```
make
```

This will make the file `lib/my_robot.so`, which is a **shared object file** containing the compiled code for your robot.

Using the Robot
===

To place the robot into the simulation, change the `agents` entry in `config.json` to
```json
"agents": [    
    {
        "definition": "defs/my_robot.json",
        "style": { "fill": "lightgreen", "stroke": "black" },
        "position": {
            "x": 0, 
            "y": 0, 
            "theta": 0
        }
    }        
]
```
Now you should be able to run
```bash
enviro
```
and see a green square in the environment. That's your robot!

More Examples
===

See the [examples directory](https://github.com/klavinslab/enviro/tree/master/examples) for more examples of agent definitions and controllers. 

Defining Agents
===

Files
---
An agent, called `MyRobot` for this example, consists of the following
- `defs/my_robot.json`: This file contains information about the agent's shape and physical properties such as its mass and friction coefficients.
- `src/my_agent.h`: A header file containing at the very least a class declaration of a class called `MyRobot` inheriting from `enviro::Agent` and a call to the macro `DECLARE_INTERFACE(MyRobot)` which allows enviro to use the class.
- `src/my_agent.cc`: A source file with whatever code you want in it, usually the implementations of methods in the header file.
- `lib.my_agent.so`: The shared object library for the agent. This is made when you run `make` from the main directory of your project. 

You can have any number of agents. Note that they will not appear in the simulation unless you add them in project configuration file, `config.json` (described below). 

JSON Definition
---

The JSON files in the  `defs/` directory should contain an object with the following fields:

> `name`<br>
A string defining the name of the agent.

> `type`<br>
Either "dynamic" or "static". If "dynamic", then the agent will move, have mass, etc. If "static" then the agent will not move but will still be something other agents will collide with (i.e. like a wall).

> `description`<br>
A string describing the agent.

> `shape`<br>
> ***polygon shaped:*** A list of pairs of the form `{ "x": 10, "y": 12 }` defining the vertices of a polygon. The physics engine will use this to determine the moment of initial and collision shape of the robot, and the user interface will use it to render the agent. All points are relative to the robot's center.<br>
> ***circular:*** The string "omni", which makes a circular omni-directional agent. If you choose this option, you also need to specify a "radius". &#x246A; New in 1.1. 

> `radius`<br>
The radius of a circular, omnidirectional robot. Only used when the `shape` field is "omni". &#x246A; New in 1.1. 

> `friction`<br>
An object with three numerical fields, `collision`, `linear`, and `rotational` defining the fricition coefficients of the robot with other robots and with the environment. Note that the latter two coefficients are only used if you apply a control in your `update()` methods such as `damp_movement()` or `track_velocity()`. 

> `sensors`<br>
> A list of range sensor definitions of the form (for example):
> ```json
> {
>    "type": "range",
>    "location": { "x": 12, "y": 0 },
>    "direction": 0
>}
>```
> The location field is relative to the robot's center. The direction is an angle in radians. 

> `mass`<br>
A number defining the mass of the robot.

> `controller`<br>
A path to the shared object library for the agent, such as `lib/my_robot.so`. 


The Agent Class
---

To define an agent, called `MyRobot` for example, you need to have a header file with at least the follow in it:
```c++
#include "enviro.h"

using namespace enviro;

class MyRobot : public Agent {
    public:
    MyRobot(json spec, World& world) : Agent(spec, world) {}
};

DECLARE_INTERFACE(MyRobot)
```
This code declares the `MyRobot` class, inherits from `enviro::Agent`, declares the constructor, and calls macro `DECLARE_INTERFACE` defined by `enviro` that sets up the shared library interface. The constructor must have exactly the type signature shown above, and must call the `Agent` constructor when it is initialized. 

You can add any number `elma` processes to and agent using the `add_process()` method in the constructor. For example,
```c++
class MyRobot : public Agent {
    public:
    MyRobot(json spec, World& world) : Agent(spec, world) {
        add_process(p1);
        add_process(p2);
        add_process(sm);
    }
    private:
    MyProcess1 p1;
    MyProcess2 p2;
    MyStateMachine sm;
};
```
Here, `MyProcess1` and `MyProcess2` should be classes that you define that (multiple) inherit from `elma::Process` and `enviro::AgentInterface`. The class `MyStateMachine` should inherit from `elma::StateMachine` and `enviro::AgentInterface`.

The AgentInterface Class
===

Processes and state machines that are added to agents are both elma processes and agent interfaces. To learn how to use elma processes, go [here](http://klavinslab.org/elma). 

The agent interface class methods are described below. They are available in the `init()`, `start()`, `update()`, and `stop()` methods of your process, as well as the `entry()`, `during()` and `exit()` methods of any states in your state machines.

Agent State
---

> `cpVect position()` <br>
This method returns the position of the agent. The `cpVect` structure has fields `x` and `y` that can be treated as `doubles`. 

> `cpVect velocity()`<br>
This method returns the velocity of the agent. The `cpVect` structure has fields `x` and `y` that can be treated as `doubles`. 

> `x()`, `y()`, `vx()`, `vy()`<br>
The horizontal and verical positions, and the horizonal and vertical velocities -- separated out so you do not need to about the structure. &#x246A; New in 1.1. 

> `double angle()`<br>
This method returns the angle of the agent in radians and can be treated as a `double`.

> `double angular_velocity()`<br>
This method returns the angular velocity of the agent in radians per second and can be treated as a `double`.

> `int id()`<br>
This method returns a unique id of the agent.

Motion Control for Oriented Agents
---

> `void apply_force(double thrust, double torque)`<br>
This methied applies a force specified by the `thrust` argument in the direction the agent is currently facing, and applies a torque specified by the `torque` argument around the center of the agent. The agent's mass comes in to play here using Newton's laws of motion. 

> `void track_velocity(double linear_velocity, double angular_velocity, double kL=10, double kR=10)` <br>
This method makes the agent attempty to track the given linear and angular velocities. If other elements are in the way, or if it is experience collisions, it may not be able to exactly track these values. The optional arguments are the proportional gains on the feedback controller that implements the tracking controller.

> `void damp_movement()`<br>
This method slows the agent down using the linear and angular friction coefficients defined in the agent's JSON definition file.  

> `void move_toward(double x, double y, double vF=75, double vR=20)`<br>
This method attepts to move the agent to the given (x,y) location. If something in the way, the agent will not get there. The robot simultaneously attempts to rotate so that it is pointing toward the target and also moves forward, going faster as its angular error is reduced. The optional arguments are the gains on the rotational and forward motion. 

> `void teleport(double x, double y, double theta)`<br>
This method instantaneously moves the agent to the given position and orientation.

Motion Control For Omni Directional Agents
---

> `void omni_apply_force(double fx, double fy)`<br>
This methied applies a force specified by the arguments. The agent's mass comes in to play here using Newton's laws of motion. &#x246A; New in 1.1.

> `void omni_track_velocity(double vx, double vy, double k=10)` <br>
This method makes the agent attempty to track the given translational velocity. If other elements are in the way, or if it is experience collisions, it may not be able to exactly track these values. The optional argument is the proportional gain on the feedback controller that implements the tracking controller. &#x246A; New in 1.1.

> `void omni_damp_movement()`<br>
This method slows the agent down using the linear and angular friction coefficients defined in the agent's JSON definition file. &#x246A; New in 1.1.

> `void omni_move_toward(double x, double y, double v=1)`<br>
This method attepts to move the agent to the given (x,y) location. If something in the way, the agent will not get there. The robot simultaneously attempts to rotate so that it is pointing toward the target and also moves forward, going faster as its angular error is reduced. The optional argument is the desired velocity of rotation and forward motion. &#x246A; New in 1.1.

Sensors
---

> `double sensor_value(int index)`<br>
This method returns the value of the specificed index. It is the distance from the location of the sensor to the nearest object in the direction the sensor is pointing. The index refers to the position in the sensor list in the agent's JSON definition. 

> `std::vector<double> sensor_values()`<br>
This method returns a list of all the sensor values, in the same order as the sensors appear in the agent's JSON definition.

> `std::string sensor_reflection_type(int index)`<br>
This method returns the name of object type the sensor of the specificed index is seeing. The index refers to the position in the sensor list in the agent's JSON definition. &#x246C; New in 1.3.

> `std::vector<std::string> sensor_values()`<br>
This method returns a list of all the sensor reflection types, in the same order as the sensors appear in the agent's JSON definition. &#x246C; New in 1.3.

Collisions
---

> `void notice_collisions_with(const std::string agent_type, std::function<void(Event&)> handler)` <br>
> Runs the handler function upon collisions with agents of the given agent type. 
> The `agent_type` string is the name used in `defs/*.json` files. 
> These should usually be set up in the `init` function of a process, as follows:
> ```c++
> void init() {
>     notice_collisions_with("Robot", [&](Event &e) {
>         int other_robot_id = e.value()["id"];
>         Agent& other_robot = find_agent(other_robot_id); 
>         // etc.
>     });
> }
> ```
> The value associated with the event `e` is a json object with a single key, `id`, which is the id of the other agent. 
> &#x246B; New in 1.2.

> `void ignore_collisions_with(const std::string agent_type)` <br>
> Stop noticing collisions with agents of the given type. 
> &#x246B; New in 1.2.

Constraints
---

> `void attach_to(Agent &other_agent)` <br>
> Create a constraint that attaches the calling agent to the `other_agent`. 
> For example, after the call
> ```c++
> Agent& other_robot = find_agent(other_robot_id); 
> attach_to(other_robot);
> ```
> the two agents center's will be constrained to remain at the same distance from each other. 
> &#x246B; New in 1.2.

Agent Management
---

> `Agent& find_agent(int id)` <br>
> Given an agent's id, returns a reference to the agent. Note: ***do not*** assign the return value of this function to a normal 
> variable, because when it goes out of scope, the agent's destructor will be called. Instead, assign it to a reference, as in
> ```c++
> Agent& other_agent = find_agent(other_agent_id;
> ```
> &#x246B; New in 1.2.

> `bool agent_exists(int id)` <br>
> Returns true or false depending on whether an agent with the given id still exists (i.e. is being managed by enviro). 
> Agents may cease to exist if `remove_agent()` is called. 
> &#x246B; New in 1.2.

> `void remove_agent(int id)` <br>
> Removes the agent with the given id from the simulation. Also calls it's desctructor, so think of it as remove and delete. 
> &#x246B; New in 1.2.

> `Agent& add_agent(const std::string name, double x, double y, double theta, const json style)` <br>
> Add's a new agent to the simulation with the given name at the given position and orientation.
> Note that an agent with this type should have been mentioned in `config.json` so enviro knows about it. 
> The style argument determines the agent's style, just as in the configuration file. 
> Any valid svg styling will work. For example:
> ```c++
> Agent& v = add_agent("Block", 0, 0, 0, {{"fill": "bllue"},{"stroke": "black"}});
> ```
> &#x246B; New in 1.2.

Styling
---

> `void set_style(json style)` <br>
> Change the agent's style, just as in the configuration file. 
> Any valid svg styling will work.
> &#x246B; New in 1.2.

> `void decorate(const std::string svg)`<br>
> Add an aribtrary svg element to the agent's rendering method on the client side. 
> The svg element will be transformed and rotated to the agent's coordinate system. 
> For example, to place a red dot in the middle of the agent do:
> ```c++
> decorate("<circle x='-5' y='5' r='5' style='fill: red'></circle>");
> ```
> To clear the decoration later, simple call
> ```c++
> decorate("");
> ```
> &#x246C; New in 1.3.

> `void label(const string str, double x, double y )` <br>
> Add a textual label to the agent's icon. The x and y values are relative to the origin of the agent's coordinate system. 
> &#x246C; New in 1.3.

> `void clear_label()` <br>
> Clear the label associated with the agent. 
> &#x246C; New in 1.3.

Project Configuration
===

The file `config.json` should have the following fields.

> `name`<br>
A string stating the name of your project. This will be displayed on the brower interface.

> `ip`<br>
The IP address of the server. Use `0.0.0.0` for now. 

> `port`<br>
The port for the server. Use `8765` for now.

> `buttons`<br>
> A list of buttons to show in the top right of the front end user interface. These should have the form
> ```json
> {
>     "name": "name",
>     "label": "Displayed Name",
>     "style": { "background": "white", "border-color": "black" }
> }
> ```
> The name field is used to catch button click events (see below). The label field defines the string displayed on the button. 
> The style field is an `css` code you want to add to the stying of the button. 
> &#x2469; New in 1.0.

> `agents`<br>
> A list of agents to put in the simulation. For example,
> ```json
> {
>     "definition": "defs/my_robot.json",
>     "style": { "fill": "lightgreen", "stroke": "black" },
>     "position": {
>         "x": 100, 
>         "y": 0, 
>         "theta": 0
>     }
> }
> ```
> The definition field should point to a shared object library. The style field can be any `svg` styling code. The position and orientation are the **initial** position and orientation of the agent.

> `statics`<br>
> A list of static objects to place in the environment. For example,
> ```json
> {
>     "style": { "fill": "gray", "stroke": "none" },
>     "shape": [
>         { "x": -350, "y": -200 },
>         { "x": -350,  "y": 200 },
>         { "x": -340,  "y": 200 },
>         { "x": -340, "y": -200 }
>     ]
> }
> ```
> The style field is any `svg` styling code, and the shape is a list of vertices of a polygon in world coordinates. The above example makes a ong, skinny rectangle for example.

Responding to Front End Events
===

The brower client relays mouse click, button press, and keyboard events to the enviro server, which your process can watch for. The event types and associated information are:

> Name: `screen_click`<br>
> Value: An object with `x` and `y` fields holding the location of the click in world coordinates.

> Name: `agent_click`<br>
> Value: An object with `id`, `x` and `y` fields holding the agent's id and the location of the click in **agent** coordinates. 

> Name: `button_click`<br>
> Value: An object with the a `name` field that matches the name field used in `config.json`. 
> &#x2469; New in 1.0.

> Name: `keydown`<br>
> Value: An object with a `key` field, which is the character pressed, as well as the following boolean fields
> ```
> ctrlKey
> shiftKey
> altKey
> metaKey
> ```
> &#x2469; New in 1.0.

> Name: `keyup`<br>
> Value: Same as for `keydown`. 
> &#x2469; New in 1.0.

To respond to events in your code, you should put elma watchers into the `init()` method of some process. For example, to respond to an agent click, you might do
```c++
void init() {
    watch("agent_click", (Event& e) => {
        if ( e.value.id == id() ) {
            std::cout << "ouch!\n"; 
        }
    });
}
```

Debugging Tools
===

The user interface is written in Javascript using React. If you are using Chrome, you can install [this plugin](https://chrome.google.com/webstore/detail/react-developer-tools/fmkadmapgofadopljbjfkapdkoienihi?hl=en). Then you can open the Chrome developer tools, click the ***Components*** tab, and then click on `Enviro` which will bring up the state on the right panel. Expand `data` to see information about all the agents. 

