#include <iostream>
#include <stdlib.h>
#include <string>
#include <iomanip>
#include <sstream>
#include "oscillator.h"

int main(int argc, char* argv[]) {
    if (argc != 1) {
        std::cout << "Usage: oscillator" << std::endl;
        return -1;
    }

    // create string stream and set parameters
    std::ostringstream stream;
    stream << std::setprecision(1);
    stream << std::fixed;

    // i/o for waveform and audio-file specifications
    std::string type;
    std::cout << "Select waveform type(sine, saw, square): " << std::endl;
    std::cin >> type;

    float frequency;
    std::string freqStr;
    std::cout << "Select frequency of waveform(0, 22050): " << std::endl;
    std::cin >> frequency;
    stream << frequency;
    freqStr = stream.str();
    stream.str("");

    float amplitude;
    std::string ampStr;
    std::cout << "Select amplitude of audio(0, 1): " << std::endl;
    std::cin >> amplitude;
    stream << amplitude;
    ampStr = stream.str();
    stream.str("");

    float duration;
    std::string durStr;
    std::cout << "Select duration of audio in seconds: " << std::endl;
    std::cin >> duration;
    stream << duration;
    durStr = stream.str();
    stream.str("");

    // creating oscillator object
    Oscillator* wave;
    if (type == "saw") {
        wave = new Saw(frequency, amplitude);
    } else if (type == "sine") {
        wave = new Sine(frequency, amplitude);
    } else if (type == "square") {
        wave = new Square(frequency, amplitude);
    } else {
        std::cout << "oscillator must be of type sine, saw, or square" << std::endl;
        return -1;
    }

    // creating outfile
    std::string outFileID = type + "_" + freqStr + "_" + ampStr + "_" + durStr + ".wav";
    std::ofstream outFile;
    outFile.open(outFileID, std::ios::binary);

    // checking if outfile is open
    if(!outFile.is_open()) {
        std::cout << "unable to create outfile\n";
        return -1;  
    }

    // RIFF Chunk
    outFile << "RIFF"; // chunkID
    outFile << "----"; // chunk size
    outFile << "WAVE"; 

    // fmt chunk
    outFile << "fmt "; // subchunk1ID
    writeToFile(outFile, 16, 4); // subchunk1Size
    writeToFile(outFile, 1, 2);  // audio format
    writeToFile(outFile, 1, 2); // num channels
    writeToFile(outFile, sampleRate, 4); //sample rate
    writeToFile(outFile, sampleRate * bitDepth / 8, 4); //byte rate
    writeToFile(outFile, bitDepth / 8, 2); // block align
    writeToFile(outFile, bitDepth, 2); // bits per sample

    // data chunk
    outFile << "data"; // subchunk2ID
    outFile << "----"; // data

    // writing to .wav file
    float maxAmplitude = pow(2, bitDepth - 1) - 1;
    int preAudioPosition = outFile.tellp();
    for (int i = 0; i < duration * sampleRate; i++) {
        float sample = wave->process();
        int intSample = static_cast<int>(sample * maxAmplitude);
        writeToFile(outFile, intSample, 2);
    }
    int postAudioPosition = outFile.tellp();

    // finishing header
    outFile.seekp(preAudioPosition - 4);
    writeToFile(outFile, postAudioPosition - preAudioPosition, 4);
    outFile.seekp(4, std::ios::beg);
    writeToFile(outFile, postAudioPosition - 8, 4);

    // closing outfile and returning
    outFile.close();
    std::cout << "wav file successfully created" << std::endl;
    return 0;
}