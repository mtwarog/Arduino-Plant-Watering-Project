var pumpServer = "http://192.168.43.40:8080/";
var sensorToReadTemp = "temp";
var sensorToReadAirhum = "air_hum";
var sensorToReadSoilhum = "soil_hum";
var sensorToReadLight = "lum";

var request = $.ajax({
  url: pumpServer,
  type: "POST",
  data: { "temp_read" : sensorToReadTemp,
          "airhum_read" : sensorToReadAirhum,
          "soilhum_read_" : sensorToReadSoilhum,
          "light_read" : sensorToReadLight },
  dataType: "html"
});

request.done(function(msg) {
  $("#temp_content").html( JSON.parse(msg).temp_read );
  $("#airhum_content").html( JSON.parse(msg).airhum_read );
  $("#soilhum_content").html( JSON.parse(msg).soilhum_read );
  $("#light_content").html( JSON.parse(msg).light_read );
});

request.fail(function(jqXHR, textStatus) {
  $("#temp_content").html( "Request failed: " + textStatus );
});

var main = function() {
  $('#run_pomp_button').click(function() {
    var timeToPump = 5;
    var request = $.ajax({
      url: pumpServer + "runPump",
      type: "POST",
      data: {"time" : timeToPump},
      dataType: "html"
    });

    request.done(function(msg) {
      $("#pump_content").html( "Pump runned." );
    });

    request.fail(function(jqXHR, textStatus) {
      $("#pump_content").html( "Request failed: " + textStatus );
    });
    //pomp run for established time
  })
}

$(document).ready(main);

//<!-- Charts_begin -->
var chart_temp = AmCharts.makeChart( "chart_temp_div", {
  "type": "serial",
  "titles": [{
    "text": "Air temperature"
  }],
  "dataProvider": chartDataTemp,
  // "dataLoader": {
  //  "url": "https://s3-us-west-2.amazonaws.com/s.cdpn.io/218423/data1.json"
  //},
  "categoryField": "day",
  "graphs": [ {
    "valueField": "temp",
    "type": "line",
    "fillAlphas": 0, // this line is redundant since the default is 0 (no fill) anyway
    "bullet": "round",
    "lineColor": "#ff1a1a" //red
  } ]
} );

var chart_airhum = AmCharts.makeChart( "chart_airhum_div", {
  "type": "serial",
  "titles": [{
    "text": "Air humidity"
  }],
  "dataProvider": chartDataAirhum,
  "categoryField": "day",
  "graphs": [ {
    "valueField": "airhum",
    "type": "line",
    "fillAlphas": 0, // this line is redundant since the default is 0 (no fill) anyway
    "bullet": "round",
    "lineColor": "#00BFFF" //blue
  } ]
} );

var chart_soilhum = AmCharts.makeChart( "chart_soilhum_div", {
  "type": "serial",
  "titles": [{
    "text": "Soil humidity"
  }],
  "dataProvider": chartDataSoilhum,
  "categoryField": "day",
  "graphs": [ {
    "valueField": "soilhum",
    "type": "line",
    "fillAlphas": 0, // this line is redundant since the default is 0 (no fill) anyway
    "bullet": "round",
    "lineColor": "#8B4513" //brown
  } ]
} );

var chart_light = AmCharts.makeChart( "chart_light_div", {
  "type": "serial",
  "titles": [{
    "text": "Light intensity"
  }],
  "dataProvider": chartDataLight,
  "categoryField": "day",
  "graphs": [ {
    "valueField": "light",
    "type": "line",
    "fillAlphas": 0, // this line is redundant since the default is 0 (no fill) anyway
    "bullet": "round",
    "lineColor": "#FFFF00" //yellow
  } ]
} );

function setDataSet(dataset_url) {
  AmCharts.loadFile(dataset_url, {}, function(data) {
    chart.dataProvider = AmCharts.parseJSON(data);
    chart.validateData();
  });
}
//<!-- Charts_end -->
