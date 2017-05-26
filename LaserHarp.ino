seint LaserState = LOW;                    // The variable that stores the state of the laser beam.
int sensor = 100; ;                         // Change this value to calibrate your harp's sensor
int delaylaser = 7;                     // If you increase this, the laser will be brighter, but the harp will be less fluid
int delaymotor = 3;                     // This variable affects the speed, and fluidity of the harp.
int stepsPerRide = 3;

int LaserPin =  7;                       // Tell the arduino that the laser is on pin 7

int motorPin1 = 8;                      //Use these names for the pin numbers.
int motorPin2 = 9;
int motorPin3 = 10;
int motorPin4 = 11;

int bit0 = 3;
int bit1 = 4;
int bit2 = 5;

int note4 = 0x35;
int note3 = 0x34;
int note2 = 0x32;
int note1 = 0x30;





int a, b, c, d, e, f, g, h, i = 0;       // Iniating the note status markers.

void setup()
{
	pinMode(8, OUTPUT);                    // Setup for the motor.
	pinMode(9, OUTPUT);
	pinMode(10, OUTPUT);
	pinMode(11, OUTPUT);

  pinMode(bit0, OUTPUT);
  pinMode(bit1, OUTPUT);
  pinMode(bit2, OUTPUT);
  WriteNote(-1);


	pinMode(LaserPin, OUTPUT);            // Setup for laser.


	pinMode(13, OUTPUT);                 // Setup for status led.

  //Serial.begin(9600);
	Serial.begin(31250);                 //  Start a serial communication channel for MIDI
}

void noteOn(int cmd, int pitch, int velocity)     // Function to play the notes
{
  //Serial.print("P: ");
	Serial.write(cmd);
  
	Serial.write(pitch);
  //Serial.print('\n');
	Serial.write(velocity);
}

void loop()
{

	digitalWrite(LaserPin, HIGH);               // Turn on the laser for the 1st beam.
  
  delay(delaylaser);
/*
  Serial.print('\n');
  Serial.print(analogRead(0));
  Serial.print('\n');
 */
	if ( (analogRead(0) > sensor ) && (a == 0) ) // If the sensor gets a signal, and the not is not playing:

	{
    WriteNote(-1);
    WriteNote(1);
		digitalWrite(13, HIGH);       // Switch on status led.
		noteOn(0x90, note1, 0x7F);    // Play note 1
		a++;                          // Change the status variable to one.
	}


	else if(analogRead(0) < sensor )             // If the sensor does not get a signal:

	{
		digitalWrite(13, LOW);         // Switch off the status led.
		noteOn(0x90, note1, 0x00);     // Stop playing note 1.
		a = 0;                         // Change the status variable to zero.
	}


	digitalWrite(LaserPin, LOW);                // Turn off the Laser.

	StepForward(stepsPerRide);
  
	delay(delaymotor);                          // Small pause


	digitalWrite(LaserPin, HIGH);              // Turn on the laser for the 2nd beam.
	delay(delaylaser);

	if( (analogRead(0) > sensor ) && (b == 0) ) // If the sensor gets a signal, and the not is not playing:

	{
    WriteNote(-1);
    WriteNote(2);
		digitalWrite(13, HIGH);              // Switch on status led.
		noteOn(0x90, note2, 0x7F);           // Play note 2
    
		b++;                                 // Change the status variable to one.
	}


	else if(analogRead(0) < sensor )           // If the sensor does not get a signal:

	{
		digitalWrite(13, LOW);          // Switch off the status led.
		noteOn(0x90, note2, 0x00);      // Stop playing note 2.
		b = 0;                          // Change the status variable to zero.
	}

	digitalWrite(LaserPin, LOW);                    // Turn off the Laser.

  StepForward(stepsPerRide);
  
	delay(delaymotor);                          // Small pause


	digitalWrite(LaserPin, HIGH);             // Turn on the laser for the 3rd beam.
	delay(delaylaser);

	if( (analogRead(0) > sensor ) && (c == 0) ) // If the sensor gets a signal, and the not is not playing:

	{
    WriteNote(-1);
    WriteNote(3);
		digitalWrite(13, HIGH);       // Switch on status led.
		noteOn(0x90, note3, 0x7F);    // Play note 3
		c++;                          // Change the status variable to one.
	}


	else if(analogRead(0) < sensor )           // If the sensor does not get a signal:
	{
		digitalWrite(13, LOW);        // Switch off the status led.
		noteOn(0x90, note3, 0x00);    // Stop playing note 2.
		c = 0;                        // Change the status variable to zero.
	}

	digitalWrite(LaserPin, LOW);                 // Turn off the Laser.

	StepForward(stepsPerRide);
 
	delay(delaymotor);                            // Small pause

	// Contiue in the same way for the rest of the code.

	digitalWrite(LaserPin, HIGH);               //This is beam 4
	delay(delaylaser);

	if( (analogRead(0) > sensor ) && (d == 0) )
	{
    WriteNote(-1);
    WriteNote(4);
		digitalWrite(13, HIGH);
		noteOn(0x90, note4, 0x7F);
		d++;
	}


	else if(analogRead(0) < sensor )
	{
		digitalWrite(13, LOW);
		noteOn(0x90, note4, 0x00);
		d = 0;
	}


	digitalWrite(LaserPin, LOW);

  StepBack(stepsPerRide);

	delay(delaymotor);


	digitalWrite(LaserPin, HIGH);    //This is beam 3
	delay(delaylaser);

	if( (analogRead(0) > sensor ) && (c == 0) )
	{
    WriteNote(-1);
    WriteNote(3);
		digitalWrite(13, HIGH);
		noteOn(0x90, note3, 0x7F);
		c++;
	}


	else if(analogRead(0) < sensor )

	{
		digitalWrite(13, LOW);
		noteOn(0x90, note3, 0x00);
		c = 0;
	}

	digitalWrite(LaserPin, LOW);

	StepBack(stepsPerRide);
  
	delay(delaymotor);

	digitalWrite(LaserPin, HIGH);    //This is beam 2
	delay(delaylaser);

	if( (analogRead(0) > sensor ) && (b == 0) )

	{
    WriteNote(-1);
    WriteNote(2);
		digitalWrite(13, HIGH);
		noteOn(0x90, note2, 0x7F);
		b++;
	}


	else if(analogRead(0) < sensor )

	{
		digitalWrite(13, LOW);
		noteOn(0x90, note2, 0x00);
		b = 0;
	}

	digitalWrite(LaserPin, LOW);

  StepBack(stepsPerRide);

	delay(delaymotor);


// Back to top.

}

