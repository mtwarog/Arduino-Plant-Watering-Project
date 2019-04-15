var shouldRunPump = false;
realTimeAirTemp = 0;
realTimeAirHum = 0;
realTimeSoilHum = 0;
realTimeLum = 0;
const http = require('http');
const port = 8081;

const requestHandler = (request, response) => {
  console.log(request.url);
  var sensors_read = request.url.slice(1).split('&');
  var sensor_name = [];
  var sensor_value = [];
  sensors_read.forEach(element => {
	  var elementArr = element.split('=');
	  sensor_name.push(elementArr[0]);
	  sensor_value.push(elementArr[1]);
  })
  console.log(JSON.stringify(sensor_name));
  console.log(JSON.stringify(sensor_value));
  realTimeAirTemp = sensor_value[0];
  realTimeAirHum = sensor_value[1];
  realTimeLum = sensor_value[2];
  realTimeSoilHum = sensor_value[3];
  if (shouldRunPump) {
		response.end('RUN_PUMP');
		shouldRunPump = false;
	}
	else
		response.end('');
}

const server = http.createServer(requestHandler)

server.listen(port, (err) => {
  if (err) {
    return console.log('something bad happened', err)
  }

  console.log(`server is listening on ${port}`);
})



//require the express nodejs module
var express = require('express'),
	//set an instance of exress
	app = express(),
	//require the body-parser nodejs module
	bodyParser = require('body-parser'),
	//require the path nodejs module
	path = require("path");

const csvFilePath='./data/readings.csv';
const csvLoad=require('csvtojson');
var csv = csvLoad({"delimiter":';'});

//support parsing of application/json type post data
app.use(bodyParser.json());

//support parsing of application/x-www-form-urlencoded post data
app.use(bodyParser.urlencoded({ extended: true }));

//tell express that www is the root of our public web folder
//app.use(express.static(path.join(__dirname, 'www')));

function readData(callback, responseThis) {
  var data = [];
  csvLoad({"delimiter":';'})
  .fromFile(csvFilePath)
  .on('json',(reading)=>{
      data.push(reading);
  })
  .on('done',(error)=>{
    callback.call(responseThis, (JSON.stringify({
        //temp_read: data[data.length-1].temp || null,
        //airhum_read: data[0].air_hum || null,
        //soilhum_read: data[0].soil_hum || null,
        //light_read: data[0].lum || null,
        temp_read: realTimeAirTemp,
        airhum_read: realTimeAirHum,
        soilhum_read: realTimeSoilHum,
        light_read: realTimeLum,
    })));
    console.log('Sent read: ' + realTimeAirTemp);
    console.log('Sent read: ' + realTimeAirHum);
    console.log('Sent read: ' + realTimeSoilHum);
    console.log('Sent read: ' + realTimeLum);
  })
}

function runPump(timeToPump) {
  console.log("Running pump");
	shouldRunPump = true;
  // Implement actual pumpimng
}

app.post('/', function(req, res){
    res.setHeader('Content-Type', 'application/json');
    res.setHeader('Access-Control-Allow-Origin', '*');
    res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");

    readData(res.send, res);
});

app.post('/runPump', function(req, res){
    res.setHeader('Content-Type', 'application/json');
    res.setHeader('Access-Control-Allow-Origin', '*');
    res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");

    runPump(req.body.time);
    res.send();
});

//wait for a connection
app.listen(8080, function () {
  console.log('Server is running. Point your web app to: http://localhost:8080');
});
