/*
 GRT MIT License
 Copyright (c) <2012> <Nicholas Gillian, Media Lab, MIT>
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all copies or substantial
 portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
 GRT Multidimensional Regression Example
 This examples demonstrates how to initialize, train, and use the Multidimensional Regression algorithm.
 
 The Multidimensional Regression class acts as a meta-algorithm for regression that allows several one-dimensional 
 regression algorithms (such as Linear Regression), to be combined together to allow an M-dimensional signal to be 
 mapped to an N-dimensional signal. This works by training N separate regression algorithms (one for each dimension), 
 each with an M-dimensional input.

 In addition to enabling one-dimensional regression algorithms (such as Linear Regression) to be used for mapping 
 N-dimensional output signals, Multidimensional Regression can also be useful for multi-dimensional regression algorithms 
 (such as Multi Layer Perceptrons), as it enables you to train N separate MLP algorithms (one for each output signal), 
 which might provide better mapping results than trying to train one MLP algorithm that can successfully map all N-dimensions 
 at once.
 
 In this example we create an instance of an Multidimensional Regression algorithm, using Linear Regression as the regression 
 module and then train a regression model using some pre-recorded training data. The trained model is then used to perform 
 regression on the test data.

 Both the training and test datasets have 2 input dimensions and 3 target dimensions.
 
 This example shows you how to:
 - Create an initialize the Multidimensional Regression algorithm
 - Create a new instance of a GestureRecognitionPipeline and add the regression instance to the pipeline
 - Load some LabelledRegressionData from a file
 - Train the Multidimensional Regression algorithm using the training dataset
 - Test the Multidimensional Regression algorithm using the test dataset
*/

#include "GRT.h"
using namespace GRT;

int main (int argc, const char * argv[])
{
    //Turn on the training log so we can print the training status of the MultidimensionalRegression to the screen
    TrainingLog::enableLogging( true );
    
    //Load the training data
    RegressionData trainingData;
    RegressionData testData;
    
    if( !trainingData.loadDatasetFromFile("MultidimensionalRegressionTrainingData.txt") ){
        cout << "ERROR: Failed to load training data!\n";
        return EXIT_FAILURE;
    }
    
    if( !testData.loadDatasetFromFile("MultidimensionalRegressionTestData.txt") ){
        cout << "ERROR: Failed to load test data!\n";
        return EXIT_FAILURE;
    }
    
    //Make sure the dimensionality of the training and test data matches
    if( trainingData.getNumInputDimensions() != testData.getNumInputDimensions() ){
        cout << "ERROR: The number of input dimensions in the training data (" << trainingData.getNumInputDimensions() << ")";
        cout << " does not match the number of input dimensions in the test data (" << testData.getNumInputDimensions() << ")\n";
        return EXIT_FAILURE;
    }
    
    if( testData.getNumTargetDimensions() != testData.getNumTargetDimensions() ){
        cout << "ERROR: The number of target dimensions in the training data (" << testData.getNumTargetDimensions() << ")";
        cout << " does not match the number of target dimensions in the test data (" << testData.getNumTargetDimensions() << ")\n";
        return EXIT_FAILURE;
    }
    
    cout << "Training and Test datasets loaded\n";
    
    //Print the stats of the datasets
    cout << "Training data stats:\n";
    trainingData.printStats();
    
    cout << "Test data stats:\n";
    testData.printStats();
    
    //Create a new gesture recognition pipeline
    GestureRecognitionPipeline pipeline;
    
    //Add a MultidimensionalRegression instance to the pipeline and set the regression algorithm to LinearRegression
    pipeline.setRegressifier( MultidimensionalRegression( LinearRegression(), true ) );
    
    //Train the MultidimensionalRegression model, which will train 3 LinearRegression models
    cout << "Training MultidimensionalRegression model...\n";
    if( !pipeline.train( trainingData ) ){
        cout << "ERROR: Failed to train MultidimensionalRegression model!\n";
        return EXIT_FAILURE;
    }
    
    cout << "Model trained.\n";
    
    if( !pipeline.savePipelineToFile( "Pipeline" ) ){
        cout << "ERROR: Failed to save pipeline!\n";
        return EXIT_FAILURE;
    }
    
    if( !pipeline.loadPipelineFromFile( "Pipeline" ) ){
        cout << "ERROR: Failed to load pipeline!\n";
        return EXIT_FAILURE;
    }
    
    //Test the model
    cout << "Testing MultidimensionalRegression model...\n";
    if( !pipeline.test( testData ) ){
        cout << "ERROR: Failed to test MultidimensionalRegression model!\n";
        return EXIT_FAILURE;
    }
    
    cout << "Test complete. Test RMS error: " << pipeline.getTestRMSError() << endl;
    
    //Run back over the test data again and output the results to a file
    fstream file;
    file.open("MultidimensionalRegressionResultsData.txt", fstream::out);
    
    for(UINT i=0; i<testData.getNumSamples(); i++){
        vector< double > inputVector = testData[i].getInputVector();
        vector< double > targetVector = testData[i].getTargetVector();
        
        //Map the input vector using the trained regression model
        if( !pipeline.predict( inputVector ) ){
            cout << "ERROR: Failed to map test sample " << i << endl;
            return EXIT_FAILURE;
        }
        
        //Get the mapped regression data
        vector< double > outputVector = pipeline.getRegressionData();
        
        //Write the mapped value and also the target value to the file
        for(UINT j=0; j<outputVector.size(); j++){
            file << outputVector[j] << "\t";
        }
        for(UINT j=0; j<targetVector.size(); j++){
            file << targetVector[j] << "\t";
        }
        file << endl;
    }
    
    //Close the file
    file.close();
    
    return EXIT_SUCCESS;
}
