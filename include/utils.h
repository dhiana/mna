#include "consts.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>

using namespace std;


void exitPolitely(int exitCode) {
    #if defined (WIN32) || defined(_WIN32)
    cout << endl << "Press any key to exit...";
    cin.get();
    cin.get();
    #endif
    exit(exitCode);
}


int openNetlistFile(int argc, char** argv, string &filepath, ifstream& netlistFile){

    switch(argc) {
        case 1: {
            cout << "Enter path to netlist file: ";
            cin >> filepath;
            break;
        }
        case 2: {
            filepath = argv[1];
            break;
        }
        default:
            cerr << "FAILURE: Too much information!" << endl;
            return EXIT_FAILURE;
    }

    netlistFile.open(filepath.c_str(), ifstream::in);
    bool debugBool = netlistFile.is_open();
    if(!netlistFile.is_open()){
        cerr << "\nFAILURE: Cannot open file " << filepath << endl;
        return EXIT_FAILURE;
    }
    return 0;
}

void printIntro(){
    cout << endl << "Modified Nodal Analysis"
         << endl << "Originally by Antonio Carlos M. de Queiroz (acmq@coe.ufrj.br)"
         << endl << "Modified by Dhiana Deva, Felipe de Leo and Silvino Vieira" << endl << endl;

    cout << "............o......o............................................." << endl <<
            ".........o............o.........................................." << endl << 
            ".......o................o........................................" << endl << 
            ".....o....................o......................................" << endl << 
            "....o......................o....................................." << endl << 
            "..o..........................o..................................." << endl << 
            ".o............................o.................................." << endl << 
            "o...............................o................................" << endl << 
            "..........---/\\/\\/\\--- ..........o.............-|(-.............o" << endl << 
            "..................................o............................o." << endl << 
            "...................................o..........................o.." << endl << 
            ".....................................o......................o...." << endl << 
            "......................................o....................o....." << endl << 
            "........................................o................o......." << endl << 
            "..........................................o............o........." << endl << 
            ".............................................o......o............" << endl << endl;
}
