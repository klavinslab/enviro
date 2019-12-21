'use strict';

const e = React.createElement;

class MyComponent extends React.Component {
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
      return <div className='message'>Error: {error.message}. Is the server running?</div>;
    } else if (!isLoaded) {
      return <div className='message'>Loading ...</div>;
    } else {
      const agent_list = data.agents.map(agent => {
        let p = agent.specification.definition.shape.map(x => x.join(",")).join(" ");
        let rot = `rotate(${180*agent.position.theta/Math.PI})`;
        let tra = `translate(${agent.position.x} ${agent.position.y})`;
        return <g key={agent.id} transform={tra + rot}>
            <polygon points={p} className="agent">
            </polygon>
        </g>
      });
      let center = `translate(${w/2} ${h/2}) scale(2)`
      return (
        <div>
          <div id="title-container">
            <span id="title">ENVIRO: </span>
            <span>{data.timestamp}</span>
          </div>
          <svg width={w} height={h}>
            <g transform={center}>
              {agent_list}
            </g>
          </svg>  
        </div>      
      );
    }
  }
}

const domContainer = document.querySelector('#main_container');
ReactDOM.render(e(MyComponent), domContainer);
