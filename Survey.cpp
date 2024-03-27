#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>

const int QuestionCount = 10;
const int AnswerCount = 4;

// vector for affiliations, order matters
std::vector<std::string> affiliations = {    
    {"Democrat", "Republican", "Green Party", "Independent"}
};

// create vector of survey data, order matters
std::vector<std::vector<std::string>> surveyData = {
    {"Who would make the best US President?", "Elizabeth Warren", "Mitt Romney", "Ralph Nader", "Not sure/someone else"},
    {"Which answer most closely resembles your opinion on healthcare?", "Single-Payer", "HSA", "Universal", "Not sure/something else"},
    {"How do you feel about taxes?", "Necessary to support public programs", "I prefer they go to defense spending", "I prefer they help protect our environment", "Not sure/something else"},
    {"How do you feel about gun rights?", "There should be increased regulations", "There should be very little regulation", "I'm indifferent", "Not sure/something else"},
    {"How do you feel about women's rights?", "Define 'woman'", "They can vote now what do they want?", "They should have rights!", "Not sure/something else"},
    {"When does life start?", "When a woman decides it starts", "When God said so", "From seed", "Not sure/something else"},
    {"What are your thoughts on polution?", "It's ugly", "Looks like money to me", "It needs to stop or we are all going to die", "Not sure/something else"},
    {"How do you feel about carpooling?", "More people should do it!", "Sit next to other people? No, thanks", "It needs to start or we are all going to die", "Not sure/something else"},
    {"What best describes your ideas on fiscal responsibility?", "Sacrifice now, reap rewards later", "Let the next generation worry about it", "Invest in the environment!", "Not sure/something else"},
    {"How do you feel about family?", "We are all family", "Nuclear", "Can you have a family in the Peace Corps?", "Not sure/something else"}
};

// Bayes Theorem
double bayesTheorem(double priorProbability, double likelihood, double evidence) {
    return ((likelihood * priorProbability) / evidence);
}

int main() {   

    // create a string that will hold user input of political affiliation
    std::string userPoliticalParty; 
    std::string haveAsked;
    
    for (int i = 0; i < QuestionCount; ++i) {

        // ask the question
        std::cout << "Question " << i + 1 << ": " << surveyData[i][0] << std::endl;
        for (int q = 1; q < AnswerCount + 1; ++q) {
            std::cout << "    Answer " << q << ": " << surveyData[i][q] << std::endl;
        }
        std::cout << std::endl;

        // get the input
        int userInput;
        std::cout << "Type the number of your choice:";
        std::cin >> userInput;

        // make sure user input is in range
        if(userInput >= 1 && userInput <= AnswerCount) {                    
            
            // figure out the affiliation associated with the answer given
            std::string questionAffiliation;
            for(int z = 1; z < affiliations.size() + 1; ++z) {

                if(userInput == z) {
                    questionAffiliation = affiliations[z - 1];
                }

            }

            // check if file exists
            std::string fileContent;
            std::ifstream file(questionAffiliation + ".txt");
            if (!file.is_open()) {

                // file doesn't exist, so create it
                std::ofstream newFile(questionAffiliation + ".txt");
                if (newFile.is_open()) {

                    // inital weight is 0.25
                    newFile << "0.25";
                    std::cout << "New file was created!";
                    newFile.close();

                } else {

                    std::cerr << "Oops, something went wrong creating the file.";
                    return 1;

                }       

            } else {

                //std::filesystem::path filePath = std::filesystem::absolute(questionAffiliation + ".txt");
                //std::cout << "File location: " << filePath << std::endl;

                // set fileContent to contents of the file
                std::getline(file, fileContent);

                // close file
                file.close();

                // get the prior probability
                double priorProbability;
                priorProbability = std::stod(fileContent);
                
                // did we get any file content?
                if (priorProbability < 0) {
                    std::cerr << "Error reading file: " << questionAffiliation << ".txt" << std::endl;
                    return 1;
                } else {                    
                    
                    // let's play with Bayes' Theorem                    
                    double likelihood = 0.8;
                    double evidence = (likelihood * priorProbability) + ((1 - likelihood) * ( 1 - priorProbability));
                    double posteriorProbability = bayesTheorem(priorProbability, likelihood, evidence);

                    // reopen the file for writing
                    std::ofstream outputfile(questionAffiliation + ".txt");

                    // verify the file is open
                    if (!outputfile.is_open()) {
                        std::cerr << "Error reopening file";
                        return 1;
                    }

                    // write back to the file
                    outputfile << std::to_string(posteriorProbability) << std::endl;

                    // close the file
                    outputfile.close();

                    // we are comfortable enough to make a prediction
                    if(posteriorProbability > .8 && i > 3) {

                        // if we already asked... don't ask again
                        if(haveAsked != "true") {

                            // we need to obtain the user's affiliation
                            std::cout << "What number corresponds to your political affiliation?" << std::endl;
                            std::cout << "1: Democrat" << std::endl;
                            std::cout << "2: Republican" << std::endl;
                            std::cout << "3: Green Party" << std::endl;
                            std::cout << "4: Independent" << std::endl;
                            std::cin >> userPoliticalParty;

                            if(std::stod(userPoliticalParty) > 0 && std::stod(userPoliticalParty) < 5) {
                                haveAsked = "true";
                            } else {
                                std::cout << "Oops... your answer needs to be between 1 and 5: " << std::endl;
                                std::cin >> userPoliticalParty;
                            }
                            

                        }       

                        // convert user's number answer into the text equiv
                        std::string affiliationMatch = affiliations[std::stod(userPoliticalParty) - 1];

                        // were we successful? if so... make prediction, If not... keep learing
                        if (questionAffiliation == affiliationMatch) {
                            
                            std::cout << "Political Affiliation Prediction: " << questionAffiliation << std::endl;
                            return 0;

                        } else if (i == QuestionCount - 1) { // are we out of questions to ask?

                            std::string predAffiliation = "";
                            double prevProbability = 0;

                            // if we didn't predict correctly, make the best guess we can
                            for (int n = 0; n < affiliations.size(); ++n) {

                                // open file
                                std::string fContent;
                                std::ifstream affiliationFile(affiliations[n] + ".txt");
                                if (!affiliationFile.is_open()) {
                                    std::cerr << "Error opening file";
                                    return 1;
                                } else {

                                    // get posterior probability
                                    std::getline(affiliationFile, fContent);

                                    if (n==0) {
                                        predAffiliation = questionAffiliation;
                                        prevProbability = std::stod(fContent);
                                    } else {
                                        
                                        if (std::stod(fContent) > prevProbability) {
                                            // update the most li
                                            predAffiliation = questionAffiliation;
                                        }
                                    }

                                    // close file
                                    affiliationFile.close();

                                }

                            }

                            // make our best guess
                            std::cout << "Political Affiliation Prediction: A very unique " << predAffiliation << std::endl;
                            return 0;

                        }
                            

                        
                    }

                    

                }

            }

        } else {
            std::cout << "Oops, your answer must be 1, 2, 3 or 4!" << std::endl;
        }

    }
    return 0;
}