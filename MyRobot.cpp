#include "WPILib.h"

class RobotDemo : public SimpleRobot
{
	RobotDrive myRobot; 	//drive train
	Joystick gamepad;		//driver joystick
	Joystick shooter;		//shooter joystick
	Talon wheel;			//talon for launcher wheel
	Victor vib_tal;			//talon for vibrator
	Solenoid launch;		//solenoid for launcher
	Solenoid rlaunch;		//return solenoid for launcher
	Talon hop1;				//talon for left hopper motor
	Talon hop2;				//talon for right hopper motor
	Compressor comp;		//compressor object
	AnalogChannel pot1;		//potentiometer for left motor
	AnalogChannel pot2;		//potentiometer for right motor
	Solenoid left_hang;		//left solenoid for hanger
	Solenoid right_hang;	//right solenoid for hanger
	Solenoid left_hang_down; 
	Solenoid right_hang_down;
	

public:
	RobotDemo(void):
		myRobot(1, 2),	//sets drive to inputs 1 and 2	
		gamepad(1),		//sets drive controller to port 1
		shooter(2),		//sets secondary controller to port 2
		wheel(8),		//sets launcher wheel talon to input 3
		vib_tal(3),		//sets vibrator talon to input 6
		launch(2),		//sets launcher solenoid to input 4
		rlaunch(1),		//sets return launcher solenoid to input 5
		hop1(5),		//sets left hopper motor talon to input 7
		hop2(9),		//sets right hopper motor talon to input 8
		comp(1, 2),		//sets compressor switch to 9 and relay to 10
		pot1(2),		//sets left potentiometer to input 11
		pot2(3),		//sets right potentiometer to input 12
		left_hang(3),	//sets left hanging solenoid to input 13
		right_hang(6),	//sets right hanging solenoid to input 14
		left_hang_down(4),
		right_hang_down(5)
	{
		myRobot.SetExpiration(0.1);
	}

	void Autonomous(void)
	{
		myRobot.SetSafetyEnabled(false);
	}

	void OperatorControl(void)
	{
		
		myRobot.SetSafetyEnabled(true);
		while (IsOperatorControl() && IsEnabled())
		{

			float left = gamepad.GetRawAxis(2);		 	 //left joystick on gamepad
			float right = gamepad.GetRawAxis(4);	 	 //right joystick on gamepad
			bool hang_up = gamepad.GetRawButton(6);	 	 //button to up hanger
			bool hang_down = gamepad.GetRawButton(8); 	 //button to down hanger
			bool shoot = shooter.GetRawButton(1);	 	 //button to shoot
			bool three_pt = shooter.GetRawButton(5);	 //button to set to 3 point slot
			bool two_pt = shooter.GetRawButton(4);	 	 //button to set to 2 point slot
			bool vibrator = shooter.GetRawButton(10);	 //button to run vibrator
			float speed = shooter.GetRawAxis(3);		 //manually control speed
			bool up_launch = shooter.GetRawButton(2);	 //brings launcher up
			bool down_launch = shooter.GetRawButton(3);	 //brings launcher down
			int pres_val = comp.GetPressureSwitchValue();//gets pressure value
			float pot_val1 = pot1.GetVoltage();			 //gets voltage of pot1
			float pot_val2 = pot2.GetVoltage();			 //gets voltage of pot2
			float angle1 = volt_to_ang(pot_val1);		 //changes voltage to angle
			float angle2 = volt_to_ang(pot_val2);		 //changes voltage to angle
			
			comp.Start();	//starts compressor

			if ( pres_val >= 120 )	//stops compressor if pressure reaches 120psi
				comp.Stop();

			myRobot.TankDrive(left, right); 	//drive with tank style (use both sticks)
			Wait(0.005);						//wait for a motor update time
						
			wheel.Set(speed);	 //turns on wheel for launching
			
			//sets the potentiometers so that the right potentiometer is 5% above or
			//5% below the left potentiometer based on the angle it is converted to
			if ( angle2 > (angle1*1.05) || angle2 < (angle1 - (angle1*.05)))
			{//checks to see if it is within range			
				if ( angle2 < (angle1*1.05))
				{//checks to see if it is less than the maximum accepted value
					if ( angle2 < (angle1 - angle1*.05))
					{//checks to see if it is less than the minimum accepted value
						while(angle2 < angle1)
						{//raises the right side while it is less than the left
							hop2.Set(1.0);
						}
					}
				}
				else
				{//if it makes it to this point it is a value greater 
				 //than the maximum accepted value
					while(angle2 > angle1)
					{//lowers the right side while it is greater than the left
						hop2.Set(-1.0);
					}
				}
			}
			
			if ( shoot == true ) //fires frisbee if button 1 is pressed
			{
				launch.Set(1);	 //activates solenoid to shoot frisbee
				rlaunch.Set(0);	 
			}
			else				 //returns launcher if button 1 is not pressed
			{
				launch.Set(0);	 //returns launcher for next shot
				rlaunch.Set(1);
			}
			
			if ( three_pt == true )		//command structure for 3-pt shot
			{
				wheel.Set(1.0);			//starts wheel spinning
				if ( angle1 > 29.24 )	//lowers launcher if it is too high
				{
					do
					{
						hop1.Set(-1.0);
						hop2.Set(-1.0);
					}while(angle1 > 29.24);
				}
				if ( angle1 < 29.24 )	//raises launcher if it is too low
				{
					do
					{
						hop1.Set(1.0);
						hop2.Set(1.0);
					}while(angle1 < 29.24);
				}
			}
			
			if ( two_pt == true )		//command structure for 2-pt shots
			{
				wheel.Set(1.0);			//starts wheel spinning
				if ( angle1 > 23.24 )	//lowers launcher if it is too high
				{
					do
					{
						hop1.Set(-1.0);
						hop2.Set(-1.0);
					}while(angle1 > 23.24);
				}
				if ( angle1 < 23.24 )	//raises launcher if it is too high
				{
					do
					{
						hop1.Set(1.0);
						hop2.Set(1.0);
					}while(angle1 < 23.24);
				}
			}
			
			if ( vibrator == true ) //activates vibrator if button 10 is pressed
			{
				vib_tal.Set(1.0);
			}
			if ( vibrator == false )
			{
				vib_tal.Set(0.0);
			}
			
			if ( up_launch == true ) //raises launcher when button 2 is pressed
			{
				hop1.Set(1.0);	//sets motor full speed positive
				hop2.Set(1.0);	//sets motor full speed positive
			}
			
			if ( down_launch == true ) //lowers launcher when button 3 is pressed
			{
				hop1.Set(-1.0);	//sets motor full speed reverse
				hop2.Set(-1.0);	//sets motor full speed reverse
			}
			
			if ( hang_up == true )	//raises hanging mechanism when button 6 is pressed
			{
				left_hang.Set(1);	
				right_hang.Set(1);
				left_hang_down.Set(0);
				right_hang_down.Set(0);
			}
			
			if ( hang_down == true )//lowers hanging mechanism when button 8 is pressed
			{
				left_hang.Set(0);
				right_hang.Set(0);
				left_hang_down.Set(1);
				right_hang_down.Set(1);
			}
		}
	}
	
	float volt_to_ang(float volt)	//converts voltage read from potentiometer to angle
	{
		float angle;
		angle = (volt - 2.4)*8.578 + 34; //formula used to convert
		return angle;
	}
};

START_ROBOT_CLASS(RobotDemo);

