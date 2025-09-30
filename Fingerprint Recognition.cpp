#include <iostream>
#include <vector>
#include <string>
#include <cmath>
using namespace std;

// ---- Type alias for fingerprint feature vector ----
using FeatureVec = vector<float>;

// ---- Simulate fingerprint feature vector (simplified) ----
FeatureVec simulateCapture(const string& id) {
    // Step 1: create a simple seed from the string
    int seed = 0;
    for (char c : id) seed += (int)c;  // sum ASCII codes

    // Step 2: lambda to generate deterministic numbers for each feature
    auto simpleRand = [seed](int i) -> float {
        return fmod((seed * 0.17 + i * 0.33), 1.0f);  // number between 0 and 1
    };

    // Step 3: fill feature vector
    FeatureVec features(10);
    for (int i = 0; i < 10; i++) {
        features[i] = simpleRand(i);
    }

    return features;
}

// ---- Cosine similarity function ----
float cosineSimilarity(const FeatureVec& a, const FeatureVec& b) {
    float dot = 0, normA = 0, normB = 0;
    for (size_t i = 0; i < a.size(); i++) {
        dot   += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    return dot / (sqrt(normA) * sqrt(normB) + 1e-6f);
}

// ---- Main demo: enroll and authenticate a user ----
int main() {
    string user = "alice";

    // Step 1: Enroll fingerprint
    FeatureVec enrolledTemplate = simulateCapture(user);
    cout << "[ENROLL] Template stored for " << user << endl;
    cout << endl;

    // Step 2: Authenticate fingerprint (simulated capture)
    FeatureVec currentScan = simulateCapture("bmjdf");
    float sim = cosineSimilarity(enrolledTemplate, currentScan);

    cout << "Similarity = " << sim << endl;
    if (sim >= 0.9f){
        cout << "MATCH - Phone unlocked " << endl;
        cout << endl;
    }
    else{
        cout << "NO MATCH - Access denied " << endl;
        cout << endl;
    }

    // Step 3: Try a different user
    FeatureVec otherScan = simulateCapture("bob");
    sim = cosineSimilarity(enrolledTemplate, otherScan);
    cout << "Similarity with 'bob' = " << sim << endl;
    if (sim >= 0.9f){
        cout << " MATCH - Phone unlocked " << endl;
        cout << endl;
    }
    else{
        cout << "NO MATCH - Access denied " << endl;
        cout << endl;
    }

    return 0;
}