/*
 * FractalCreator.cpp
 *
 *  Created on: Sep 21, 2015
 *      Author: johnpurcell
 */

#include "FractalCreator.hpp"
#include <iostream>
#include <chrono>

namespace bit {

    FractalCreator::FractalCreator(int width, int height) : width(width), height(height), histo_ptr(new int[Mandelbrot::MAX_ITER] { 0 }), fractal_ptr(new int[width * height] { 0 }), bitmap(width, height), zoomList(width, height)
    {
        zoomList.add(Zoom(width / 2, height / 2, 4.0 / width));
    }
    
    void FractalCreator::addRange(double rangeEnd, const Colouring& c)
    {
        range.push_back(rangeEnd * Mandelbrot::MAX_ITER);
        colours.push_back(c);
        
        if (firstRange) {numOfRanges.push_back(0);}
        
        firstRange = true;
    }
    
    int FractalCreator::getRange(int it) const
    {
        int ran = 0;
        
        for (int i = 1; i < range.size(); i++)
        {
            
            if (range[i] > it) {break;}
            
            ran = i;
        }
        return ran;
    }
    
    void FractalCreator::addZoom(const Zoom& zoom)
    {
        zoomList.add(zoom);
    }
    
    void FractalCreator::run(const string &name)
    {
        auto start = std::chrono::high_resolution_clock::now();
        std::cout << "#1. Started Generating" << std::endl;
        calcIter();
        std::cout << "#1. finished!" << std::endl;

        auto step_2 = std::chrono::high_resolution_clock::now();

        std::cout << "#2. Started Counting total Iter" << std::endl;
        totalIter();
        std::cout << "#2. finished!" << std::endl;

        auto step_3 = std::chrono::high_resolution_clock::now();

        std::cout << "#3. Started Counting total Iter" << std::endl;
        rangeTotal();
        std::cout << "#3. finished!" << std::endl;

        auto step_4 = std::chrono::high_resolution_clock::now();

        std::cout << "#4. Started Drawing" << std::endl;
        drawFrac();
        std::cout << "#4. finished!" << std::endl;

        auto step_5 = std::chrono::high_resolution_clock::now();

        std::cout << "#5. Started Writing down" << std::endl;
        writeBitmap(name);
        std::cout << "#5. finished!" << std::endl;

        auto end = std::chrono::high_resolution_clock::now();


        std::chrono::duration<double> diff = end-start;
        std::cout << "Time to for all " << diff.count() << " s\n";
        std::cout << "Time to for first step: " << (diff = step_2 - start).count() << " s\n";
        std::cout << "            second step: " << (diff = step_3 - step_2).count() << " s\n";
        std::cout << "            third step: " << (diff = step_4 - step_3).count() << " s\n";
        std::cout << "            forth step: " << (diff = step_5 - step_4).count() << " s\n";
        std::cout << "            fifth step: " << (diff = end - step_5).count() << " s\n";
    }
    
    
    void FractalCreator::calcIter()
    {
        std::thread th_arr[THREAD_CNT];
        int cell_h = height / LVL_OF_DIVISION;
        int cell_w = width / LVL_OF_DIVISION;
        int additional_h = height - (cell_h * LVL_OF_DIVISION);
        int additional_w = width - (cell_w * LVL_OF_DIVISION);
        int curr_x = 0;
        int curr_y = 0;
        int next_x = 0;
        int next_y = 0;


        for (int y = 0; y < LVL_OF_DIVISION; y++) {
            next_y = curr_y + cell_h;
            if(y == LVL_OF_DIVISION - 1){next_y += additional_h;}

            for (int x = 0; x < LVL_OF_DIVISION; x++) {
                next_x = curr_x + cell_w;
                if(x == LVL_OF_DIVISION - 1){next_x += additional_w;}
                task.push({dot(curr_x, curr_y), dot(next_x, next_y)});
                curr_x = next_x;
            }
            curr_x = 0;
            curr_y = next_y;
        }

//        for(int i = 0; i < THREAD_CNT; i++){
//            int next = current + for_one_th;
//            if(i == THREAD_CNT - 1){
//                next += additional;
//            }
//            th_arr[i] = std::thread(&FractalCreator::calcPartIter, this);
//            current = next;
//        }

        for(int i = 0; i < THREAD_CNT; i++){
            th_arr[i] = std::thread(&FractalCreator::calcPartIter, this);
        }

        for(auto &th: th_arr){
            th.join();
        }
    }

