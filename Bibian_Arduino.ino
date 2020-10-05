const int N=1;
const int S=2;
const int OFF=0;

unsigned long int t = 0, t0 = 0; // Records time
float iA; // Records current in coil A
float iB; // Records current in coil B
int button = 0; // Homing button variable
int ct; // Count variable

// Variables above this line                     

void setup() {  
  pinMode(12,OUTPUT); // Coil A: DIR
  pinMode(3,OUTPUT);  // Coil A: CURRENT
  pinMode(9,OUTPUT);  // Coil A: BRAKE

  pinMode(13,OUTPUT); // Coil B: DIR
  pinMode(11,OUTPUT); // Coil B: CURRENT
  pinMode(8,OUTPUT);  // Coil B: BRAKE

  Serial.begin(115200);
  
  shutdown_all();
}


// ********************** USE THESE SETTINGS TO PREVENT OVERHEATING *********************
int power_stall=50;     // Just enough to move. Use 7 for half-stepping, 50 for full-stepping
int power=50;           // Set from 30 to 100 depending on required torque. Higher values make the motor shield hotter.
                        // Use values <= 50 for half-stepping (because both coils are on simultaneously)
int step_duration=2500; // For very long step_duration (>7000) use low power to prevent overheating the shield.
                        // If motor gets stuck (especially for step_duration around 5500), increase power.

                        // ALWAYS CHECK THE MOTOR SHIELD FOR OVERHEATING !!
                        
// **************************************************************************************

void loop() { 
  
  t0 = micros(); // Records time since Arduino started running the program.
  ct = 0;
  
  while(button == 0){
    button = digitalRead(10); // Stepper does nothing until button is pressed.
  }

  delay(1000); // Gives the user 1 second.


  // *** <HOMING> ***
  button = digitalRead(10); // Button value should be low.
  
  while(button == LOW){
    button = digitalRead(10);
    coilPolarities(N, OFF); // Forward toward button
    delay(1);
    coilPolarities(OFF, N);
    delay(1);
    coilPolarities(S, OFF);
    delay(1);
    coilPolarities(OFF, S);
    delay(1);
    
    ct++;

    // *** CURRENT ***
    
    iA= float(analogRead(0)) /1023.0*5.0 / 1.65 * 1000.0;
    iB= float(analogRead(1)) /1023.0*5.0 / 1.65 * 1000.0;
    
    t = micros() - t0;
    Serial.print(String(t / 1000000) + "\t" + String(ct*4) + "\t");
    Serial.print(iA);
    Serial.print("\t");
    Serial.print(iB);

    // *** STEPS ***
    
    t = micros() - t0;
    Serial.println( String(t / 1000000) + "\t" + String(ct*4));

  } // End of while loop


  // *** HALF SPEED, REVERSE DRIVE ***
  
  step_duration = 10000; // Half speed
  power = 30; // Lower power because of long step duration
  
  for ( ct = 1; ct < 31; ct ++){  // 8mm per 1 rev. 5mm/8mm = (0.625 revs * 50 ct per rev) = 31.25 revs.
    coilPolarities(N, OFF); // Reverse away from button
    delay(2);
    coilPolarities(OFF,S);
    delay(2);
    coilPolarities(S, OFF);
    delay(2);
    coilPolarities(OFF, N);
    delay(2);
  }

  // *** CURRENT ***
    iA= float(analogRead(0)) /1023.0*5.0 / 1.65 * 1000.0;
    iB= float(analogRead(1)) /1023.0*5.0 / 1.65 * 1000.0;
    
    t = micros() - t0;
    Serial.print(String(t / 1000000) + "\t" + String(ct*4) + "\t");
    Serial.print(iA);
    Serial.print("\t");
    Serial.print(iB);

    // *** STEPS ***
    
    t = micros() - t0;
    Serial.println( String(t / 1000000) + "\t" + String(ct*4));

  button = digitalRead(10);
  power_stall = 7; // Half stepping
  ct = 0; // Reset count
  step_duration = 5000; // Half speed, half step

  while(button == LOW){
    button = digitalRead(10);

    coilPolarities(N, OFF); // Forward toward button
    delay(4);
    coilPolarities(OFF, N);
    delay(4);
    coilPolarities(S, OFF);
    delay(4);
    coilPolarities(OFF, S);
    delay(4);

    ct++;

    // *** CURRENT ***
    
    iA= float(analogRead(0)) /1023.0*5.0 / 1.65 * 1000.0;
    iB= float(analogRead(1)) /1023.0*5.0 / 1.65 * 1000.0;
    
    t = micros() - t0;
    Serial.print(String(t / 1000000) + "\t" + String(ct*4) + "\t");
    Serial.print(iA);
    Serial.print("\t");
    Serial.print(iB);

    // *** STEPS ***
    
    t = micros() - t0;
    Serial.println( String(t / 1000000) + "\t" + String(ct*4));

  }

  step_duration = 10000; // Quarter speed, half step

  for (int ct=1;ct<=188;ct++){    //8mm per 1 rev. 30mm/8mm = (3.75 revs * 50 ct per rev) = 187.5 revs.
    coilPolarities(N, OFF); // Reverse away from button
    delay(4);
    coilPolarities(OFF,S);
    delay(4);
    coilPolarities(S, OFF);
    delay(4);
    coilPolarities(OFF, N);
    delay(4);

    // *** CURRENT ***
    
    iA= float(analogRead(0)) /1023.0*5.0 / 1.65 * 1000.0;
    iB= float(analogRead(1)) /1023.0*5.0 / 1.65 * 1000.0;
    
    t = micros() - t0;
    Serial.print(String(t / 1000000) + "\t" + String(ct*4) + "\t");
    Serial.print(iA);
    Serial.print("\t");
    Serial.print(iB);

    // *** STEPS ***
    
    t = micros() - t0;
    Serial.println( String(t / 1000000) + "\t" + String(ct*4));
  }
  
  shutdown_all(); // Everything off
  while(true); // Sinkhole
}

