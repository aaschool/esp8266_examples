# Google App Script Backend Code

To be pasted into the App Script `Code.gs` file.

```
/*
 * ESP8266 Sensor to Cloud
 * 2025 francesco.anselmo@aaschool.ac.uk
 * ESP8266 + Analogue Sensor + Google App Script + Google Sheets
 *
 * Dependencies
 * HTTPSRedirect: https://github.com/electronicsguy/HTTPSRedirect
 *
 */

// Enter Spreadsheet ID here
var SS = SpreadsheetApp.openById('CHANGEME-SpreadsheetID');
var str = "";

function doPost(e) {

  var parsedData;
  var result = {};
  
  try { 
    parsedData = JSON.parse(e.postData.contents);
  } 
  catch(f){
    return ContentService.createTextOutput("Error in parsing request body: " + f.message);
  }
   
  if (parsedData !== undefined){
    var flag = parsedData.format;
    if (flag === undefined){
      flag = 0;
    }
    
    var sheet = SS.getSheetByName(parsedData.sheet_name); // sheet name to publish data to is specified in Arduino code
    var dataArr = parsedData.values.split(","); // creates an array of the values to publish 

    var timestamp = new Date();
    
    var value = dataArr [0]; // value from Arduino code
    
    // read and execute command from the "payload_base" string specified in Arduino code
    switch (parsedData.command) {
      
      case "insert_row":
         
         sheet.insertRows(2); // insert full row directly below header text
         
         sheet.getRange('A2').setValue(timestamp);
         sheet.getRange('B2').setValue(value);
         
         str = "Success"; // string to return back to Arduino serial console
         SpreadsheetApp.flush();
         break;
         
      case "append_row":
         
         var publish_array = new Array(); // create a new array
         
         publish_array [0] = timestamp;
         publish_array [1] = value;
         
         sheet.appendRow(publish_array); // publish data in publish_array after the last row of data in the sheet
         
         str = "Success"; // string to return back to Arduino serial console
         SpreadsheetApp.flush();
         break;     
 
    }
    
    return ContentService.createTextOutput(str);
  } 
  
  else {
    return ContentService.createTextOutput("Error! Request body empty or in incorrect format.");
  }
}
```

# Google App Script Frontend Code

To be pasted in a different App Script project than the Backend, in the `Code.gs` file:

```
function doGet() {
  return HtmlService.createHtmlOutputFromFile('frontend.html');
}

function getSensorData() {
  // Get the active spreadsheet and sheet
  var ss = SpreadsheetApp.getActiveSpreadsheet();
  var ss = SpreadsheetApp.openById('CHANGEME-SpreadsheetID');
  var sheet = ss.getSheetByName("SensorData");

  // Get all data from the sheet
  var values = sheet.getDataRange().getValues();
  console.log(values);
  sensorValue = values[values.length - 1][1];
  console.log(sensorValue);
  return sensorValue;
}
```

To be pasted in an HTML file called `frontend.html`:

```
<!DOCTYPE html>
<html>
<head>
  <title>Photocell Data Visualization</title>
  <script src="https://cdnjs.cloudflare.com/ajax/libs/p5.js/1.4.0/p5.min.js"></script>
</head>
<body>
  <script>
    let sensorValue = 0; 

    function setup() {
      createCanvas(600, 400);
      loadData(); // Load initial data
      setInterval(loadData, 10000); // Update every 10 seconds
    }

    function draw() {
      background(220);
      
      // Calculate the diameter based on sensorValue (map to a suitable range)
      let diameter = map(sensorValue, 0, 1023, 50, 300); 
      
      // Draw the circle
      fill(255, 0, 0); // Red color
      ellipse(width / 2, height / 2, diameter, diameter);
    }

    function loadData() {
      google.script.run.withSuccessHandler(processData).getSensorData();
    }

    function processData(data) {
      console.log(data);
      sensorValue = data;
      redraw(); // Redraw the canvas with the new data
    }
  </script>
</body>
</html>
```