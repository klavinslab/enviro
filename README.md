ENVIRO: The multi-agent, multi-user, multi-everything simulator
===

Dockerhub Images
===

To start the docker image environment, do
```bash
docker run -p80:80 -p8765:8765 -v $PWD:/source -it klavins/enviro:v1.0 bash
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

Note that `esm generate` only makes directional robots. For an omni-directional robot, you need to subsequently edit the `defs/my_robot.json` file and change "shape" to "omni" and add a "radius" field (see below). &#x1F537; New in 1.1.

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
> ***circular:*** The string "omni", which makes a circular omni-directional agent. If you choose this option, you also need to specify a "radius". &#x1F537; New in 1.1. 

> `radius`<br>
The radius of a circular, omnidirectional robot. Only used when the `shape` field is "omni". &#x1F537; New in 1.1. 

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
---

Processes and state machines that are added to agents are both elma processes and agent interfaces. To learn how to use elma processes, go [here](http://klavinslab.org/elma). 

The agent interface class methods are described below. They are available in the `init()`, `start()`, `update()`, and `stop()` methods of your process, as well as the `entry()`, `during()` and `exit()` methods of any states in your state machines.

> `cpVect position()` <br>
This method returns the position of the agent. The `cpVect` structure has fields `x` and `y` that can be treated as `doubles`. 

> `cpVect velocity()`<br>
This method returns the velocity of the agent. The `cpVect` structure has fields `x` and `y` that can be treated as `doubles`. 

> `x()`, `y()`, `vx()`, `vy()`<br>
The horizontal and verical positions, and the horizonal and vertical velocities -- separated out so you do not need to about the structure. &#x1F537; New in 1.1. 

> `double angle()`<br>
This method returns the angle of the agent in radians and can be treated as a `double`.

> `double angular_velocity()`<br>
This method returns the angular velocity of the agent in radians per second and can be treated as a `double`.

> `int id()`<br>
This method returns a unique id of the agent.

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

> `double sensor_value(int index)`<br>
This method returns the value of the specificed index. It is the distance from the location of the sensor to the nearest object in the direction the sensor is pointing. The index refers to the position in the sensor list in the agent's JSON definition. 

> `std::vector<double> sensor_values()`<br>
This method returns a list of all the sensor values, in the same order as the sensors appear in the agent's JSON definition.

For Omni Directional Robots
---

> `void omni_apply_force(double fx, double fy)`<br>
This methied applies a force specified by the arguments. The agent's mass comes in to play here using Newton's laws of motion. &#x1F537; New in 1.1.

> `void omni_track_velocity(double vx, double vy, double k=10)` <br>
This method makes the agent attempty to track the given translational velocity. If other elements are in the way, or if it is experience collisions, it may not be able to exactly track these values. The optional argument is the proportional gain on the feedback controller that implements the tracking controller. &#x1F537; New in 1.1.

> `void omni_damp_movement()`<br>
This method slows the agent down using the linear and angular friction coefficients defined in the agent's JSON definition file. &#x1F537; New in 1.1.

> `void omni_move_toward(double x, double y, double v=1)`<br>
This method attepts to move the agent to the given (x,y) location. If something in the way, the agent will not get there. The robot simultaneously attempts to rotate so that it is pointing toward the target and also moves forward, going faster as its angular error is reduced. The optional argument is the desired velocity of rotation and forward motion. &#x1F537; New in 1.1.

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
> &#x1F536; New in 1.0.

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

> Name: `screen_click`
> Value: An object with `x` and `y` fields holding the location of the click in world coordinates.

> Name: `agent_click`
> Value: An object with `id`, `x` and `y` fields holding the agent's id and the location of the click in **agent** coordinates. 

> Name: `button_click`
> Value: An object with the a `name` field that matches the name field used in `config.json`. 
> &#x1F536; New in 1.0.

> Name: `keydown`
> Value: An object with a `key` field, which is the character pressed, as well as the following boolean fields
> ```
> ctrlKey
> shiftKey
> altKey
> metaKey
> ```
> &#x1F536; New in 1.0.

> Name: `keyup`
> Value: Same as for `keydown`. 
> &#x1F536; New in 1.0.

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

