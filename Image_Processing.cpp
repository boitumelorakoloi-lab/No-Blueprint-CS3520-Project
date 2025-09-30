#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdint>
using namespace std;

struct Object{
    string Name;            // label / user name
    double* Descriptor;   // assuming zonesX=4, zonesY=4, bins=8 → 4*4*8=128

	Object(string name, int length){
        this->Name = name;
        this->Descriptor = new double[length];
	}
	void Print(){
		cout << Name << " - (";
		for(int i=0; i < 128; i++){
			cout << Descriptor[i];
			if(i < 127){
				cout << ", ";
			}
		}
		cout << ")"<< endl;
	}
};

struct Gradient {
    double Magnitude;
    double Angle;
    void Print(){
        cout << "("<< this->Magnitude << ", " << this->Angle << ")"<< endl;
    }
};

bool writeArrayToCSV(Object& object, int length) {
    ofstream file(object.Name+".csv");
    if (!file.is_open()) {
        cerr << "Failed to open file: " << object.Name << ".csv\n";
        return false;
    }
    for(int i=0; i < length; i++){
        file << object.Descriptor[i];
        if(i < length-2){
            file << ",";
        }
    }
    file.close();
    return true;
}

template<int H, int W>
bool loadPGM_P5(const string& filename, uint8_t img[H][W]) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) return false;

    string line;
    // Read magic number
    getline(file, line);
    if (line.find("P5") == string::npos) return false;

    // Skip comments / blank lines
    do { getline(file, line); } while(line.empty() || line[0] == '#');

    // Read width and height
    int w, h;
    istringstream wh(line);
    wh >> w >> h;
    if (w != W || h != H) return false;

    // Read max value
    int maxVal;
    file >> maxVal;
    file.ignore(1); // skip one whitespace character after maxVal

    // Read binary pixels
    file.read(reinterpret_cast<char*>(img), H * W);
    if (!file) return false;

    return true;
}

// Sobel filter convolution on a fixed-size image
template<int H, int W>
void computeGradients(uint8_t img[H][W], Gradient grads[H][W]) {
	// Sobel filters for X
    int Gy[3][3] = {
        {-1, -2, -1},
        { 0,  0,  0},
        { 1,  2,  1}
    };
	// Sobel filters for Y
    int Gx[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };

    for (int y = 1; y < H-1; y++) {
        for (int x = 1; x < W-1; x++) {
            int sumX = 0, sumY = 0;
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    int pixel = img[y+i][x+j];
                    sumX += pixel * Gx[i+1][j+1];
                    sumY += pixel * Gy[i+1][j+1];
                }
            }
            double mag = sqrt(sumX*sumX + sumY*sumY);
            double ang = atan2(sumY, sumX);
            grads[y][x] = {mag, ang};
        }
    }
}

// zonesX × zonesY × bins descriptor
template<int H, int W, int zonesX=4, int zonesY=4, int bins=8>
void buildDescriptor(Gradient grads[H][W], double descriptor[zonesX * zonesY * bins]) {
    const int zoneH = H / zonesY;
    const int zoneW = W / zonesX;
    const int descSize = zonesX * zonesY * bins;

    // clear descriptor
    for (int i = 0; i < descSize; i++) descriptor[i] = 0.0;

    int idx = 0;
    for (int zy = 0; zy < zonesY; zy++) {
        for (int zx = 0; zx < zonesX; zx++) {
            double hist[bins];
            for (int b = 0; b < bins; b++) hist[b] = 0.0;

            for (int y = zy * zoneH; y < (zy+1) * zoneH; y++) {
                for (int x = zx * zoneW; x < (zx+1) * zoneW; x++) {
                    double angle = grads[y][x].Angle;
                    if (angle < 0) angle += 2*M_PI;
                    int bin = static_cast<int>(bins * angle / (2*M_PI));
                    if (bin >= bins) bin = bins - 1; // clamp
                    hist[bin] += grads[y][x].Magnitude;
                }
            }

            // copy hist into descriptor
            for (int b = 0; b < bins; b++) {
                descriptor[idx++] = hist[b];
            }
        }
    }

    // normalize descriptor
    double norm = 0.0;
    for (int i = 0; i < descSize; i++) norm += descriptor[i] * descriptor[i];
    norm = sqrt(norm);
    for (int i = 0; i < descSize; i++) descriptor[i] /= (norm + 1e-6);
}


int main() {
    const int H = 32, W = 32;
	int zonesX = 4, zonesY = 4, bins = 8;
	int descSize = zonesX * zonesY * bins;

	Object obj1("Object_01", descSize);

    // Example: 8x8 grayscale image
    uint8_t img[H][W];


    if (!loadPGM_P5<H, W>("test.pgm", img)) {
        cerr << "Failed to load image\n";
        return 1;
    }

    // Declaring the gradiant
    Gradient grads[H][W] = {};

    // Compute Sovel Gradients
    computeGradients<H, W>(img, grads);

	// suppose grads is Gradient** (allocated [h][w])
	buildDescriptor<H, W>(grads, obj1.Descriptor);

	// Save to file
    writeArrayToCSV(obj1, descSize);
    obj1.Print();
	return 0;
}