void StepForward(int n){
  int i;
  for(i = 0; i < n ; i++){
    digitalWrite(motorPin1, HIGH);             // Move the motor to create the second beam.( One step forward)
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, LOW);
    delay(4);
    digitalWrite(motorPin1, LOW);             // Move the motor to create the second beam.( One step forward)
    digitalWrite(motorPin2, HIGH);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, LOW);
    delay(4);
    digitalWrite(motorPin1, LOW);             // Move the motor to create the second beam.( One step forward)
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, HIGH);
    digitalWrite(motorPin4, LOW);
    delay(4);
    digitalWrite(motorPin1, LOW);             // Move the motor to create the second beam.( One step forward)
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, HIGH);
    if( i != n-1){
      delay(4);
    }
  }
}

void StepBack(int n){
  int i;
  for(i = 0; i < n; i++){
    digitalWrite(motorPin1, LOW);             // Move the motor to create the second beam.( One step forward)
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, HIGH);
    delay(4);
    digitalWrite(motorPin1, LOW);             // Move the motor to create the second beam.( One step forward)
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, HIGH);
    digitalWrite(motorPin4, LOW);
    delay(4);
    digitalWrite(motorPin1, LOW);             // Move the motor to create the second beam.( One step forward)
    digitalWrite(motorPin2, HIGH);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, LOW);
    delay(4);
    digitalWrite(motorPin1, HIGH);             // Move the motor to create the second beam.( One step forward)
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, LOW);
    if( i != n-1){
      delay(4);
    }
  }
}

void WriteNote(int n){
  switch(n){
    case 1:
      digitalWrite(bit0, HIGH);
      digitalWrite(bit1, LOW);
      digitalWrite(bit2, LOW);
      break;
    case 2:
      digitalWrite(bit0, LOW);
      digitalWrite(bit1, HIGH);
      digitalWrite(bit2, LOW);
      break;
    case 3:
      digitalWrite(bit0, HIGH);
      digitalWrite(bit1, HIGH);
      digitalWrite(bit2, LOW);
      break;
    case 4:
      digitalWrite(bit0, LOW);
      digitalWrite(bit1, LOW);
      digitalWrite(bit2, HIGH);
      break;
    default:
      digitalWrite(bit0, LOW);
      digitalWrite(bit1, LOW);
      digitalWrite(bit2, LOW);
      break;
  }
}

