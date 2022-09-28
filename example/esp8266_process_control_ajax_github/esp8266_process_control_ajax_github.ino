 /* This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  
  WARNING! MAY CAUSE UNPREDICABLE ACTION IN POWER CONNECTED DEVICES, USE AT OWN RISK.
  
  Discussion:Since the esp8266 has limited input/output pins this example uses PCF8574P expander. Using
  I2C protocol up to 8(only 5 used in example) PCF8574 may be used(address 0x20 to 0x27), P0-P4 configured 
  as output and P5-P7 configured as input for each IC(you may change this).
     The PCF8591 can only source 1mA but can sink 25mA,so output devices
   must be active low. I am using PCF8591, 8bit ADC for analog inputs(address 0x4F
  address pins all set to 5V) using op-amp to boost device analog output to 5V referance to
  input into the PCF8591. The example uses browser local storage so whe the page is reloaded the
  browser page and controlled devices maintains state. I am using the esp8266 pins for 
 analog outputs(PWM), however a I2C module is avialable if you need to free these pins.I am using
 MicroSoft edge as the browser, I don't know how the example will work with other browsers, works on
 my android tablet though.
*/

String header = "http/1.1 200 ok\r\ncontent-type:text/html\r\n\r\n";

const char webPage [] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
 <head>
 <meta name='viewport' content='width=device-width, initial-scale=1.0'/>
 <meta charset='utf-8'>
 <style>

  #div3,#div12,#div13{font-size:14px;color:red;}
  #pump, #fan,#heater,#temp,#valve,#dust{margin:10px 10px;text-align:center;float:left;  padding: 0 10px 0 10px;width:330px;height:270px;background-color:lightblue;} 
  h2 {text-align:center; } 
  #ppbutton,#lubPP,#ventfan,#combfan,#conveyheater, #combfanhtr,#lubePPOutlet,#gasValve,#dustSupp,#vibfilter{margin:10px auto; padding:10px 10px 10px 10px;  
   background-color:red; font-size: 18px;width:300px;}
label{display:block; width:300px; border:2px groove blue; background-color:violet;font-size:14px;padding:10px 0px;}
legend{font-size:18px;font-weight:bold;}
#div6,#div8,#div10,#div14{background-color:lightblue;width:300px;height:60px;color:brown;font-size:14px;font-weight:bold;
border:5px solid green;margin:5px 0px;}
   </style>
  </head>
  
<body>
 <fieldset id='pump'>
  <legend>Pump Control</legend>
  <input type="button" id = "ppbutton" onclick="switchDevice('Recirc Pump ','ppbutton');saveVal(this)"  value="Recirc Pump OFF"> 
  <input type="button" id="lubPP" onclick="switchDevice('Lube Pump ','lubPP');saveVal(this)"  value="Lube Pump OFF">
  <div id='div13'></div>
  <label for="speed" ><b> Conveyer Motor speed( 0% to 100%)</b></label>
   <input type="range"  onchange="fncRange('speed','Motor Speed=');saveVal(this)" id="speed" name="speed" min="0" max="100" step="5" value="0">
   <div id='div2'></div>
   <div id='div3'></div>
  </fieldset>
  
 <fieldSet id='fan'>
 <legend>Fan Control</legend>
   <input type="button" id = "ventfan" onclick="switchDevice('Vent Fan ','ventfan');saveVal(this)"  value="Vent Fan OFF">
    <input type="button" id = "combfan" onclick="switchDevice('Combustion Fan ','combfan');saveVal(this)"  value="Combustion Fan OFF">
     <label for="dampos" ><b> Damper Position( 0% to 100%)</b></label>
      <input type="range"onchange="fncRange('dampos','Louver Position=');saveVal(this)"id="dampos"name="dampos"min="0"max="100"step="5"value="0">
     <div id='div4'></div> 
 </fieldSet>

  <fieldSet id='heater'>
 <legend>Heater Control</legend>
   <input type="button" id = "conveyheater" onclick="switchDevice('Conveyor Gallery Heater ','conveyheater');
   saveVal(this)"  value="Conveyor Gallery Heater OFF">
    <input type="button" id = "combfanhtr" onclick="switchDevice('Combustion Fan Heater ','combfanhtr');
    saveVal(this)"  value="Combustion Fan Heater OFF">
    <div id='div12'></div>
     <label for="luboilcooler" ><b> Lube Oil Cooler V/V Position( 0% to 100%)</b></label>
      <input type="range"onchange="fncRange('luboilcooler','Lube Oil Cooler V/V Position=');
      saveVal(this)"id="luboilcooler"name="luboilcooler" min="0"max="100"step="5"value="0">
     <div id='div5'></div> 
 </fieldSet>

