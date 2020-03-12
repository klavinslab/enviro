//
// export BABEL_ENV=production
// npx babel --watch src/enviro.js --out-file enviro.js 
// 

'use strict';

const e = React.createElement;

var CLIENT_ID;

function generate_id(){
  // Source: https://gist.github.com/gordonbrander/2230317
  function chr4(){
    return Math.random().toString(16).slice(-4);
  }
  return chr4() + chr4() +
    '-' + chr4() +
    '-' + chr4() +
    '-' + chr4() +
    '-' + chr4() + chr4() + chr4();
}

class Loading extends React.Component {
  render() {
    return <div className='message'>Loading ...</div>
  }
}

class Error extends React.Component {
  render() {
    return <div className='message'>
      Enviro Error: {this.props.error.message}. Is the server running?
      See <a href="https://github.com/klavinslab/enviro" target="enviro-github">here</a> for help.
    </div>
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
  let data_with_id = data;
  data_with_id.id = CLIENT_ID;
  fetch('http://127.0.0.1:8765/event', {
    method: "POST", 
    mode: 'no-cors',
    headers: { 'Content-Type': 'application/json'},
    body: JSON.stringify(data)
  }).then(res => { return JSON.parse(res.body) })
    .catch(error => { console.log("JSON parse error in post response: ", error) });  
}

class Agent extends React.Component {

  findSize(el) {
    if(el) {
      this.rect = el.getBoundingClientRect();
    }
  }  

  click(e) {
    post_event({
      type: "agent_click",
      id: this.props.agent.id,
      x: e.clientX - this.rect.left - this.rect.width/2,
      y: e.clientY - this.rect.top - this.rect.height/2
    });
    e.stopPropagation();
  }

  render() {
    let agent = this.props.agent;
    if ( agent.specification.definition.shape == "omni" ) {
      let rot = `rotate(${180*agent.position.theta/Math.PI})`;
      let unrot = `rotate(${-180*agent.position.theta/Math.PI})`;
      let tra = `translate(${agent.position.x} ${agent.position.y})`;
      return <g key={agent.id} transform={tra + rot} ref={el => this.findSize(el)}>
          <circle cx={0} cy={0} r={agent.specification.definition.radius}
                  className="agent" 
                  style={agent.specification.style} 
                  onClick={e=> this.click(e) }/>
          <line x1={0} y1={0} x2={agent.specification.definition.radius} y2={0} className="rotation-indicator"></line>
          {agent.sensors.map((value,i) => <Sensor value={value} agent={agent} i={i} key={i} />)}
          <g dangerouslySetInnerHTML={{__html: agent.decoration}} />
          <text x={agent.label.x} y={agent.label.y} transform={unrot}>{agent.label.text}</text>
      </g>
    } else { 
      let p = agent.specification.definition.shape.map(p => `${p.x},${p.y}`).join(" ");
      let rot = `rotate(${180*agent.position.theta/Math.PI})`;
      let unrot = `rotate(${-180*agent.position.theta/Math.PI})`;
      let tra = `translate(${agent.position.x} ${agent.position.y})`;
      return <g key={agent.id} transform={tra + rot} ref={el => this.findSize(el)}>
          <polygon points={p} className="agent" style={agent.specification.style} onClick={e=> this.click(e) }/>
          {agent.sensors.map((value,i) => <Sensor value={value} agent={agent} i={i} key={i} />)}
          <g dangerouslySetInnerHTML={{__html: agent.decoration}} />
          <text x={agent.label.x} y={agent.label.y} transform={unrot}>{agent.label.text}</text>
      </g>   
    }
  }

}

class Arena extends React.Component {

  findSize(el) {
    if(el) {
      this.rect = el.getBoundingClientRect();
    }
  }

  click(e) {

    post_event({
       type: "screen_click",
       x: e.clientX - this.rect.left - this.rect.width/2,
       y: e.clientY - this.rect.top - this.rect.height/2
    });

  }

  render() {
    let center = `translate(${this.props.w/2} ${this.props.h/2}) scale(1)`;
    return <svg width={this.props.w} 
                height={this.props.h} 
                onClick={e => this.click(e)}
                ref={el => this.findSize(el)}>
      <g transform={center}>
        {this.props.data.agents.map(agent => <Agent agent={agent} key={agent.id} />)}
      </g>
    </svg>    
  }
}

class Taskbar extends React.Component {

  click(e, value) {

    post_event({
       type: "button_click",
       value: value
    });

  } 

  render() {
    let buttons = this.props.buttons;
    if ( !buttons ) {
      buttons = [];
    }
    return <div id="title-container">
      <span id="title">ENVIRO: {this.props.name}</span>  
      <div className="buttons">
        {buttons.map(button => 
          <button key={button.label}
                  className='action-button'
                  style={button.style}
                  onClick={e => this.click(e, button.name)}>
            {button.label}
          </button>)}
      </div>
    </div>
  }
}

class Enviro extends React.Component {  

  constructor(props) {
    super(props);
    this.state = {
      error: null,
      mode: "uninitialized",
      items: []
    };
  }

  update() {

    if ( this.state.mode == "connecting" ) {
      this.get_configuration();
    } else if ( this.state.mode == "connected" ) {
      this.tick();
    }

  }

  get_configuration() {
    fetch("http://127.0.0.1:8765/config/"+CLIENT_ID)
      .then(res => res.json())
      .then(
        res => {
          console.log("got config", res.config);
          this.setState({ mode: "connected", config: res.config }, () => {
            setTimeout(() => { this.update() } , 25);
          });
        },
        error => {
          this.setState({ mode: "connecting", config: null, error: { message: "No connection" }}, () => {
            setTimeout(() => { this.update() } , 1000);
          });
        }
      )
  }

  tick() {
    fetch("http://127.0.0.1:8765/state/"+CLIENT_ID)
      .then(res => res.json())
      .then(
        (result) => {
          this.setState({
            mode: "connected",
            data: result,
            error: false
          }, () => {
            setTimeout(() => { this.update() } , 25);
          });
        },
        (error) => {
          console.log(error);
          this.setState({
            mode: "connecting",
            error
          }, () => {
            setTimeout(() => { this.update() } , 1000);
          });
        }
      )
  }

  componentDidMount() {

    CLIENT_ID = generate_id();

    this.setState({ 
      mode: "connecting"
    }, this.update);

    document.addEventListener("keydown", e => {
      post_event({
        type: "keydown",
        key: e.key,
        ctrlKey: e.ctrlKey,
        shiftKey: e.shiftKey,
        altKey: e.altKey,
        metaKey: e.metaKey,
        repeat: e.repeat  
      });
    });

    document.addEventListener("keyup", e => {
      post_event({
        type: "keyup",
        key: e.key,
        ctrlKey: e.ctrlKey,
        shiftKey: e.shiftKey,
        altKey: e.altKey,
        metaKey: e.metaKey,
        repeat: e.repeat  
      });
    });    

  }

  render() {
    const { mode, error, data, config } = this.state;
    let w = window.innerWidth;
    let h = window.innerHeight-41;    
    if (error) {
      return <Error error={error}/>;
    } else if (mode == "connecting" || !data) {
      return <Loading />;
    } else {
      return (
        <div>
          <Taskbar name={config.name} data={data} buttons={config.buttons} />
          <Arena w={w} h={h} data={data} />
        </div>      
      );
    }
  }
}

const domContainer = document.querySelector('#main_container');
ReactDOM.render(e(Enviro), domContainer);
