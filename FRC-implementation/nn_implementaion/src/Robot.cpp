#include <constants.h>
#include <CounterBase.h>
#include <Encoder.h>
#include <Joystick.h>
#include <RobotBase.h>
#include <RobotDrive.h>
#include <SampleRobot.h>
#include <Timer.h>
#include <fstream>
#include <string>
#include <vector>

#include "../../../include/net.h"

/**
 * This is a demo program showing the use of the RobotDrive class.
 * The SampleRobot class is the base of a robot application that will automatically call your
 * Autonomous and OperatorControl methods at the right time as controlled by the switches on
 * the driver station or the field controls.
 *
 * WARNING: While it may look like a good choice to use for your code if you're inexperienced,
 * don't. Unless you know what you are doing, complex code will be much more difficult under
 * this system. Use IterativeRobot or Command-Based instead if you're new.
 */
class Robot: public SampleRobot
{
	RobotDrive myRobot; // robot drive system
	Joystick stick; // only 1 joystick
	Encoder leftEncoder; // encoders for this example
	Encoder rightEncoder;
	// similar to the example in source/
	int trainingPass = 0;
	int epochs = 0;
	bool test = false;
	bool testFile = false;
	vector<unsigned> topology
	{ 1, 4, 4, 1 };
	string transferFunction = "sig";
	Net net;
	//learning parameters, feel free to play around with these.
	//These will affect rate of convergence, but won't prevent
	//convergence if set to a reasonable value.

	double eta = 0.15;    //overall net learning rate, [0.0..1.0]
	double momentum = 0.5;   //multiplier of last deltaWeight, [0.0..1.0]
	//Variables to store the error of the network
	double recentAverageError;
	double globalError;

	//Create files to record the behavior of the netowrk over time
	ofstream trainingDataOutput;
	ofstream trainingDataGlobalOutput;

	//acceptable error
	double eps = .0001;

	//Declare variables to store the useful information about
	//the network
	vector<double> inputVals, resultValues;

	//Target values are constant, set them here
	//declare your targetValues as such:
	//vector<double> targetValues (output1, output2, ...., outputn);
	vector<double> targetValues
	{ 120.0f, 0.0f }; // 10 feet forward, then back again
	//Keep training until your node weights have converged,
	//that is, if |weight_new - weight_old| < eps, assume
	//convergence
	//Or keep training until your error is minimal, this does
	//the latter

public:
	Robot() :
			myRobot(0, 1, 2, 3), // these must be initialized in the same order
			stick(0),		// as they are declared above.
			leftEncoder(0, 1, true, Encoder::k4X), rightEncoder(2, 3, false,
					Encoder::k4X), net(topology, transferFunction)
	{
		myRobot.SetExpiration(0.1);
		leftEncoder.SetDistancePerPulse(DRIVE_DISTANCE_PER_PULSE);
		rightEncoder.SetDistancePerPulse(DRIVE_DISTANCE_PER_PULSE);
		recentAverageError = 0.0f; // to please the compiler warnings
		globalError = 0.0f;
		trainingDataOutput.open("trainingDataOutput.txt");
		trainingDataGlobalOutput.open("trainingDataGlobalOutput.txt");
	}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous()
	{
		myRobot.SetSafetyEnabled(false);

	}

	/**
	 * Runs the motors with arcade steering.
	 */
	void OperatorControl()
	{
		myRobot.SetSafetyEnabled(true);
		while (IsOperatorControl() && IsEnabled())
		{
			myRobot.ArcadeDrive(stick);
		}
	}

	/**
	 * Runs during test mode
	 */
	void Test()
	{
		while (recentAverageError < eps)
		{
			//clear inputs and results to ensure no lurking data
			inputVals.clear();
			resultValues.clear();

			//get your data, populate it in inputVals
			//with inputVals[trainingPass].push_back(int)

			// Get new input data and feed it forward:
			net.feedForward(inputVals);

			//Send your motor values, which are stored in the
			//last hidden layer to wherever they need to go.
			//Example code:
			//vector<double> motor_values;
			//motor_values = net.getLayerValues(3);
			vector<double> motor_values;
			motor_values = net.getLayerValues(3);
			//Send motor_values to the appropriate place
			float outputVal = (float) motor_values.back();
			myRobot.Drive(outputVal, 0.0f);
			//wait a finite amount of time, say 50 ms
			//to see what the motor values you just got
			//equate to in terms of sensor output
			//wait(50);
			Wait(50);
			//get updated sensor data
			//vector<double> results;
			vector<double> results;
			results.push_back(leftEncoder.GetDistance() + rightEncoder.GetDistance() / 2);
			//Store updataed sensor data as the output of the network,
			//Example code:
			//net.setLayer(results, 4);
			net.setLayer(results, 1);
			// Train the net with what the outputs should have been:
			net.backPropagation(targetValues, eta, momentum);

			// Report how well the training is working, average over recent samples:
			recentAverageError = net.getRecentAverageError();
			globalError += recentAverageError;
			trainingDataOutput << recentAverageError << endl;
			globalError /= trainingPass;
			trainingDataGlobalOutput << globalError << endl;

			//increment iteration counter
			trainingPass++;

		} //<---End of while loop
	}
};

START_ROBOT_CLASS(Robot);
