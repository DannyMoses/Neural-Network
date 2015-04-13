#include "../include/net.h"
#include "../include/trainingdata.h"
#include "../include/functions.h"

int main(int argc, char *argv[])
{
    int trainingPass = 0;
    int epochs=0;
    bool test=false;
    bool testFile=false;
    bool realtime = false;

    if(!realtime)
    {
        cout<<argc<<endl;
        return 0;
        srand(time(NULL));
        TrainingData trainData(argv[1]);

        // e.g., { 3, 2, 1 }
        vector<unsigned> topology;

        vector<vector<double>> inputValsA, targetValuesA;
        vector<double> inputVals, targetValues, resultValues;
        double minError=atof(argv[2]);
        int maxEpochs=atoi(argv[3]);

        ofstream trainingDataOutput;
        ofstream trainingDataGlobalOutput;
        ofstream testDataOutput;
        TrainingData *testDataInput;


        trainingDataOutput.open(argv[4]);
        trainingDataGlobalOutput.open(argv[5]);

        if(argc>6){
            if(!strcmp(argv[6],"-t"))
            {
                testDataOutput.open(argv[7]);
                test=true;
            }
            else if(!strcmp(argv[6],"-tf"))
            {
                testDataInput=new TrainingData(argv[7]);
                testDataOutput.open(argv[8]);
                testFile=true;
            }
        }
        //Gnuplot gp;
        double eta,momentum;
        string transferFunction;

        trainData.getTopology(topology);
        trainData.getEta(eta);
        trainData.getMomentum(momentum);
        trainData.getTransferFunction(transferFunction);


        //Load trainning data from file
        while (!trainData.isEof())
        {
            trainingPass++;

            // Get new input data and feed it forward:
            if (trainData.getNextInputs(inputVals) != topology[0])
                break;
            inputValsA.push_back(inputVals);

            // Train the net what the outputs should have been:
            trainData.getTargetOutputs(targetValues);
            assert(targetValues.size() == topology.back());
            targetValuesA.push_back(targetValues);
        }

        Net net(topology,transferFunction);
        double recentAverageError;
        double globalError;

        do
        {
            globalError=0.0;

            for (int i = 0; i < trainingPass; i++)
            {
                // Get new input data and feed it forward:
                net.feedForward(inputValsA[i]);

                // Collect the net's actual output results:
                net.getResults(resultValues);

                // Train the net what the outputs should have been:
                net.backPropagation(targetValuesA[i],eta,momentum);

                // Report how well the training is working, average over recent samples:
                recentAverageError=net.getRecentAverageError();
                globalError+=recentAverageError;
                trainingDataOutput <<recentAverageError<<endl;
                // gp << "plot " << recentAverageError<<endl;
            }
            globalError/=trainingPass;
            trainingDataGlobalOutput <<globalError<<endl;
            epochs++;

        }while(round(globalError,minError)>minError && epochs<maxEpochs);

        trainingDataOutput.close();
        trainingDataGlobalOutput.close();

        if (test)
        {
            for (int i = 0; i < trainingPass; i++)
            {
                // Get new input data and feed it forward:][]
                net.feedForward(inputValsA[i]);
                showVectorValues("Input:", inputValsA[i],testDataOutput);

                showVectorValues("Target_Output:", targetValuesA[i],testDataOutput);

                // Collect the net's actual output results:
                net.getResults(resultValues);
                showTestVectorValues("Output:", resultValues,testDataOutput);
            }
        }

        if (testFile)
        {
            trainingPass=0;
            //clear trainning data vectors
            inputValsA.clear();
            targetValuesA.clear();
            inputVals.clear();
            targetValues.clear();
            resultValues.clear();

            //Load trainning data from file
            while (!testDataInput->isEof())
            {
                trainingPass++;

                // Get new input data and feed it forward:
                if (testDataInput->getNextInputs(inputVals) != topology[0])
                    break;
                inputValsA.push_back(inputVals);

                // Train the net what the outputs should have been:
                testDataInput->getTargetOutputs(targetValues);
                assert(targetValues.size() == topology.back());
                targetValuesA.push_back(targetValues);
            }

            for (int i = 0; i < trainingPass; i++)
            {
                // Get new input data and feed it forward:][]
                net.feedForward(inputValsA[i]);
                showVectorValues("Input:", inputValsA[i],testDataOutput);

                showVectorValues("Target_Output:", targetValuesA[i],testDataOutput);

                // Collect the net's actual output results:
                net.getResults(resultValues);
                showVectorValues("Output:", resultValues,testDataOutput);
            }
        }

        testDataOutput.close();
    }

    if(realtime)
    {
        //Declare the architecture of your network
        //Example: vector<unsigned> topology (2,4,4,2);
        vector<unsigned> topology;

        //Restrict your node output to [-1, 1] with a
        //function such as the sigmoid function or tanh.
        string transferFunction = "sig";

        //Time to creat the neural network
        Net net(topology,transferFunction);

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

        trainingDataOutput.open("trainingDataOutput.txt");
        trainingDataGlobalOutput.open("trainingDataGlobalOutput.txt");

        //a variable to keep track of how many times
        //the NN has trained a set of data
        int trainingPass = 0;

        //acceptable error
        double eps = .0001;

        //Declare variables to store the useful information about
        //the netowrk
        vector<double> inputVals, targetValues, resultValues;

        //Target values are constant, set them here
        //declare your targetValues as such:
        //vector<double> targetValues (output1, output2, ...., outputn);

        //Keep training until your node weights have converged,
        //that is, if |weight_new - weight_old| < eps, assume
        //convergence
        //Or keep training until your error is minimal, this does
        //the latter
        while(recentAverageError < eps)
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
            //Send motor_values to the appropriate place
            
            //wait a finite amount of time, say 50 ms
            //to see what the motor values you just got
            //equate to in terms of sensor output
            //wait(50);
            //get updated sensor data

            //Store updataed sensor data as the output of the network,
            //aka the last layer
            //Example code:
            //vector<double> results;
            //net.setLayer(results, 4);

            // Train the net with what the outputs should have been:
            net.backPropagation(targetValues,eta,momentum);

            // Report how well the training is working, average over recent samples:
            recentAverageError=net.getRecentAverageError();
            globalError+=recentAverageError;
            trainingDataOutput << recentAverageError<< endl;
            globalError/=trainingPass;
            trainingDataGlobalOutput <<globalError<<endl;

            //increment iteration counter
            trainingPass++;

        } //<---End of while loop
    }
}
