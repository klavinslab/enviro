//
// export BABEL_ENV="development"
// npx babel --watch src/enviro.js --out-file enviro.js 
// 

'use strict';

const e = React.createElement;

class Loading extends React.Component {
  render() {
    return <div className='message'>Loading ...</div>
  }
}

class Error extends React.Component {
  render() {
    return <div className='message'>Error: {this.props.error.message}. Is the server running?</div>
  }
}

class Sensor extends React.Component {
  render() {
    let agent = this.props.agent,
        i = this.props.i,
        location = agent.specification.definition.sensors[i].location,
        x1 = location.x,
        y1 = location.y,
        angle = agent.specification.definition.sensors[i].direction,
        x2 = x1 + this.props.value * Math.cos(angle),
        y2 = y1 + this.props.value * Math.sin(angle);
    return <line x1={x1} y1={y1} x2={x2} y2={y2} className="sensor" key={this.props.i}></line>    
  }
}

function post_event(data) {
  fetch('http://127.0.0.1:8765/event', {
    method: "POST", 
    mode: 'no-cors',
    headers: { 'Content-Type': 'application/json'},
    body: JSON.stringify(data)
  }).then(res => { return JSON.parse(res.body) })
    .catch(error => { console.log("Screen click error: ", error) });  
}

class Agent extends React.Component {

  click(e) {
    post_event({
      type: "agent_click",
      id: this.props.agent.id,
      x: e.clientX,
      y: e.clientY
    });
    e.stopPropagation();
  }

  render() {
    let agent = this.props.agent;
    let p = agent.specification.definition.shape.map(p => `${p.x},${p.y}`).join(" ");
    let rot = `rotate(${180*agent.position.theta/Math.PI})`;
    let tra = `translate(${agent.position.x} ${agent.position.y})`;
    return <g key={agent.id} transform={tra + rot}>
        <polygon points={p} className="agent" style={agent.specification.style} onClick={e=> this.click(e) }/>
        {agent.sensors.map((value,i) => <Sensor value={value} agent={agent} i={i} key={i} />)}
    </g>  
  }

}

class Arena extends React.Component {

  click(e) {

   post_event({
      type: "screen_click",
      x: e.clientX,
      y: e.clientY
    });

  }

  render() {
    let center = `translate(${this.props.w/2} ${this.props.h/2}) scale(2)`;
    return <svg width={this.props.w} height={this.props.h} onClick={e => this.click(e)}>
      <g transform={center}>
        {this.props.data.agents.map(agent => <Agent agent={agent} key={agent.id} />)}
      </g>
    </svg>    
  }
}

class Taskbar extends React.Component {
  render() {
    return <div id="title-container">
      <span id="title">ENVIRO: </span>
      <span>{this.props.data.timestamp}</span>
    </div>
  }
}

class Enviro extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      error: null,
      isLoaded: false, 
      items: []
    };
  }

  componentDidMount() {
    this.interval = setInterval(() => {
        this.tick();
    }, 25);
  }

  tick() {
    fetch("http://127.0.0.1:8765/state")
      .then(res => res.json())
      .then(
        (result) => {
          this.setState({
            isLoaded: true,
            data: result,
            error: false
          });
        },
        (error) => {
          this.setState({
            isLoaded: true,
            error
          });
        }
      )
  }

  render() {
    const { error, isLoaded, data } = this.state;
    let w = window.innerWidth;
    let h = window.innerHeight-41;    
    if (error) {
      return <Error error={error}/>;
    } else if (!isLoaded) {
      return <Loading />;
    } else {
      return (
        <div>
          <Taskbar data={data} />
          <Arena w={w} h={h} data={data} />
        </div>      
      );
    }
  }
}

const domContainer = document.querySelector('#main_container');
ReactDOM.render(e(Enviro), domContainer);