<fieldSet id='temp'>
<legend> Temperatures</legend>
<div id='div6'><p>Combustion Air Outlet Temperature</p><div id='div7'></div></div>
<div id='div8'><p>Lube Pump Outlet Temperature</p><div id='div9'></div></div>
<div id='div10'><p>Conveyor Gallery Temperature</p><div id='div11'></div></div>
</fieldSet>

<fieldSet id='valve'>
<legend>Valve Control</legend>
<input type="button" id = "lubePPOutlet" onclick="switchDevice('Lube Pump Outlet Valve ','lubePPOutlet');saveVal(this)"  value="Lube Pump Outlet Valve CLOSED">
<input type="button" id = "gasValve" onclick="switchDevice('Gas Supply Valve ','gasValve');saveVal(this)"  value="Gas Supply Valve CLOSED">
<input type="button" id="dustSupp" onclick="switchDevice('Soot Suppression Valve ','dustSupp');saveVal(this)"value="Soot Suppression Valve CLOSED">
</fieldSet>

<fieldSet id='dust'>
<legend>Dust Control</legend>
<div id='div14'><p>Dust Opacity Level</p><div id='div15'></div></div>
<label for="efilter" ><b> Electrostaic Filter Voltage( 0 to 100000V)</b></label>
 <input type="range"onchange="fncRange('efilter','Electrostatic Filter Voltage=');saveVal(this)"id="efilter"name="efilter"min="0"max="100"step="5"value="0">
     <div id='div16'></div> 
     <input type="button" id="vibfilter" onclick="switchDevice('E Filter Vibrate ','vibfilter');saveVal(this)" value="E Filter Vibrate OFF">
</fieldSet>

 <script>
var dis=document.getElementById('div2');
var disLube=document.getElementById('div3');

   function switchDevice(device,id) 
  {
       var button=document.getElementById(id);
       var buttonText = button.value;
             
     if (buttonText==device+ "OFF")
     {
       button.value = device+ "ON";
       button.style.backgroundColor="green";
       ajaxLoad(device+"ON"); 
     }
     else if(buttonText==device+ "ON")
     {
       button.value = device+ "OFF";
       button.style.backgroundColor="red";
       ajaxLoad(device+"OFF");
     }
      else if(buttonText==device+ "CLOSED")
     {
       button.value = device+ "OPEN";
       button.style.backgroundColor="green";
       ajaxLoad(device+"OPEN");
     }
      else if(buttonText==device+ "OPEN")
     {
       button.value = device+ "CLOSED";
       button.style.backgroundColor="red";
       ajaxLoad(device+"CLOSED");
     }
  }

 function fncRange(newId,deviceName){
var rangeVal=document.getElementById(newId);
var setRange=rangeVal.value;
  ajaxLoad(deviceName+setRange);
 }

 window.onload=function(){  var allIds=document.querySelectorAll('*[id]');

 for(var i=0;i<allIds.length;i++){
   var textVal=JSON.parse(localStorage.getItem(allIds[i].id));
 if(textVal==null)continue;
  document.getElementById(allIds[i].id).value=textVal[0];
  document.getElementById(allIds[i].id).style.backgroundColor=textVal[1];
 }
  }
  
