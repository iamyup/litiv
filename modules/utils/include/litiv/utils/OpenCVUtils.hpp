
// This file is part of the LITIV framework; visit the original repository at
// https://github.com/plstcharles/litiv for more information.
//
// Copyright 2015 Pierre-Luc St-Charles; pierre-luc.st-charles<at>polymtl.ca
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "litiv/utils/CxxUtils.hpp"

namespace cv { // extending cv

    struct DisplayHelper;
    using DisplayHelperPtr = std::shared_ptr<DisplayHelper>;

    //! returns pixel coordinates clamped to the given image & border size
    static inline void clampImageCoords(int& nSampleCoord_X,int& nSampleCoord_Y,const int nBorderSize,const cv::Size& oImageSize) {
        if(nSampleCoord_X<nBorderSize)
            nSampleCoord_X = nBorderSize;
        else if(nSampleCoord_X>=oImageSize.width-nBorderSize)
            nSampleCoord_X = oImageSize.width-nBorderSize-1;
        if(nSampleCoord_Y<nBorderSize)
            nSampleCoord_Y = nBorderSize;
        else if(nSampleCoord_Y>=oImageSize.height-nBorderSize)
            nSampleCoord_Y = oImageSize.height-nBorderSize-1;
    }

    //! returns a random init/sampling position for the specified pixel position, given a predefined kernel; also guards against out-of-bounds values via image/border size check.
    template<int nKernelHeight,int nKernelWidth>
    static inline void getRandSamplePosition(const std::array<std::array<int,nKernelWidth>,nKernelHeight>& anSamplesInitPattern,
                                             const int nSamplesInitPatternTot,int& nSampleCoord_X,int& nSampleCoord_Y,
                                             const int nOrigCoord_X,const int nOrigCoord_Y,const int nBorderSize,const cv::Size& oImageSize) {
        int r = 1+rand()%nSamplesInitPatternTot;
        for(nSampleCoord_X=0; nSampleCoord_X<nKernelWidth; ++nSampleCoord_X) {
            for(nSampleCoord_Y=0; nSampleCoord_Y<nKernelHeight; ++nSampleCoord_Y) {
                r -= anSamplesInitPattern[nSampleCoord_Y][nSampleCoord_X];
                if(r<=0)
                    goto stop;
            }
        }
        stop:
        nSampleCoord_X += nOrigCoord_X-nKernelWidth/2;
        nSampleCoord_Y += nOrigCoord_Y-nKernelHeight/2;
        clampImageCoords(nSampleCoord_X,nSampleCoord_Y,nBorderSize,oImageSize);
    }

    //! returns a random init/sampling position for the specified pixel position; also guards against out-of-bounds values via image/border size check.
    static inline void getRandSamplePosition_3x3_std1(int& nSampleCoord_X,int& nSampleCoord_Y,const int nOrigCoord_X,const int nOrigCoord_Y,const int nBorderSize,const cv::Size& oImageSize) {
        // based on 'floor(fspecial('gaussian',3,1)*256)'
        static_assert(sizeof(std::array<int,3>)==sizeof(int)*3,"bad std::array stl impl");
        static const int s_nSamplesInitPatternTot = 256;
        static const std::array<std::array<int,3>,3> s_anSamplesInitPattern ={
                std::array<int,3>{19,32,19,},
                std::array<int,3>{32,52,32,},
                std::array<int,3>{19,32,19,},
        };
        getRandSamplePosition<3,3>(s_anSamplesInitPattern,s_nSamplesInitPatternTot,nSampleCoord_X,nSampleCoord_Y,nOrigCoord_X,nOrigCoord_Y,nBorderSize,oImageSize);
    }

