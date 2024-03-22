#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>

using namespace std;

struct QuestionData {
    string question;
    double answer;
};

vector<vector<string>> questions = {
    {"Do you support higher taxes?", "0.5"},
    {"Do you support stricter immigration policies?", "0.6"},
    {"Do you support government-funded healthcare?", "0.8"},
    {"Do you support stricter gun control laws?", "0.7"}
};

bool isValidAnswer(const string& answer) {
    double parsed;
    try {
        parsed = stod(answer);
    } catch (const invalid_argument&) {
        return false;
    }
    return parsed >= 1 && parsed <= 5;
}

bool isValidPoliticalAffiliation(const string& politicalAffiliation) {
    return !politicalAffiliation.empty();
}

void askQuestions(vector<QuestionData>& answers) {
    for (const auto& q : questions) {
        cout << q[0] << endl;
        string answer;
        do {
            cout << "Please enter your answer (1-5): ";
            cin >> answer;
        } while (!isValidAnswer(answer));
        QuestionData qData = {q[0], stod(answer) * stod(q[1])};
        answers.push_back(qData);
    }
}

string predictAffiliation(const vector<QuestionData>& answers) {
    // Load data from files
    vector<QuestionData> loadedData;
    for (const auto& q : questions) {
        ifstream file(q[0] + ".txt");
        if (!file) {
            cerr << "Error opening file: " << q[0] << ".txt" << endl;
            return "Unknown";
        }
        string line;
        getline(file, line);
        istringstream iss(line);
        double answer;
        if (!(iss >> answer)) {
            cerr << "Error parsing file: " << q[0] << ".txt" << endl;
            return "Unknown";
        }
        loadedData.push_back({q[0], answer});
    }

    // Calculate prediction
    double minDistance = numeric_limits<double>::max();
    string predictedAffiliation;
    for (const auto& loaded : loadedData) {
        double distance = 0;
        for (size_t i = 0; i < answers.size(); ++i) {
            distance += pow(answers[i].answer - loaded.answer, 2);
        }
        distance = sqrt(distance);
        if (distance < minDistance) {
            minDistance = distance;
            predictedAffiliation = loaded.question;
        }
    }
    return predictedAffiliation;
}

void saveDataToFile(const vector<QuestionData>& answers) {
    for (size_t i = 0; i < questions.size(); ++i) {
        ofstream file(questions[i][0] + ".txt");
        if (!file) {
            cerr << "Error opening file for writing: " << questions[i][0] << ".txt" << endl;
            return;
        }
        file << answers[i].answer << endl;
        file.close();
    }
}

int main() {
    vector<QuestionData> answers;
    askQuestions(answers);

    string politicalAffiliation;
    do {
        cout << "What is your political affiliation? ";
        cin.ignore();
        getline(cin, politicalAffiliation);
    } while (!isValidPoliticalAffiliation(politicalAffiliation));

    saveDataToFile(answers);
    string prediction = predictAffiliation(answers);
    cout << "Predicted political affiliation: " << prediction << endl;

    return 0;
}
