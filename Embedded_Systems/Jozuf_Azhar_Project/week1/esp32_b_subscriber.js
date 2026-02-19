const mqtt = require("mqtt");
const client = mqtt.connect("mqtt://localhost:1883");


client.on("connect", ()=> {
    console.log("Connected to MQTT broker");
    client.subscribe('sensor/esp32/a', (err)=> {
        if(!err) {console.log("subscribed to sensor/esp32/a")}
        else {console.error("subscription error")}
    });
});

client.on("message", (topic,message)=> {
    console.log(`${topic}: ${message}`);
})

client.on("error",(error)=>console.log(error));