    //! returns a random init/sampling position for the specified pixel position; also guards against out-of-bounds values via image/border size check.
    static inline void getRandSamplePosition_7x7_std2(int& nSampleCoord_X,int& nSampleCoord_Y,const int nOrigCoord_X,const int nOrigCoord_Y,const int nBorderSize,const cv::Size& oImageSize) {
        // based on 'floor(fspecial('gaussian',7,2)*512)'
        static_assert(sizeof(std::array<int,7>)==sizeof(int)*7,"bad std::array stl impl");
        static const int s_nSamplesInitPatternTot = 512;
        static const std::array<std::array<int,7>,7> s_anSamplesInitPattern ={
                std::array<int,7>{ 2, 4, 6, 7, 6, 4, 2,},
                std::array<int,7>{ 4, 8,12,14,12, 8, 4,},
                std::array<int,7>{ 6,12,21,25,21,12, 6,},
                std::array<int,7>{ 7,14,25,28,25,14, 7,},
                std::array<int,7>{ 6,12,21,25,21,12, 6,},
                std::array<int,7>{ 4, 8,12,14,12, 8, 4,},
                std::array<int,7>{ 2, 4, 6, 7, 6, 4, 2,},
        };
        getRandSamplePosition<7,7>(s_anSamplesInitPattern,s_nSamplesInitPatternTot,nSampleCoord_X,nSampleCoord_Y,nOrigCoord_X,nOrigCoord_Y,nBorderSize,oImageSize);
    }

    //! returns a random neighbor position for the specified pixel position, given a predefined neighborhood; also guards against out-of-bounds values via image/border size check.
    template<int nNeighborCount>
    static inline void getRandNeighborPosition(const std::array<std::array<int,2>,nNeighborCount>& anNeighborPattern,
                                               int& nNeighborCoord_X,int& nNeighborCoord_Y,
                                               const int nOrigCoord_X,const int nOrigCoord_Y,
                                               const int nBorderSize,const cv::Size& oImageSize) {
        int r = rand()%nNeighborCount;
        nNeighborCoord_X = nOrigCoord_X+anNeighborPattern[r][0];
        nNeighborCoord_Y = nOrigCoord_Y+anNeighborPattern[r][1];
        clampImageCoords(nNeighborCoord_X,nNeighborCoord_Y,nBorderSize,oImageSize);
    }

    //! returns a random neighbor position for the specified pixel position; also guards against out-of-bounds values via image/border size check.
    static inline void getRandNeighborPosition_3x3(int& nNeighborCoord_X,int& nNeighborCoord_Y,const int nOrigCoord_X,const int nOrigCoord_Y,const int nBorderSize,const cv::Size& oImageSize) {
        typedef std::array<int,2> Nb;
        static const std::array<std::array<int,2>,8> s_anNeighborPattern ={
                Nb{-1, 1},Nb{0, 1},Nb{1, 1},
                Nb{-1, 0},         Nb{1, 0},
                Nb{-1,-1},Nb{0,-1},Nb{1,-1},
        };
        getRandNeighborPosition<8>(s_anNeighborPattern,nNeighborCoord_X,nNeighborCoord_Y,nOrigCoord_X,nOrigCoord_Y,nBorderSize,oImageSize);
    }

    //! returns a random neighbor position for the specified pixel position; also guards against out-of-bounds values via image/border size check.
    static inline void getRandNeighborPosition_5x5(int& nNeighborCoord_X,int& nNeighborCoord_Y,const int nOrigCoord_X,const int nOrigCoord_Y,const int nBorderSize,const cv::Size& oImageSize) {
        typedef std::array<int,2> Nb;
        static const std::array<std::array<int,2>,24> s_anNeighborPattern ={
                Nb{-2, 2},Nb{-1, 2},Nb{0, 2},Nb{1, 2},Nb{2, 2},
                Nb{-2, 1},Nb{-1, 1},Nb{0, 1},Nb{1, 1},Nb{2, 1},
                Nb{-2, 0},Nb{-1, 0},         Nb{1, 0},Nb{2, 0},
                Nb{-2,-1},Nb{-1,-1},Nb{0,-1},Nb{1,-1},Nb{2,-1},
                Nb{-2,-2},Nb{-1,-2},Nb{0,-2},Nb{1,-2},Nb{2,-2},
        };
        getRandNeighborPosition<24>(s_anNeighborPattern,nNeighborCoord_X,nNeighborCoord_Y,nOrigCoord_X,nOrigCoord_Y,nBorderSize,oImageSize);
    }

