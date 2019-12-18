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
    fetch("http://127.0.0.1:8765/state")
      .then(res => res.json())
      .then(
        (result) => {
          this.setState({
            isLoaded: true,
            data: result
          });
        },
        // Note: it's important to handle errors here
        // instead of a catch() block so that we don't swallow
        // exceptions from actual bugs in components.
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
    if (error) {
      return <div>Error: {error.message}</div>;
    } else if (!isLoaded) {
      return <div>Loading...</div>;
    } else {
      const agent_list = data.agents.map(agent => {
        let p = agent.specification.definition.shape.map(x => x.join(",")).join(" ");
        let rot = `rotate(${180*agent.position.theta/Math.PI})`;
        let tra = `translate(${agent.position.x} ${agent.position.y})`;
        return <g key={agent.id} transform={tra + rot}>
            <polygon points={p}
                     style={{fill:"gray", stroke:"black", strokeWidth:1}}>
            </polygon>
        </g>
      });
      return (
        <svg width="500" height="800">
          {agent_list}
        </svg>        
      );
    }
  }
}

const domContainer = document.querySelector('#like_button_container');
ReactDOM.render(e(MyComponent), domContainer);
