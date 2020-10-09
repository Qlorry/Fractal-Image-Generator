//
//  FractalCreator.hpp
//  Fractal Image Generator
//
//  Created by Claire Fritzler on 2018-02-07.
//  Copyright © 2018 Claire Fritzler. All rights reserved.
//

#ifndef FractalCreator_hpp
#define FractalCreator_hpp

#include <string>
#include <cstdint>
#include <memory>
#include <math.h>
#include <vector>
#include <mutex>
#include <thread>
#include <queue>
#include <atomic>

#include "Zoom.h"
#include "Mandelbrot.hpp"
#include "BitMap.h"
#include "ZoomList.hpp"
#include "Colouring.hpp"


namespace bit {
    struct dot{
        int x, y;

        dot(int x, int y){
            this->x = x;
            this->y = y;
        }
        dot(){
            this->x = 0;
            this->y = 0;
        }
    };


    class FractalCreator {
    private:
        int width;
        int height;
        unique_ptr<int[]> histo_ptr;
        unique_ptr<int[]> fractal_ptr;
        Bitmap bitmap;
        ZoomList zoomList;
        int total { 0 };
        
        vector<int> range;
        vector<Colouring> colours;
        vector<int> numOfRanges;
        
        bool firstRange{false};

        std::mutex histo_mtx, task_mtx;
        atomic_int16_t progress_cnt = 1;
        std::queue<std::pair<dot, dot>> task;

        int CELL_CNT, LVL_OF_DIVISION;

    private:
        void calcPartIter();
        void calcIter();
        void totalIter();
        void rangeTotal();
        void drawPartFrac(int begin, int end);
        void drawFrac();
        void writeBitmap(string name);
        int getRange(int it) const;
        
    public:
        int THREAD_CNT = 1;

        FractalCreator(int width, int height);
        void addRange(double rangeEnd, const Colouring& c);
        void addZoom(const Zoom& zoom);
        virtual ~FractalCreator();
        void run(const string &name);
        void setThCnt(int th_cnt);
    };
    
}

#endif