    //! writes a given text string on an image using the original cv::putText (this function only acts as a simplification wrapper)
    static inline void putText(cv::Mat& oImg, const std::string& sText, const cv::Scalar& vColor, bool bBottom=false, const cv::Point2i& oOffset=cv::Point2i(4,15), int nThickness=2, double dScale=1.2) {
        cv::putText(oImg,sText,cv::Point(oOffset.x,bBottom?(oImg.rows-oOffset.y):oOffset.y),cv::FONT_HERSHEY_PLAIN,dScale,vColor,nThickness,cv::LINE_AA);
    }

    //! removes all keypoints from voKPs which fall on null values (or outside the bounds) of oROI
    static inline void validateKeyPoints(const cv::Mat& oROI, std::vector<cv::KeyPoint>& voKPs) {
        CV_Assert(!oROI.empty() && oROI.type()==CV_8UC1);
        std::vector<cv::KeyPoint> voNewKPs;
        for(size_t k=0; k<voKPs.size(); ++k) {
            if( voKPs[k].pt.x>=0 && voKPs[k].pt.x<oROI.cols &&
                voKPs[k].pt.y>=0 && voKPs[k].pt.y<oROI.rows &&
                oROI.at<uchar>(voKPs[k].pt)>0)
                voNewKPs.push_back(voKPs[k]);
        }
        voKPs = voNewKPs;
    }

    //! helper struct for image display & callback management (must be created via DisplayHelper::create due to enable_shared_from_this interface)
    struct DisplayHelper : public CxxUtils::enable_shared_from_this<DisplayHelper> {

        //! by default, comes with a filestorage algorithms can use for debug
        static DisplayHelperPtr create(const std::string& sDisplayName,
                                       const std::string& sDebugFSDirPath="./",
                                       const cv::Size& oMaxSize=cv::Size(1920,1080),
                                       int nWindowFlags=cv::WINDOW_AUTOSIZE);
        //! will reformat the given image, print the index and mouse cursor point on it, and show it
        void display(const cv::Mat& oImage, size_t nIdx);
        //! will reformat the given images, print the index and mouse cursor point on them, and show them horizontally concatenated
        void display(const cv::Mat& oInputImg, const cv::Mat& oDebugImg, const cv::Mat& oOutputImg, size_t nIdx);
        // @@@ add matvector-based display function for extra flexibility?

        //! will call cv::waitKey and block if m_bContinuousUpdates is false, and loop otherwise (also returns cv::waitKey's captured value)
        int waitKey(int nDefaultSleepDelay=1);
        //! desctructor automatically closes its window
        ~DisplayHelper();

        const std::string m_sDisplayName;
        const cv::Size m_oMaxDisplaySize;
        cv::FileStorage m_oDebugFS; // will be closed & printed when released, i.e. on destruction
        std::mutex m_oEventMutex; // should be always used if m_oLatestMouseEvent is accessed externally
        struct {
            cv::Point2i oPosition;
            cv::Size oDisplaySize;
            int nEvent;
            int nFlags;
        } m_oLatestMouseEvent;

    private:
        DisplayHelper(const std::string& sDisplayName,
                      const std::string& sDebugFSDirPath,
                      const cv::Size& oMaxSize,
                      int nWindowFlags);
        cv::Size m_oLastDisplaySize;
        bool m_bContinuousUpdates;
        bool m_bFirstDisplay;
        std::function<void(int,int,int,int)> m_oMouseEventCallback;
        void onMouseEventCallback(int nEvent, int x, int y, int nFlags);
        static void onMouseEvent(int nEvent, int x, int y, int nFlags, void* pData);
    };

    //! always-empty-mat, which allows functions to return const-references to an empty mat without dangling ref issues
    static const cv::Mat g_oEmptyMat = cv::Mat();
    //! always-empty-size, which allows functions to return const-references to an empty size without dangling ref issues
    static const cv::Size g_oEmptySize = cv::Size();
    //! returns an always-empty-mat by reference
    static inline const cv::Mat& emptyMat() {return g_oEmptyMat;}
    //! returns an always-empty-size by reference
    static inline const cv::Size& emptySize() {return g_oEmptySize;}

} //namespace cv