function ajaxLoad(ajaxURL)
{
  //if(!ajaxRequest){ alert("AJAX is not supported."); return; }
 var  ajaxRequest =new XMLHttpRequest(); 
  ajaxRequest.open("GET",ajaxURL,true);
  ajaxRequest.onreadystatechange = function()//server(esp8266 returns response)
  {
  if(ajaxRequest.readyState == 4 && ajaxRequest.status==200)
    {
           var ajaxResult = ajaxRequest.responseText;
        }
  }
  ajaxRequest.send();
}
 setInterval(function(){lubePPStatus();gasVVStatus();lubePPOutVVStatus();},1000);

  function lubePPOutVVStatus(){
  var permLubePPOutVV=new XMLHttpRequest();
  permLubePPOutVV.onreadystatechange=function(){ if(permLubePPOutVV.readyState==4 && permLubePPOutVV.status==200){
    var lubePPOutVVStatus=permLubePPOutVV.responseText;
    document.getElementById("div13").innerHTML=lubePPOutVVStatus;
    if(lubePPOutVVStatus=="Lube Pump Outlet VV CLOSED/Lube Pump OFF"){
     document.getElementById("lubPP").value="Lube Pump OFF";
     document.getElementById("lubPP").style["background-color"]="red";
    }
  }
 };
 permLubePPOutVV.open("get","checkLubeOutVV",true);
 permLubePPOutVV.send();
 }

 function gasVVStatus(){
  var permGasVV=new XMLHttpRequest();
  permGasVV.onreadystatechange=function(){ if(permGasVV.readyState==4 && permGasVV.status==200){
    var gasVVStatus=permGasVV.responseText;
    document.getElementById("div12").innerHTML=gasVVStatus;
    if(gasVVStatus=="Gas Valve Closed/Combustion Heater OFF"){
     document.getElementById("combfanhtr").value="Combustion Fan Heater OFF";
     document.getElementById("combfanhtr").style["background-color"]="red";
    }
  }
 };
 permGasVV.open("get","checkGasVV",true);
 permGasVV.send();
 }
 
function lubePPStatus(){
  var permLubePP = new XMLHttpRequest();
   permLubePP.onreadystatechange = function() { if (permLubePP.readyState == 4 && permLubePP.status == 200) {
    var lubPPStatus=permLubePP.responseText;
      document.getElementById("div3").innerHTML =lubPPStatus;
      if(lubPPStatus=="Lube Pump OFF/Conveyor Speed 0%")
      {
document.getElementById("speed").value=0;
document.getElementById('div2').innerHTML="Conveyer Speed :"+0+"%";
}
     }
  };
  permLubePP.open("GET", "checkLubePP", true);
  permLubePP.send();
}

setInterval(function() {combTemp();lubeTempVal();convGallTemp();dustLevel()}, 2000);

function combTemp() {
    var combTempVal = new XMLHttpRequest();
   combTempVal.onreadystatechange = function() { if (combTempVal.readyState == 4 && combTempVal.status == 200) {
    var combTempResp=combTempVal.responseText;
      document.getElementById("div7").innerHTML ="Temp C="+combTempResp;
     }
  };
  combTempVal.open("GET", "cmbtemp", true);
  combTempVal.send();
}

function dustLevel(){
  var dustlevel=new XMLHttpRequest();
  dustlevel.onreadystatechange=function(){if(dustlevel.readyState==4&&dustlevel.status==200){
    var dustVal=dustlevel.responseText;
    document.getElementById('div15').innerHTML=dustVal+" particles/cm3";
  }
};
dustlevel.open("get","dust",true);
dustlevel.send();
}

  function lubeTempVal(){
  var lubeTemp = new XMLHttpRequest();
   lubeTemp.onreadystatechange = function() { if (lubeTemp.readyState == 4 && lubeTemp.status == 200) {
    var lubeTempVal=lubeTemp.responseText;
      document.getElementById("div9").innerHTML ="Temp C="+lubeTempVal;
     
     if(lubeTempVal>30){var borderColor=document.getElementById('div8');
     borderColor.style["background-color"]="yellow";}else{borderColor=document.getElementById('div8');
      borderColor.style["background-color"]="lightblue";
     }
       }
  };
  lubeTemp.open("GET", "lubetemp", true);
  lubeTemp.send();
}

  function convGallTemp(){
  var gallTemp = new XMLHttpRequest();
  gallTemp.onreadystatechange = function() { if (gallTemp.readyState == 4 && gallTemp.status == 200) {
    var gallTempVal=gallTemp.responseText;
      document.getElementById("div11").innerHTML ="Temp C="+gallTempVal;
     
     if(gallTempVal>27){var borderColor=document.getElementById('div10');
     borderColor.style["background-color"]="yellow";}else{borderColor=document.getElementById('div10');
      borderColor.style["background-color"]="lightblue";
     }
       }
  };
  gallTemp.open("GET", "galltemp", true);
  gallTemp.send();
}

