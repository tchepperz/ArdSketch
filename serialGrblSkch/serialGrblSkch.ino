/*Here is a basic arduino sketch to show how 
open a gcode file from sd card reader connected to an Arduino
and stream code via Serial to an other Arduino which is running grbl.

The Arduino Uno has one Serial port:
Serial on pins 0(RX) and 1(TX)
The Arduino Mega has four serial ports: 
Serial on pins 0(RX) and 1(TX), Serial1 on pins 19 (RX) and 18 (TX), 
Serial2 on pins 17 (RX) and 16 (TX),Serial3 on pins 15 (RX) and 14 (TX).

The 2 arduino must be connected between them as this:
RX(uno) to TX(Mega) and TX(Uno) to RX(Mega) and GND to GND.

You need to launch serial monitor to interact with the program.

You can replace the serial monitor interface by any
LCD touchscreen interface and then you almost have a CNC without any computer.

Enjoy!*/

#include <SD.h>

File myFile;
boolean restart = true;		

void setup(){

	Serial.begin(115200);
	Serial1.begin(115200);
}


void loop(){

	checkSD();
	while(restart){
		openFileSD();
		sendGcode();
	}
}



void checkSD(){

	// Check if SD card is OK

	// On the Ethernet Shield, CS is pin 4. It's set as an output by default.
	// Note that even if it's not used as the CS pin, the hardware SS pin 
	// (10 on most Arduino boards, 53 on the Mega) must be left as an output 
	// or the SD library functions will not work.

	while(!SD.begin(53)){					// change this to 10 on uno
		Serial.println("Please insert SD card...\n");
		delay(1000);
	}

	Serial.println("SD card OK...\n");
	delay(1000);
}

void openFileSD(){

	String fileName = "";
	char fileNameChar[100]={0};				// char array for SD functions arguments

	Serial.println("Enter name for a gcode file on SD : \n");
	emptySerialBuf(0);
	fileName=getSerial(0);

	for(int i=0;fileName.charAt(i)!='\n';i++)		//convert String in char array removing '\n'
		fileNameChar[i]=fileName.charAt(i);

	if(!SD.exists(fileNameChar)){				//check if file already exists
		Serial.print("-- ");
		Serial.print(fileNameChar);
		Serial.print(" doesn't exists");
		Serial.println(" --");
		Serial.println("Please select another file\n");
      	delay(200);
      	openFileSD();
    } 
    else{
     	myFile = SD.open(fileNameChar, FILE_READ);		//create a new file
     	Serial.print("-- ");
     	Serial.print("File : ");
		Serial.print(fileNameChar);
		Serial.print(" opened!");
		Serial.println(" --\n");
    }
}

void emptySerialBuf(int serialNum){

	//Empty Serial buffer
	

	if(serialNum==0){
	 	while(Serial.available())                      
			Serial.read();
	}
	else if(serialNum==1){
		while(Serial1.available())                      
    	Serial1.read();
    }
}

void waitSerial(int serialNum){

	// Wait for data on Serial
	//Argument serialNum for Serial number

 	boolean serialAv = false;

 	if(serialNum==0){
 		while(!serialAv){ 
			if(Serial.available())
     		serialAv=true;
 		}
	}
	else if(serialNum==1){
		while(!serialAv){ 
			if(Serial1.available())
			serialAv=true;
		}
	}
}

String getSerial(int serialNum){

	//Return String  from serial line reading
	//Argument serialNum for Serial number

	String inLine = "";
	waitSerial(serialNum);

	if(serialNum==0){
		while(Serial.available()){              
			inLine += (char)Serial.read();
			delay(2);
		}
		return inLine;
	}
	else if(serialNum==1){
		while(Serial1.available()){               
    		inLine += (char)Serial1.read();
    		delay(2);
 		}
		return inLine;
	}
}

void sendGcode(){

	//READING GCODE FILE AND SEND ON SERIAL PORT TO GRBL

	//START GCODE SENDING PROTOCOL ON SERIAL 1

	String line = "";

    Serial1.print("\r\n\r\n");			//Wake up grbl
    delay(2);
    emptySerialBuf(1);
    if(myFile){                                                                      
	    while(myFile.available()){		//until the file's end
	    	line = readLine(myFile);	//read line in gcode file 
	      	Serial.print(line);		//send to serials
	      	Serial1.print(line);
	      	Serial.print(getSerial(1));	//print grbl return on serial
		}
	}
	else
		fileError();

	myFile.close();
	Serial.println("Finish!!\n");
	delay(2000);
}

void fileError(){

	// For file open or read error

	Serial.println("\n");
	Serial.println("File Error !");
}

String readLine(File f){
	
	//return line from file reading
  
	char inChar;
	String line = "";

	do{
		inChar =(char)f.read();
    	line += inChar;
    }while(inChar != '\n');

	return line;
}