    void FractalCreator::calcPartIter() {
        dot first, second;
        task_mtx.lock();
        while(!task.empty()) {
            first = task.front().first;
            second = task.front().second;
            task.pop();
            task_mtx.unlock();

            for (int y = first.y; y < second.y; y++) {
                for (int x = first.x; x < second.x; x++) {
                    pair<double, double> coords = zoomList.doZoom(x, y);

                    int it = Mandelbrot::getIter(coords.first,
                                                 coords.second);

                    fractal_ptr[y * width + x] = it;

                    if (it != Mandelbrot::MAX_ITER) {
                        histo_mtx.lock();
                        histo_ptr[it]++;
                        histo_mtx.unlock();
                    }
                }
            }
            std::cout << "  [" << progress_cnt++ << "/"<< CELL_CNT << "] Part from (" << first.x << "; " << first.y << ") to (" << second.x << "; "<< second.y << ") done!!"
                      << std::endl;
        }
        task_mtx.unlock();
    }
    
    void FractalCreator::rangeTotal()
    {
        
        int rangeIndex = 0;
        
        for (int i = 0; i < Mandelbrot::MAX_ITER; i++)
        {
            int pixels = histo_ptr[i];
            
            if (i >= range[rangeIndex + 1]) {rangeIndex++;}
            
            numOfRanges[rangeIndex] += pixels;
        }
    }
    
    void FractalCreator::totalIter()
    {
        
        for (int i = 0; i < Mandelbrot::MAX_ITER; i++)
        {
            total += histo_ptr[i];
        }
    }
    
    void FractalCreator::drawFrac(){
        std::thread th_arr[THREAD_CNT];
        int for_one_th = height / THREAD_CNT;
        int additional = height - (for_one_th * THREAD_CNT);
        int current = 0;

        for(int i = 0; i < THREAD_CNT; i++){
            int next = current + for_one_th;
            if(i == THREAD_CNT - 1){
                next += additional;
            }
            th_arr[i] = std::thread(&FractalCreator::drawPartFrac, this, current, next);
            current = next;
        }

        for(auto &th: th_arr){
            th.join();
        }
    }

    void FractalCreator::drawPartFrac(int begin, int end)
    {
        progress_cnt = 1;
        for (int y = begin; y < end; y++) {
            for (int x = 0; x < width; x++)
            {
                int it = fractal_ptr[y * width + x];
                
                int ran = getRange(it);
                int rangeTotal = numOfRanges[ran];
                int rangeStart = range[ran];
                
                Colouring& startColor = colours[ran];
                Colouring& endColor = colours[ran + 1];
                Colouring colorDiff = endColor - startColor;
                
                uint8_t red = 37;
                uint8_t green = 37;
                uint8_t blue = 37;

                if (it != Mandelbrot::MAX_ITER) {

                    int totalPixels = 0;

                    for (int i = rangeStart; i <= it; i++) {totalPixels += histo_ptr[i];}

                    red = startColor.r
                    + colorDiff.r * (double) totalPixels / rangeTotal;
                    green = startColor.g
                    + colorDiff.g * (double) totalPixels / rangeTotal;
                    blue = startColor.b
                    + colorDiff.b * (double) totalPixels / rangeTotal;

                }

                bitmap.setPixel(x, y, red, green, blue);

//                bitmap.setPixel(x, y, startColor.r, startColor.g, startColor.b);
            }
        }
        std::cout << "  [" << progress_cnt++ << "] Part from "<< begin <<" to "<< end << " done!!" << std::endl;
    }
    
    void FractalCreator::writeBitmap(string name)
    {
        bitmap.writeBMP(name);
    }
    
    
    FractalCreator::~FractalCreator(){}

    void FractalCreator::setThCnt(int th_cnt) {
        this->THREAD_CNT = th_cnt;
        CELL_CNT = THREAD_CNT * 10;
        LVL_OF_DIVISION = sqrt(CELL_CNT);
    }

}