function saveVal(el){

  var id=el.id;
  var val=el.value; 
    var col=el.style.backgroundColor;
  var value=[val,col];
 if(col==""&&id=="speed"){ document.getElementById('div2').innerHTML="Conveyer Speed :"+val+"%"; }
else if(col==""&&id=="dampos"){document.getElementById('div4').innerHTML="Damper Position :"+val+"%";}
else if(col==""&&id=="luboilcooler"){document.getElementById('div5').innerHTML="Lube Oil Cooler V/V position :"+val+"%";}
else if(col==""&&id=="efilter"){document.getElementById('div16').innerHTML="Electrostaic Filter Votage :"+val+"KV";}
  localStorage.setItem(id,JSON.stringify(value));
  
}
 </script>
</body>
</html>
)====="; 

#include <ESP8266WiFi.h>
 #include <Wire.h>

char ssid[] = "yourSSID";       //your wifi SSID
char pass[] = "yourpassword";          //your wifi password
WiFiServer espserver(80);

String request = "";
 int analogOut=0;
 int analogVal[4]={1,1,1,1};
 int numOfAnalogPins=4;
int analogPins[]={D6,D7,D3,D5};//same sequence as call for setRange function

 //pcf variables
 byte outputVal=B0;
boolean p0,p1,p2,p3,p4,p5,p6,p7;
boolean *expOut0[]={&p0,&p1,&p2,&p3,&p4};
boolean *expIn0[]={&p5,&p6,&p7};
boolean p8,p9,p10,p11,p12,p13,p14,p15;
boolean *expOut1[]={&p8,&p9,&p10,&p11,&p12};
boolean *expIn1[]={&p13,&p14,&p15};
boolean p16,p17,p18,p19,p20,p21,p22,p23;
boolean *expOut2[]={&p16,&p17,&p18,&p19,&p20};
boolean *expIn2[]={&p21,&p22,&p23};
boolean p24,p25,p26,p27,p28,p29,p30,p31;
boolean *expOut3[]={&p24,&p25,&p26,&p27,&p28};
boolean *expIn3[]={&p29,&p30,&p31};
boolean p32,p33,p34,p35,p36,p37,p38,p39;
boolean *expOut4[]={&p32,&p33,&p34,&p35,&p36};
boolean *expIn4[]={&p37,&p38,&p39};
boolean p40,p41,p42,p43,p44,p45,p46,p47;
boolean *expOut5[]={&p40,&p41,&p42,&p43,&p44};
boolean *expIn5[]={&p45,&p46,&p47};

 byte expWriteVal0=B11111111;//set p4-p7 as input write 1
 byte expWriteVal1=B11111111;
  byte expWriteVal2=B11111111;
  byte expWriteVal3=B11111111;
  byte expWriteVal4=B11111111;
   byte expWriteVal5=B11111111;
 long lastPrint=0;
 boolean state=0;
 long lastFlash=0;
boolean readPinVal=0;
 byte readVal=B0;
 //end of pcf variables
boolean &ventFAN=p1;
boolean &lubePP=p4;
boolean &combFAN=p9;
boolean &filVIB=p10;
boolean &recircPP=p12;
boolean &galHTR=p16;
boolean &sootSuppVV=p18;
boolean &combfanHTR=p20;
boolean &lubePPOutVV=p25;
boolean &combGasVV=p27;

long timeStamp=0;

void setup() 
{
 pinMode(D3,OUTPUT);
 digitalWrite(D3,LOW);//set normally high D3 low;
    Serial.begin(115200);
    Wire.begin();
    Serial.println("Serial started at 115200");
    Serial.println("ESP8266 Process Control");
       
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) 
    {
        Serial.print(".");
        delay(500);
    }
    Serial.println("");
    Serial.print(F("Wifi Connected to ")); Serial.print(F("IP address:")); Serial.println(WiFi.localIP()); 
    espserver.begin();
    
  } 

