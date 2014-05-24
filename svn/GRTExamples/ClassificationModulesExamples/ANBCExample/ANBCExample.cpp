/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @section LICENSE
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

 @example ANBCExample.cpp
 */

/*
 GRT ANBC Example
 This examples demonstrates how to initialize, train, and use the ANBC algorithm for classification.
 
 The Adaptive Naive Bayes Classifier (ANBC) is a naive but powerful classifier that works very well on both basic and more complex recognition problems.
 
 In this example we create an instance of an ANBC algorithm and then train the algorithm using some pre-recorded training data.
 The trained ANBC algorithm is then used to predict the class label of some test data.
 
 This example shows you how to:
 - Create an initialize the ANBC algorithm
 - Load some LabelledClassificationData from a file and partition the training data into a training dataset and a test dataset
 - Train the ANBC algorithm using the training dataset
 - Test the ANBC algorithm using the test dataset
 - Manually compute the accuracy of the classifier
*/

//You might need to set the specific path of the GRT header relative to your project
#include "GRT.h"
using namespace GRT;

int main (int argc, const char * argv[])
{
    
    //Create a new ANBC instance
    ANBC anbc;
    anbc.setNullRejectionCoeff( 10 );
    anbc.enableScaling( true );
    anbc.enableNullRejection( true );
    
    //Load some training data to train the classifier
    ClassificationData trainingData;
    
    if( !trainingData.loadDatasetFromFile("ANBCTrainingData.txt") ){
        cout << "Failed to load training data!\n";
        return EXIT_FAILURE;
    }
    
    //Use 20% of the training dataset to create a test dataset
    ClassificationData testData = trainingData.partition( 80 );
    
    //Train the classifier
    bool trainSuccess = anbc.train( trainingData );
    
    if( !trainSuccess ){
        cout << "Failed to train classifier!\n";
        return EXIT_FAILURE;
    }
    
    //Save the ANBC model to a file
    bool saveSuccess = anbc.saveModelToFile("ANBCModel.txt");
    
    if( !saveSuccess ){
        cout << "Failed to save the classifier model!\n";
        return EXIT_FAILURE;
    }
    
    //Load the ANBC model from a file
    bool loadSuccess = anbc.loadModelFromFile("ANBCModel.txt");
    
    if( !loadSuccess ){
        cout << "Failed to load the classifier model!\n";
        return EXIT_FAILURE;
    }
    
    //Use the test dataset to test the ANBC model
    double accuracy = 0;
    for(UINT i=0; i<testData.getNumSamples(); i++){
        //Get the i'th test sample
        UINT classLabel = testData[i].getClassLabel();
        vector< double > inputVector = testData[i].getSample();
        
        //Perform a prediction using the classifier
        bool predictSuccess = anbc.predict( inputVector );
        
        if( !predictSuccess ){
            cout << "Failed to perform prediction for test sampel: " << i <<"\n";
            return EXIT_FAILURE;
        }
        
        //Get the predicted class label
        UINT predictedClassLabel = anbc.getPredictedClassLabel();
        vector< double > classLikelihoods = anbc.getClassLikelihoods();
        vector< double > classDistances = anbc.getClassDistances();
        
        //Update the accuracy
        if( classLabel == predictedClassLabel ) accuracy++;
        
        cout << "TestSample: " << i <<  " ClassLabel: " << classLabel << " PredictedClassLabel: " << predictedClassLabel << endl;
    }
    
    cout << "Test Accuracy: " << accuracy/double(testData.getNumSamples())*100.0 << "%" << endl;
    
    return EXIT_SUCCESS;
}
