/* Natcar Team 8 */
/* Roland Zeng - Kevin Kim - Jeff Wang - Andrew Li */


#include <Servo.h>

/*********************  Teensey Constants *********************/
const int servo_pin = 6;
const int motor_pin1 = 9;
const int motor_pin2 = 10;

/*********************  Servo Constants *********************/
Servo myservo;
const int pos_mid = 88;
const int pos_max_left = 130;
const int pos_right = 45;

/*********************  Camera Constants *********************/
const int CLK = -1; //ToDo: Replace with correct pin assignment
const int SI = -1; //ToDo: Replace with correct pin assignment
const int AO = -1; //ToDo: Replace with correct pin assignment

/*********************  Calculation Variable Declarations *********************/
int pos; // position of servo ; adjusted with myservo.write(pos)

int PixelArrayRaw[128]; //stores each line of camera pixels as numbers
char PixelArray[128]; //stores edge and center of camera pixels

int prevMidpoint = 0; //previous value of where the middle of the track is
int curMidpoint = 0;   //current value of where the middle of the track is

/*********************  Setup *********************/
void setup() {
	//servo pin assignment
	myservo.attach(servo_pin);

	//motor pin assignment
	pinMode(motor_pin1, OUTPUT);
	pinMode(motor_pin2, OUTPUT);

	//camera pin assignment
  	pinMode(SI, OUTPUT);           // Set SI as  output.
  	pinMode(CLK, OUTPUT);          // Set CLK as output.
 	pinMode(AO, INPUT);            // Set AO as input.

Serial.begin(9600);

//Initialize Clock Process

digitalWrite(SI, HIGH);
digitalWrite(CLK, HIGH);
digitalWrite(SI, LOW);
digitalWrite(CLK, LOW);
for(int i = 0; i < 128; i++) {
digitalWrite(CLK, HIGH);
digitalWrite(CLK, LOW);
}



}

/*********************  Loop *********************/
void loop() {
	/* STEP 1 : Get the relative location of the line on the ground */

	int sensorValue = analogRead(AO);
// Starts pixel count.
digitalWrite(SI, HIGH);
digitalWrite(CLK, HIGH);
digitalWrite(SI, LOW);
digitalWrite(CLK, LOW);

//localMax and localMin store the minimum and maximum color spectrum
int localMax = -1000; //arbitrary values to be overwritten
int localMin = -1000; //same

// This loop iterates through and captures one line. Each pixel’s color value is stored
// localMin stores the smallest color value of the line
// localMax stores the largest color value
for (int i = 0; i < 128; i++) {
delayMicroseconds(sensorValue);  //  saturation time
PixelArrayRaw[i] = analogRead(A0);

if (localMax == -1000 || PixelArrayRaw[i] >= localMax)
	localMax = PixelArrayRaw[i];

if (localMin == -1000 || PixelArrayRaw[i] <= localMin)
localMin = PixelArrayRaw[i];

  digitalWrite(CLK, HIGH);
  digitalWrite(CLK, LOW);
}


	// Using localMax and localMin, we calculate the middle-point of brightness
	int midBrightness = (localMax + localMin) / 2;

	// Now we loop through each color:
// if it’s greater than the midpoint, assign it a value of 9
	// If it’s less than the mid brightness, assign it a value of 0
	// This will help us distinguish where the line is

	for (int i = 0; i < 128; i++) {
		if (PixelArrayRaw[i] >= midBrightness) PixelArrayRaw[i] = 9;
else PixelArrayRaw[i] = 0;
}

	// TEST CODE: Print everything on one line; ToDo: comment this out later
	for (int i = 0; i < 128; i++) Serial.print(PixelArrayRaw[i]);

	// Now we have should have something similar to the following output (for a single line)
	// 0000000099999900000000 where the 9s represent where the track is located

	// However, due to light fluctuations, we could have anomalies such as:
	// 000000900999990099000
	// Hence, to find where the true track is, we must look for the largest substring of 9s

	//ToDo: faults may lie in this code so let’s check this part thoroughly
	int maxcount = 0;
	int counter = 0;
	int edge1 = -10;
	int edge2 = -10;
	for (int i = 0; i < 128; i++) {
		if (PixelArrayRaw[i] == 9) {
		for (int j = i; j < 128; j++) {
			if (PixelArrayRaw[j] == 0) break;
			else counter++;
}
if (counter > maxcount) {
	edge1 = i;
	edge2 = j;
}
counter = 0;
i = j;
}
}

// Now, we’ve found where the two edges of the track are
// Hence, the observed midpoint is just the middle of the two edges
int observedMidpoint = (edge1 + edge2) / 2;

	// TEST CODE: Print everything on one line; ToDo: comment this out later
	Serial.println();
	for (int i = 0; i < 128; i++) {
		if (i == observedMidpoint) PixelArrayRaw[i] = 5;
	Serial.print(PixelArrayRaw[i]);
}
// The above test code should print out something like:
// 000090999599900000  where 5 is the midpoint of the track
// ToDo: Make sure this is the case so we can continue to the rest of the code


	// However, due to fluctuations in light, our observed midpoint will change all the time
	// We can fix this by ignoring any instance of when the change is below a certain value
	if (abs(prevMidpoint - observedMidpoint) < 10) { // ToDo: make the 10 a global const
		curMidpoint = prevMidpoint;
}
else {
curMidpoint = observedMidpoint;
}

	//Test Code; ToDo: remove later
	for (int i = 0; i < 128; i++) {
		if (i == edge1 || i == edge2) PixelArray[i] = ‘|’;
		else if (i == curMidpoint) PixelArray[i] = ‘x’;
		else PixelArray[i] = ‘ ‘;
	Serial.print(PixelArray[i]);
}

	/* STEP 2 : PID: adjust servo depending on last known location of car */
	// ToDo

	//prevMidpoint = curMidpoint;

	delay(20); //slight delay for the sake of the serial monitor. ToDo: remove this for sure!!!

}
