import React, { useEffect, useState } from 'react';

const arduinoIp = 'http://<Arduino_IP_Address>'; // <-- Replace with the actual IP address of your Arduino

function ArduinoData() {
  const [data, setData] = useState(null); // State to hold the data from Arduino

  useEffect(() => {
    // Fetch data from Arduino server
    fetch(`${arduinoIp}/`)
      .then(response => response.json()) // Parse JSON response
      .then(data => setData(data)) // Set the data to state
      .catch(error => console.error('Error fetching data:', error)); // Handle errors
  }, []);

  if (!data) {
    return <div>Loading...</div>; // Show loading message while data is being fetched
  }

  return (
    <div>
      <h1>Arduino Data</h1>
      <p>Fan 1 Status: {data.fan1_status ? 'On' : 'Off'}</p>
      <p>Fan 2 Status: {data.fan2_status ? 'On' : 'Off'}</p>
      <p>Light 1 Status: {data.light1_status ? 'On' : 'Off'}</p>
      <p>Light 2 Status: {data.light2_status ? 'On' : 'Off'}</p>
      <p>Pump Status: {data.pump_status ? 'On' : 'Off'}</p>
      <p>Water Level: {data.water_level}</p>
      <p>Temperature & Humidity: {data.temperature_humidity}</p>
      <p>Humidity: {data.humidity}%</p> {/* <-- Added humidity data */}
      <p>Temperature: {data.temperatureF}°F</p> {/* <-- Added temperature data */}
    </div>
  );
}

export default ArduinoData;