void loop() 
{
    // Check if a client has connected
   WiFiClient browclient = espserver.available();
    if (browclient)  { 
     request = browclient.readStringUntil('\r');
   // Serial.print("request: "); Serial.println(request); 

    if       ( request.indexOf("Recirc%20Pump%20ON") > 0 )  { writePcfPin(recircPP, LOW);  }
    else if  ( request.indexOf("Recirc%20Pump%20OFF") > 0 ) { writePcfPin(recircPP, HIGH);   }
    else  if  ( request.indexOf("Lube%20Pump%20ON") > 0 ) {if(!(readExp(p30,0x23))) {writePcfPin(lubePP,LOW);}}
      else if  ( request.indexOf("Lube%20Pump%20OFF") > 0 ) { writePcfPin(lubePP, HIGH);   }
       else  if  ( request.indexOf("Vent%20Fan%20ON") > 0 ) { writePcfPin(ventFAN, LOW);   }
        else  if  ( request.indexOf("Vent%20Fan%20OFF") > 0 ) { writePcfPin(ventFAN, HIGH);   }
    else  if  ( request.indexOf("Combustion%20Fan%20ON") > 0 ) { writePcfPin(combFAN, LOW);   }
        else  if  ( request.indexOf("Combustion%20Fan%20OFF") > 0 ) { writePcfPin(combFAN, HIGH);   } 
          else  if  ( request.indexOf("Conveyor%20Gallery%20Heater%20ON") > 0 ) { writePcfPin(galHTR, LOW);   }
         else  if  ( request.indexOf("Conveyor%20Gallery%20Heater%20OFF") > 0 ) { writePcfPin(galHTR, HIGH);   } 
       else  if  ( request.indexOf("Combustion%20Fan%20Heater%20ON") > 0 ) {if(!(readExp(p29,0x23))){ writePcfPin(combfanHTR, LOW);}}
         else  if  ( request.indexOf("Combustion%20Fan%20Heater%20OFF") > 0 ) { writePcfPin(combfanHTR, HIGH);   }  
       else if  ( request.indexOf("Motor%20Speed=") > 0 ){ if(!(readExp(p5,0x20))){setRange(request,D6);}}//Wemos board D6 otherwise 12
       else if(request.indexOf("checkLubePP")>0){if(readExp(p5,0x20)&&analogVal[0]>0)
       {browclient.print("Lube Pump OFF/Conveyor Speed 0%");analogWrite(D6,0);}else{browclient.print(" ");}}
       else if  ( request.indexOf("Louver%20Position=") > 0 ) {setRange(request,D7);} //Wemos board D7 otherwise 13
       else if  ( request.indexOf("Lube%20Oil%20Cooler%20V/V%20Position=") > 0 ) {setRange(request,D3);} //Wemos board D3 otherwise 0
        else if  ( request.indexOf("Electrostatic%20Filter%20Voltage=") > 0 ) {setRange(request,D5);}
        else if(request.indexOf("E%20Filter%20Vibrate%20OFF")>0){writePcfPin(filVIB,HIGH);}
        else if(request.indexOf("E%20Filter%20Vibrate%20ON")>0){writePcfPin(filVIB,LOW);}
       else if(request.indexOf("cmbtemp")>0){String tpRead =readTempDust8591(0x4f,2);browclient.print(tpRead);}
        else if(request.indexOf("lubetemp")>0){String tpRead =readTempDust8591(0x4f,1);browclient.print(tpRead);}
         else if(request.indexOf("galltemp")>0){String tpRead =readTempDust8591(0x4f,0);browclient.print(tpRead);}
         else if(request.indexOf("dust")>0){String dustConc=readTempDust8591(0x4f,3);browclient.print(dustConc);}
         else if(request.indexOf("Lube%20Pump%20Outlet%20Valve%20CLOSED")>0){writePcfPin(lubePPOutVV,HIGH);}
         else if(request.indexOf("Lube%20Pump%20Outlet%20Valve%20OPEN")>0){writePcfPin(lubePPOutVV,LOW);}
         else if(request.indexOf("Gas%20Supply%20Valve%20CLOSED")>0){writePcfPin(combGasVV,HIGH);}
         else if(request.indexOf("Gas%20Supply%20Valve%20OPEN")>0){writePcfPin(combGasVV,LOW);}
         else if(request.indexOf("Soot%20Suppression%20Valve%20CLOSED")>0){writePcfPin(sootSuppVV,HIGH);}
         else if(request.indexOf("Soot%20Suppression%20Valve%20OPEN")>0){writePcfPin(sootSuppVV,LOW);}
         else if(request.indexOf("checkGasVV")>0){if(readExp(p29,0x23)){writePcfPin(combfanHTR, HIGH);
         browclient.print("Gas Valve Closed/Combustion Heater OFF");}else{browclient.print(" ");}}
          else if(request.indexOf("checkLubeOutVV")>0){if(readExp(p30,0x23)){writePcfPin(lubePP, HIGH);
         browclient.print("Lube Pump Outlet VV CLOSED/Lube Pump OFF");}else{browclient.print(" ");}}
         
   else
    {
        browclient.flush();
        browclient.print( header );
        String webStr=webPage;
        browclient.print( webStr );   
        
     }
      }
     
}

