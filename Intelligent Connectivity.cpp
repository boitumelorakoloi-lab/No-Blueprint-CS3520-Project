#include <iostream>
#include <string>

// Structure to hold user information
struct User {
    std::string name;
    std::string location;
    double latitude;
    double longitude;
};

// Function to send SOS request (simulated)
void sendSOS(User user) {
    std::cout << "SOS Request Sent:" << std::endl;
    std::cout << "Name: " << user.name << std::endl;
    std::cout << "Location: " << user.location << std::endl;
    std::cout << "Latitude: " << user.latitude << std::endl;
    std::cout << "Longitude: " << user.longitude << std::endl;
}

int main() {
    User user;
    user.name = "John Doe";
    user.location = "New York";
    user.latitude = 40.7128;
    user.longitude = -74.0060;

    sendSOS(user);

    return 0;
}