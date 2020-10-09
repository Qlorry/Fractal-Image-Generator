//
//  main.cpp
//  Fractal Image Generator
//
//  Created by Claire Fritzler on 2018-02-05.
//  Copyright © 2018 Claire Fritzler. All rights reserved.
//
#define DISTRIB true

#include <iostream>
#include "FractalCreator.hpp"
#include "Colouring.hpp"
#include "Zoom.h"

using namespace std;
using namespace bit;

int main(int argc, char **argv) {
//    int width = 8400;
//    int heigth = 3600;
    int width = 20000;
    int heigth = 20000;
    std::string name("Image");

    double zoom_mod_x = 50, zoom_mod_y = 50;
    double scale = 1;

    std::string argv_i;
    for(int i = 1; i < argc; i++){
        argv_i = argv[i];
        if(argv_i == "-r"){
            width = std::stoi(argv[++i]);
            heigth = std::stoi(argv[++i]);

            std::cout<<"Resolution set to: "<< width <<" x "<<heigth<<std::endl;
            continue;
        }
    }


    FractalCreator fractalCreator(width, heigth);

    for(int i = 1; i < argc; i++) {
        argv_i = argv[i];

        if(argv_i == "-r"){ i++; i++; continue;}
        if(argv_i == "-c"){
            int r, g, b;
            std::string colour = argv[++i];
            int first = colour.find(";");
            r = stoi(colour.substr(0, first));

            colour = colour.substr(first+1);
            first = colour.find(";");
            g = stoi(colour.substr(0, first));

            colour = colour.substr(first+1);
            b = stoi(colour);

            fractalCreator.addRange(stod(argv[++i]), Colouring(r, g, b));
            std::cout<<"Added colouring up to: "<< stod(argv[i]) <<" Colour: #"<<r << " " << g<< " " << b<<std::endl;

            continue;
        }
        if(argv_i == "-j"){
            fractalCreator.setThCnt(std::stoi(argv[++i]));

            std::cout<< fractalCreator.THREAD_CNT <<" threads will be used in parallel "<<std::endl;
            continue;
        }
        if(argv_i == "-z"){
            zoom_mod_x = std::stod(argv[++i]);
            zoom_mod_y = std::stod(argv[++i]);
            scale = std::stod(argv[++i]);

            std::cout<< "Set "<< scale <<" zoom on mods("<< zoom_mod_x<< "; "<<zoom_mod_y<<")"<<std::endl;
            continue;
        }
        if(argv_i == "-n"){
            name = argv[++i];

            std::cout<<"Output name: "<< name <<std::endl;

            continue;
        }
        if(argv_i == "-h" || argv_i == "help" || argv_i == "-help"){
            std::cout<<"How to use: drawing-fractal.exe [flag] [params] ...\n"
                       "TO set: 1)Resolution(default 20.000k x 20.000k) print: -r <width> <heigth>\n"
                       "        2)Colouring(mne len meniat defaults) you can only add new print: -c <r;g;b> <rangeEnd>\n"
                       "            Example: -c 0;0;0 0.07 will give you black for ranges up to 0.07 completed iterations\n"
                       "        3)Numer of used cores(I dont knew what случится если поставите больше чем у проца(повиснет нахуй))\n"
                       "            print: -j <num>\n"
                       "        4)Zoom(center = (50; 50) print: -z <point_x> <point_y> <scale>\n"
                       "            Example: -z 60 60 0.5 will give you 2x zoom on point 60 60\n"
                       "        5)Result file name print: -n <name>\n"
                       "            Example: -n \"Kirill loh\"\n";

            return 0;
        }
    }

    fractalCreator.addRange(0.0, Colouring(0, 0, 255));
    fractalCreator.addRange(0.05, Colouring(255, 99, 71));
    fractalCreator.addRange(0.08, Colouring(255, 150, 0));
    fractalCreator.addRange(1.0, Colouring(255, 255, 89));

    double zoom_x = 50 * (width/100);
    double zoom_y = 50 * (heigth/100);

    if(DISTRIB){
        zoom_x = zoom_mod_x * (width/100);
        zoom_y = zoom_mod_y * (heigth/100);
    }else{
        scale = 0.7;
    }

    fractalCreator.addZoom(Zoom(zoom_x, zoom_y, scale));
    fractalCreator.run(name + ".bmp");

    cout << "Finished." << endl;
    return 0;
}
