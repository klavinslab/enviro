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
    let agent = this.props.agent;
    let x1 = agent.specification.definition.sensors[i].location.x,
    y1 = agent.specification.definition.sensors[i].location.y,
    x2 = x1 + value * Math.cos(agent.specification.definition.sensors[i].direction),
    y2 = y1 + value * Math.sin(agent.specification.definition.sensors[i].direction);
    return <line x1={x1} y1={y1} x2={x2} y2={y2} className="sensor" key={i}></line>    
  }
}

class Agent extends React.Component {
  render() {
    let agent = this.props.agent;
    let p = agent.specification.definition.shape.map(p => `${p.x},${p.y}`).join(" ");
    let rot = `rotate(${180*agent.position.theta/Math.PI})`;
    let tra = `translate(${agent.position.x} ${agent.position.y})`;
    return <g key={agent.id} transform={tra + rot}>
        <polygon points={p} className="agent" style={agent.specification.style} />
        {agent.sensors.map((value,i) => <Sensor key={i} />)}
    </g>  
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
      let center = `translate(${w/2} ${h/2}) scale(2)`
      return (
        <div>
          <div id="title-container">
            <span id="title">ENVIRO: </span>
            <span>{data.timestamp}</span>
          </div>
          <svg width={w} height={h}>
            <g transform={center}>
              {data.agents.map(agent => <Agent agent={agent} key={agent.id} />)}
            </g>
          </svg>  
        </div>      
      );
    }
  }
}

const domContainer = document.querySelector('#main_container');
ReactDOM.render(e(Enviro), domContainer);