void setRange(String requestVal,int pinOut)
{
      int index=requestVal.indexOf('=');
       String num=  requestVal.substring(index+1);
   int   rangeVal=num.toInt();
   analogOut=map(rangeVal,0,100,0,255);
          analogWrite(pinOut,analogOut);
          for(int h=0;h<numOfAnalogPins;h++)
          {
       if(analogPins[h]==pinOut){analogVal[h]=rangeVal;}
          }
      
}
  
void writePcfPin(boolean &pinNumb,boolean newState)
{
  for(int j=0;j<5;j++)
  {
    if(expOut0[j]==&pinNumb)
    {
      bitWrite(expWriteVal0,j,newState); 
      writeExp(expWriteVal0,0x20);
         }
    else if(expOut1[j]==&pinNumb)
    {
      bitWrite(expWriteVal1,j,newState);
      writeExp(expWriteVal1,0x21);
    }
     else if(expOut2[j]==&pinNumb)
    {
      bitWrite(expWriteVal2,j,newState);
      writeExp(expWriteVal2,0x22);
    }
      else if(expOut3[j]==&pinNumb)
    {
      bitWrite(expWriteVal3,j,newState);
      writeExp(expWriteVal3,0x23);
    }
      else if(expOut4[j]==&pinNumb)
    {
      bitWrite(expWriteVal4,j,newState);
      writeExp(expWriteVal4,0x24);
    }
      else if(expOut5[j]==&pinNumb)
    {
      bitWrite(expWriteVal5,j,newState);
      writeExp(expWriteVal5,0x25);
    }
  }
}

void writeExp(byte expWriteVal,int expAdd)
{
   //  Serial.println(expWriteVal,BIN);
   Wire.beginTransmission(expAdd);
   Wire.write(expWriteVal);
   Wire.endTransmission();
}

boolean readExp(boolean &expPinRead,int expAdd)
{
  byte expWriteValAll;

switch(expAdd)
{
  case 0x20:
  expWriteValAll=expWriteVal0;
    break;
   case 0x21:
  expWriteValAll=expWriteVal1;
    break;
   case 0x22:
  expWriteValAll=expWriteVal2;
    break;
   case 0x23:
  expWriteValAll=expWriteVal3;
    break;
   case 0x24:
  expWriteValAll=expWriteVal4;
   break;
   case 0x25:
  expWriteValAll=expWriteVal5;
    break;
}
{
  Wire.beginTransmission(expAdd);
   Wire.write(expWriteValAll);//write 1's for input p7,p6,p5
   Wire.endTransmission();
}

  Wire.beginTransmission(expAdd);
  Wire.requestFrom(expAdd,1);
  if(Wire.available())
  {
  readVal=Wire.read();
  }
  
  Wire.endTransmission();
  readVal=readVal&B11100000;
  
for(int j=0;j<3;j++)
{
  if(expIn0[j]==&expPinRead||expIn1[j]==&expPinRead||expIn2[j]==&expPinRead||expIn3[j]==&expPinRead||expIn4[j]==&expPinRead||expIn5[j]==&expPinRead)
  {
  readPinVal=bitRead(readVal,j+5);
   return readPinVal;
  }
}
}

String readTempDust8591(byte pcf8591Add,int analogPinIn)
{
  float analogPinInVal[4];
   byte readVal=B0;
  float voltage=0;
  int dustBin=0;
  for(byte t=0;t<4;t++)
  {
    Wire.beginTransmission(pcf8591Add);
    Wire.write(t);
  Wire.endTransmission();
   
  Wire.requestFrom(pcf8591Add,2);
  if(Wire.available())
  {
    Wire.read();
    readVal=Wire.read();
  }
  Wire.endTransmission();
 if(t==3){dustBin=readVal;}
    voltage=float(readVal)/255*5.0;
  //Serial.println(readVal,DEC);
  float temp=voltage/5*100;
 
  analogPinInVal[t]=temp;
 }
switch (analogPinIn)
{
  case 0:
    return String(analogPinInVal[0]);
    break;
  case 1:
   return String(analogPinInVal[1]);
    break;
  case 2:
      return String(analogPinInVal[2]);
 
    break;
    case 3:
    int dustVal=map(dustBin,50,255,0,1000);
    dustVal=constrain(dustVal,0,1000);
    return String(dustVal);
    break;
}
 }



  
 
