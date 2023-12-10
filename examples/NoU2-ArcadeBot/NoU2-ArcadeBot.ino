#include <PestoLink-Receive.h>
#include <Alfredo_NoU2.h>

// If your robot has more than a drivetrain and one servo, add those actuators here 
NoU_Motor frontLeftMotor(1);
NoU_Motor frontRightMotor(2);
NoU_Motor rearLeftMotor(3);
NoU_Motor rearRightMotor(4);
NoU_Servo servo(1);

// This creates the drivetrain object, you shouldn't have to mess with this
NoU_Drivetrain drivetrain(&frontLeftMotor, &frontRightMotor, &rearLeftMotor, &rearRightMotor);

void setup() {
	//EVERYONE SHOULD CHANGE "ESP32 Bluetooth" TO THE NAME OF THEIR ROBOT
	PestoLink.begin("ESP32 Bluetooth");

	// If a motor in your drivetrain is spinning the wrong way, change the value for it here from 'false' to 'true'
    frontLeftMotor.setInverted(false);
    frontRightMotor.setInverted(false);
    rearLeftMotor.setInverted(false);
    rearRightMotor.setInverted(false);

	// No need to mess with this code
    RSL::initialize();
    RSL::setState(RSL_ENABLED);
}

void loop() {
	// Here we define the variables we use in the loop
    int throttle = 0;
    int rotation = 0;
    int servoAngle = 0;

	//When PestoLink.update() is called, it returns "true" only if the robot is connected to phone/laptop  
    if (PestoLink.update()) {
        float throttle = -PestoLink.getAxis(1);
        float rotation = PestoLink.getAxis(0);
        
        drivetrain.arcadeDrive(throttle, rotation);
		
		// Here we decide what the servo angle will be based on if a button is pressed ()
		if (PestoLink.buttonHeld(0)) {
			servoAngle = 50;
		}
		else {
			servoAngle = 110;
		}
		
		// Here we set the drivetrain motor speeds and servo angle based on what we found in the above code
		servo.write(servoAngle);		
		
        RSL::setState(RSL_ENABLED);
    } else {
        RSL::setState(RSL_DISABLED);
    }

	// No need to mess with this code
    RSL::update();
}
