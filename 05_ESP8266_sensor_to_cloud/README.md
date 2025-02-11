
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