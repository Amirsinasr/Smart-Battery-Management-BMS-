import React, { useState, useEffect } from 'react';
import mqtt from 'mqtt';
import { LineChart, Line, XAxis, YAxis, Tooltip, CartesianGrid, ResponsiveContainer } from 'recharts';

function App() {
  const [batteryData, setBatteryData] = useState({ voltage: 0, current: 0, soc: 100 });
  const [chartData, setChartData] = useState([]);
  const [connectionStatus, setConnectionStatus] = useState('Connecting to Broker...');

  useEffect(() => {
    const client = mqtt.connect('wss://broker.hivemq.com:8884/mqtt');

    client.on('connect', () => {
      setConnectionStatus('Connected (Live)');
      client.subscribe('smartbms/battery/Sina', (err) => {
        if (err) {
          console.error('Subscription error', err);
        }
      });
    });

    client.on('message', (topic, message) => {
      try {
        const parsedData = JSON.parse(message.toString());
        setBatteryData(parsedData);

        setChartData((prevData) => {
          const timestamp = new Date().toLocaleTimeString();
          const newData = [...prevData, { time: timestamp, soc: parsedData.soc, voltage: parsedData.voltage }];
          if (newData.length > 20) newData.shift();
          return newData;
        });
      } catch (e) {
        console.error('Failed to parse incoming payload', e);
      }
    });

    client.on('error', (err) => {
      console.error('MQTT Connection error: ', err);
      setConnectionStatus('Connection Failed');
    });

    return () => {
      client.end();
    };
  }, []);

  return (
    <div style={{ padding: '30px', fontFamily: 'apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif', backgroundColor: '#f0f2f5', minHeight: '100vh' }}>
      <header style={{ marginBottom: '20px' }}>
        <h1 style={{ margin: 0, color: '#1a1a1a' }}>Smart BMS IoT Dashboard</h1>
        <p style={{ color: connectionStatus.includes('Live') ? '#2e7d32' : '#d32f2f', fontWeight: 'bold' }}>
          Status: {connectionStatus}
        </p>
      </header>

      {/* Top Metric Cards */}
      <div style={{ display: 'flex', gap: '20px', marginBottom: '30px', flexWrap: 'wrap' }}>
        <div style={{ background: 'white', padding: '20px', borderRadius: '10px', boxShadow: '0 4px 6px rgba(0,0,0,0.05)', flex: '1 1 200px' }}>
          <h4 style={{ margin: '0 0 10px 0', color: '#666' }}>Live Voltage</h4>
          <h2 style={{ margin: 0, color: '#333' }}>{batteryData.voltage.toFixed(3)} <span style={{ fontSize: '16px' }}>V</span></h2>
        </div>
        <div style={{ background: 'white', padding: '20px', borderRadius: '10px', boxShadow: '0 4px 6px rgba(0,0,0,0.05)', flex: '1 1 200px' }}>
          <h4 style={{ margin: '0 0 10px 0', color: '#666' }}>Current Draw</h4>
          <h2 style={{ margin: 0, color: '#333' }}>{batteryData.current.toFixed(1)} <span style={{ fontSize: '16px' }}>mA</span></h2>
        </div>
        <div style={{ background: 'white', padding: '20px', borderRadius: '10px', boxShadow: '0 4px 6px rgba(0,0,0,0.05)', flex: '1 1 200px' }}>
          <h4 style={{ margin: '0 0 10px 0', color: '#666' }}>Battery State of Charge</h4>
          <h2 style={{ margin: 0, color: '#1976d2' }}>{batteryData.soc.toFixed(2)} <span style={{ fontSize: '16px' }}>%</span></h2>
        </div>
      </div>

      {/* Recharts Live Graph Container */}
      <div style={{ background: 'white', padding: '25px', borderRadius: '10px', boxShadow: '0 4px 6px rgba(0,0,0,0.05)' }}>
        <h3 style={{ margin: '0 0 20px 0', color: '#333' }}>Discharge Curve (Real-Time SoC Tracker)</h3>
        <div style={{ width: '100%', height: 350 }}>
          <ResponsiveContainer>
            <LineChart data={chartData}>
              <CartesianGrid strokeDasharray="3 3" stroke="#e0e0e0" />
              <XAxis dataKey="time" stroke="#666" />
              
              {/* MAGIC FIX 1: Auto-zoom the Y-Axis and format the decimals */}
              <YAxis 
                domain={['auto', 'auto']} 
                tickFormatter={(tick) => tick.toFixed(4)}
                stroke="#666" 
                width={80}
              />
              
              <Tooltip />
              
              {/* MAGIC FIX 2: Turn off the sliding animation so historical points stay still */}
              <Line 
                type="monotone" 
                dataKey="soc" 
                stroke="#1976d2" 
                strokeWidth={3} 
                dot={{ r: 3 }} 
                activeDot={{ r: 8 }}
                isAnimationActive={false} 
              />
            </LineChart>
          </ResponsiveContainer>
        </div>
      </div>
    </div>
  );
}

export default App;