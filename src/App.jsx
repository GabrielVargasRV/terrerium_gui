import './App.css';
import ChartsContainer from './components/ChartsContainer/ChartsContainer';
import Controls from './components/Controls/Controls';
import ArduinoData from './components/ArduinoData'; // <-- Import the ArduinoData component

function App() {
  return (
    <div className="container">
      <ChartsContainer />
      <Controls />
      <ArduinoData /> {/* <-- Use the ArduinoData component */}
    </div>
  );
}

export default App;