// Functions below this line

void coilPolarities(int polarityA, int polarityB){
  int powerA=power, powerB=power;

  switch(polarityA){
    case N:
      digitalWrite(12, HIGH);
      break;
    case S:
      digitalWrite(12,LOW);
      break;
    default:
      digitalWrite(12,LOW);
      powerA=0;
  }
  switch(polarityB){
    case N:
      digitalWrite(13, HIGH);
      break;
    case S:
      digitalWrite(13,LOW);
      break;
    default:
      digitalWrite(13,LOW);
      powerB=0;
  }
  setCurrents(powerA, powerB);
}

void setCurrents(int powerA, int powerB){
  long int t0=0, PWM_cycle_duration=50;
  long int PWM_high_A=0, PWM_high_B=0, dutyA=0, dutyB=0;
  int first_pin=0, second_pin=0;
  long int first_time=0, second_time=0;
  
  if (powerA>100) powerA=100;
  if (powerA<0) powerA=0;
  if (powerB>100) powerB=100;
  if (powerB<0) powerB=0;

  dutyA=power_stall+int(powerA*(100-power_stall)/100);
  dutyB=power_stall+int(powerB*(100-power_stall)/100);
  
  PWM_high_A=int(PWM_cycle_duration*dutyA/100);
  PWM_high_B=int(PWM_cycle_duration*dutyB/100);

  if (PWM_high_A>=PWM_high_B){
    first_pin=11;
    second_pin=3;
    first_time=PWM_high_B;
    second_time=PWM_high_A;
  }
  else{
    first_pin=3;
    second_pin=11;
    first_time=PWM_high_A;
    second_time=PWM_high_B;
  }

  t0=micros();
  while(micros()-t0<step_duration){
    digitalWrite(first_pin, HIGH);
    digitalWrite(second_pin, HIGH);
    delayMicroseconds(first_time);
    digitalWrite(first_pin, LOW);
    delayMicroseconds(second_time-first_time);
    digitalWrite(second_pin, LOW);      
  }
}

void shutdown_all(){
  digitalWrite(12,LOW);
  digitalWrite(3,LOW);
  digitalWrite(9,LOW);

  digitalWrite(13,LOW);
  digitalWrite(11,LOW);
  digitalWrite(8,LOW);
}
// End of code
