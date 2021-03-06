/*
    v4l2stereo
    A command line utility for stereoscopic vision
    Copyright (C) 2010 Bob Mottram and Giacomo Spigler
    fuzzgun@gmail.com

    Requires packages:
		libgstreamer-plugins-base0.10-dev
		libgst-dev

    sudo apt-get install libcv2.1 libhighgui2.1 libcvaux2.1 libcv-dev libcvaux-dev libhighgui-dev libgstreamer-plugins-base0.10-dev libgst-dev

    For details of use see:

        http://sluggish.homelinux.net/wiki/Libv4l2cam

    For details of the ELAS dense stereo algorithm see:

        http://rainsoft.de/software/libelas.html

        @INPROCEEDINGS{Geiger10,
        author = {Andreas Geiger and Martin Roser and Raquel Urtasun},
        title = {Efficient Large-Scale Stereo Matching},
        booktitle = {Asian Conference on Computer Vision},
        year = {2010},
        month = {November},
        address = {Queenstown, New Zealand}
        }

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* enable or disable gstreamer functionality */
//#define GSTREAMER

// ensure that openCV functions are defined within libcam
#define USE_OPENCV

#include <iostream>
//#include <cv.h>
//#include <highgui.h>

#include <opencv/cv.h>
#include <opencv/cxmisc.h>
#include <opencv/cvaux.h>
#include <opencv/highgui.h>

#include <stdio.h>
#include <sstream>
#include <omp.h>
#include <unistd.h>

#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef GSTREAMER
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappbuffer.h>
#endif

#include "anyoption.h"
#include "drawing.h"
#include "stereo.h"
#include "fast.h"
#include "libcam.h"

#include"stereovision.h"

#define VERSION			1.054

using namespace std;

/*!
 * \brief expands a subregion of the given image with pixel interpolation.  It is assumed that the subregion has the same aspect as the original.
 * \param img colour image data
 * \param img_width width of the image
 * \param img_height height of the image
 * \param tx subregion top left x coordinate
 * \param ty subregion top left y coordinate
 * \param bx subregion bottom right x coordinate
 * \param by subregion bottom right y coordinate
 * \param expanded returned expanded image
 */

int main(int argc, char* argv[])
{

     StereoVision* vision;
     vision = new StereoVision(320,240);

     int ww = 320;
     int hh = 240;
     int fps = 60;
     int skip_frames = 1;
     bool show_matches = false;
     bool show_depthmap = false;
     bool show_anaglyph = false;
     bool show_disparity_map = false;
     bool show_FAST = false;
     int grab_timeout_ms = 3000;

     bool headless = false;
     bool save_images = false;
     std::string dev0 = "/dev/video2";
     std::string dev1 = "/dev/video1";

     char system_call_buffer[50];
     sprintf(system_call_buffer,"su -c \"chmod 666 %s\"",  dev0.c_str());
     system(system_call_buffer);

     char system_call_bufferSecond[50];
     sprintf(system_call_bufferSecond,"su -c \"chmod 666 %s\"",  dev1.c_str());
     system(system_call_bufferSecond);

     std::string stereo_matches_filename = "";

     Camera c(dev0.c_str(), ww, hh, fps);
     Camera c2(dev1.c_str(), ww, hh, fps);

     std::string left_image_title = "Left image";
     std::string right_image_title = "Right image";

     IplImage *l=cvCreateImage(cvSize(ww, hh), 8, 3);
     IplImage *r=cvCreateImage(cvSize(ww, hh), 8, 3);

     IplImage *lg=cvCreateImage(cvGetSize(l),IPL_DEPTH_8U,1);
     IplImage *rg=cvCreateImage(cvGetSize(r),IPL_DEPTH_8U,1);

     CvMat* imageRectifiedPair=cvCreateMat(hh, ww*2,CV_8UC3 );

//     printf("Loading calibration file [%s]...", "calibrate.dat");
//     if(RESULT_OK == vision->calibrationLoad("/home/imocanu/calibrate.dat"))
//     {
//         printf("+OK- ALready calibrated OK");
//     }
//     else
//     {
//         printf("-FAIL open calibration file : calibrate.data");
//         vision->calibrationStart(9, 6);
//     }

     Size boardSize;
     boardSize = Size(9, 6);

     bool displayCorners = true;//true;
     const int maxScale = 2;
     const float squareSize = 1.f;  // Set this to your actual square size
     // ARRAY AND VECTOR STORAGE:

     vector<vector<Point2f> > imagePoints[2];
     vector<vector<Point3f> > objectPoints;
     Size imageSize;

     int i = 0, j = 0, k, nimages = 40;  // nr de perechi

     imagePoints[0].resize(nimages);
     imagePoints[1].resize(nimages);
     vector<IplImage*> goodImageList;

     bool useCalibrated=false;
     bool showRectified=true;
     bool finalCalibrated = true;
     bool isVerticalStereo  = true;

     Mat rmap[2][2];
     Rect validRoi[2];

     while (1)
     {

         if (!c.Update(&c2, 25, grab_timeout_ms))
         {
             printf("Failed to acquire images\n");
             break;
         }

         c.toIplImage(l);
         c2.toIplImage(r);

         /* display the left and right images */
         if ((!save_images) && (!headless) && (stereo_matches_filename == ""))
         {
             cvShowImage(left_image_title.c_str(), l);
             if ((!show_matches) &&
                     (!show_FAST) &&
                     (!show_depthmap) &&
                     (!show_anaglyph) &&
                     (!show_disparity_map)) {
                 cvShowImage(right_image_title.c_str(), r);
             }
             //printf("poza ......\n");
         }

         cvCvtColor(l, lg, CV_BGR2GRAY);
         cvCvtColor(r, rg, CV_BGR2GRAY);

         //for( i = j = 0; i < nimages; i++ )
         if(i < nimages)
         {
             for( k = 0; k < 2; k++ )
             {
                 //const string& filename = imagelist[i*2+k];

                 Mat img;

                 if(k == 0)
                 {
                     Mat img1(lg);
                     img = img1;
                 }
                 else if(k == 1)
                 {
                     Mat img1(rg);
                     img = img1;
                 }

                 if(img.empty())
                 {
                     break;
                 }
                 if( imageSize == Size() )
                 {
                     imageSize = img.size();
                 }
                 else if( img.size() != imageSize )
                 {
                     //cout << "The image " << filename << " has the size different from the first image size. Skipping the pair\n";
                     break;
                 }
                 bool found = false;
                 vector<Point2f>& corners = imagePoints[k][j];
                 for( int scale = 1; scale <= maxScale; scale++ )
                 {
                     Mat timg;
                     if( scale == 1 )
                         timg = img;
                     else
                         resize(img, timg, Size(), scale, scale);

                     found = findChessboardCorners(timg, boardSize, corners,
                         CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE);

                     if( found )
                     {
                         if( scale > 1 )
                         {
                             Mat cornersMat(corners);
                             cornersMat *= 1./scale;
                         }
                         break;
                     }
                 }
                 if( displayCorners )
                 {
                     Mat cimg, cimg1;
                     cvtColor(img, cimg, COLOR_GRAY2BGR);
                     drawChessboardCorners(cimg, boardSize, corners, found);
                     double sf = 320./MAX(img.rows, img.cols);
                     resize(cimg, cimg1, Size(), sf, sf);
                     imshow("corners", cimg1);
                     char c = (char)waitKey(500);
                     if( c == 27 || c == 'q' || c == 'Q' ) //Allow ESC to quit
                         exit(-1);
                 }
                 else
                     putchar('.');

                 if( !found )
                 {
                     break;
                 }

                 cornerSubPix(img, corners, Size(11,11), Size(-1,-1),
                              TermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS,
                                           30, 0.01));
             }

             if( k == 2 )
             {
                 goodImageList.push_back(lg);
                 goodImageList.push_back(rg);
                 sleep(1);
                 j++;
                 i++;
                 printf("good images .......... (%d)\n", j);
             }
         }

         else
         {


//         if(vision->getCalibrationStarted())
//         {
//            int result = vision->calibrationAddSample(lg,rg);

//            if(RESULT_OK == result)
//            {
//             printf("+OK (%d)\n", vision->getSampleCount());
//             if(vision->getSampleCount() >= 50)
//             {

//                 vision->calibrationEnd();
//                 sleep(2);
//                 if(RESULT_OK == vision->calibrationSave("/home/imocanu/calibrate.dat"))
//                 {
//                     printf("+OK Saving calibration file ........\n");
//                 }
//                 else
//                 {
//                     printf("-FAIL Saving calibration file\n");
//                 }

//                 printf("Calibration Done !\n");
//             }
//            }
//            else
//            {
//             //printf("-FAIL Try a different position. Chessboard should be visible on both cameras.\n");
//            }
//            usleep(5000);
//         }

//         if(vision->getCalibrationDone())
//         {
//             CvSize imageSize = vision->getImageSize();
//             if(!imageRectifiedPair) imageRectifiedPair = cvCreateMat(hh, ww*2,CV_8UC3 );

//             vision->stereoProcess(lg, rg, l, r);

//             CvMat part;
//             cvGetCols( imageRectifiedPair, &part, 0, imageSize.width );
//             cvCvtColor( vision->imagesRectified[0], &part, CV_GRAY2BGR );
//             cvGetCols( imageRectifiedPair, &part, imageSize.width,imageSize.width*2 );
//             cvCvtColor( vision->imagesRectified[1], &part, CV_GRAY2BGR );
//             for(int j = 0; j < imageSize.height; j += 16 )
//             {
//                 cvLine( imageRectifiedPair, cvPoint(0,j),cvPoint(imageSize.width*2,j),CV_RGB((j%3)?0:255,((j+1)%3)?0:255,((j+2)%3)?0:255));
//             }

//             cvShowImage("rectified", imageRectifiedPair );
//             cvShowImage("depthNormalized", vision->imageDepthNormalized );
//         }


//             * Fundamental Matrix F
//             * Camera Matrices left/right CM1 CM2
//             * Distortion Coefficients for Camera left/right DIST1 DIST2
//             * Rotation Matrix from cvStereoCalibrate R
//             * translation vector from cvStereoCalibrate T
//             * Reprojection Matrix Q from cvStereoRectify
//             * rectified rotation matrix for left camera R1
//             * rectified rotation matrix for right camera R2
//             * projection equation matrix for left image P1
//             * projection equation matrix for right image P1
//             * undistorted pointlist for left image POINTS1
//             * corresponding undistorted pointlist for right image POINTS2

     if(finalCalibrated)
     {
         printf("====> %d pairs have been successfully detected.\n", j);
         nimages = j;

         imagePoints[0].resize(nimages);
         imagePoints[1].resize(nimages);
         objectPoints.resize(nimages);

         for( i = 0; i < nimages; i++ )
         {
             for( j = 0; j < boardSize.height; j++ )
                 for( k = 0; k < boardSize.width; k++ )
                     objectPoints[i].push_back(Point3f(j*squareSize, k*squareSize, 0));
         }

         cout << "Running stereo calibration ...\n";

         Mat cameraMatrix[2], distCoeffs[2];
         cameraMatrix[0] = Mat::eye(3, 3, CV_64F);
         cameraMatrix[1] = Mat::eye(3, 3, CV_64F);
         Mat R, T, E, F;
         Mat iSize(lg);
         imageSize = iSize.size();

         double rms = stereoCalibrate(objectPoints, imagePoints[0], imagePoints[1],
                         cameraMatrix[0], distCoeffs[0],
                         cameraMatrix[1], distCoeffs[1],
                         imageSize, R, T, E, F,
                         TermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 100, 1e-5),
                         CV_CALIB_FIX_ASPECT_RATIO +
                         CV_CALIB_ZERO_TANGENT_DIST +
                         CV_CALIB_SAME_FOCAL_LENGTH +
                         CV_CALIB_RATIONAL_MODEL +
                         CV_CALIB_FIX_K3 + CV_CALIB_FIX_K4 + CV_CALIB_FIX_K5);
         cout << "done with RMS error=" << rms << endl;

     // CALIBRATION QUALITY CHECK
     // because the output fundamental matrix implicitly
     // includes all the output information,
     // we can check the quality of calibration using the
     // epipolar geometry constraint: m2^t*F*m1=0
         double err = 0;
         int npoints = 0;
         vector<Vec3f> lines[2];
         for( i = 0; i < nimages; i++ )
         {
             int npt = (int)imagePoints[0][i].size();
             Mat imgpt[2];
             for( k = 0; k < 2; k++ )
             {
                 imgpt[k] = Mat(imagePoints[k][i]);
                 undistortPoints(imgpt[k], imgpt[k], cameraMatrix[k], distCoeffs[k], Mat(), cameraMatrix[k]);
                 computeCorrespondEpilines(imgpt[k], k+1, F, lines[k]);
             }
             for( j = 0; j < npt; j++ )
             {
                 double errij = fabs(imagePoints[0][i][j].x*lines[1][j][0] +
                                     imagePoints[0][i][j].y*lines[1][j][1] + lines[1][j][2]) +
                                fabs(imagePoints[1][i][j].x*lines[0][j][0] +
                                     imagePoints[1][i][j].y*lines[0][j][1] + lines[0][j][2]);
                 err += errij;
             }
             npoints += npt;
         }
         cout << "average reprojection err = " <<  err/npoints << endl;





         // save intrinsic parameters
         FileStorage fs("intrinsics.yml", CV_STORAGE_WRITE);
         if( fs.isOpened() )
         {
             fs << "M1" << cameraMatrix[0] << "D1" << distCoeffs[0] <<
                 "M2" << cameraMatrix[1] << "D2" << distCoeffs[1];
             fs.release();
         }
         else
             cout << "Error: can not save the intrinsic parameters\n";

         Mat R1, R2, P1, P2, Q;
         //Rect validRoi[2];

         stereoRectify(cameraMatrix[0], distCoeffs[0],
                       cameraMatrix[1], distCoeffs[1],
                       imageSize, R, T, R1, R2, P1, P2, Q,
                       CALIB_ZERO_DISPARITY, 1, imageSize, &validRoi[0], &validRoi[1]);

         fs.open("extrinsics.yml", CV_STORAGE_WRITE);
         if( fs.isOpened() )
         {
             fs << "R" << R << "T" << T << "R1" << R1 << "R2" << R2 << "P1" << P1 << "P2" << P2 << "Q" << Q;
             fs.release();
         }
         else
             cout << "Error: can not save the intrinsic parameters\n";

         // OpenCV can handle left-right
         // or up-down camera arrangements
         isVerticalStereo = fabs(P2.at<double>(1, 3)) > fabs(P2.at<double>(0, 3));

     // COMPUTE AND DISPLAY RECTIFICATION
         if( !showRectified )
         {
             exit(1);
         }

     //    Mat rmap[2][2];
     // IF BY CALIBRATED (BOUGUET'S METHOD)
         if( useCalibrated )
         {
             // we already computed everything
         }
     // OR ELSE HARTLEY'S METHOD
         else
      // use intrinsic parameters of each camera, but
      // compute the rectification transformation directly
      // from the fundamental matrix
         {
             vector<Point2f> allimgpt[2];
             for( k = 0; k < 2; k++ )
             {
                 for( i = 0; i < nimages; i++ )
                     std::copy(imagePoints[k][i].begin(), imagePoints[k][i].end(), back_inserter(allimgpt[k]));
             }
             F = findFundamentalMat(Mat(allimgpt[0]), Mat(allimgpt[1]), FM_8POINT, 0, 0);
             Mat H1, H2;
             stereoRectifyUncalibrated(Mat(allimgpt[0]), Mat(allimgpt[1]), F, imageSize, H1, H2, 3);

             R1 = cameraMatrix[0].inv()*H1*cameraMatrix[0];
             R2 = cameraMatrix[1].inv()*H2*cameraMatrix[1];
             P1 = cameraMatrix[0];
             P2 = cameraMatrix[1];
         }

         //Precompute maps for cv::remap()
         initUndistortRectifyMap(cameraMatrix[0], distCoeffs[0], R1, P1, imageSize, CV_16SC2, rmap[0][0], rmap[0][1]);
         initUndistortRectifyMap(cameraMatrix[1], distCoeffs[1], R2, P2, imageSize, CV_16SC2, rmap[1][0], rmap[1][1]);

         finalCalibrated = false;
    }

         Mat canvas;
         double sf;
         int w, h;
         if( !isVerticalStereo )
         {
             sf = 320./MAX(imageSize.width, imageSize.height);
             w = cvRound(imageSize.width*sf);
             h = cvRound(imageSize.height*sf);
             canvas.create(h, w*2, CV_8UC3);
         }
         else
         {
             sf = 240./MAX(imageSize.width, imageSize.height);
             w = cvRound(imageSize.width*sf);
             h = cvRound(imageSize.height*sf);
             canvas.create(h*2, w, CV_8UC3);
         }


             for( k = 0; k < 2; k++ )
             {
                 Mat rimg, cimg;

                 Mat img;
                 if(k == 0)
                 {
                     Mat img1(lg);
                     img = img1;
                     cvShowImage("lg", lg);

                 }
                 else
                 {
                     Mat img1(rg);
                     img = img1;
                     cvShowImage("rg", rg);

                 }

                 remap(img, rimg, rmap[k][0], rmap[k][1], CV_INTER_LINEAR);
                 cvtColor(rimg, cimg, COLOR_GRAY2BGR);
                 Mat canvasPart = !isVerticalStereo ? canvas(Rect(w*k, 0, w, h)) : canvas(Rect(0, h*k, w, h));
                 resize(cimg, canvasPart, canvasPart.size(), 0, 0, CV_INTER_AREA);
//                 if( useCalibrated )
//                 {
                     Rect vroi(cvRound(validRoi[k].x*sf), cvRound(validRoi[k].y*sf),
                               cvRound(validRoi[k].width*sf), cvRound(validRoi[k].height*sf));
                     rectangle(canvasPart, vroi, Scalar(0,0,255), 3, 8);
//                 }
             }

             if( !isVerticalStereo )
                 for( j = 0; j < canvas.rows; j += 16 )
                     line(canvas, Point(0, j), Point(canvas.cols, j), Scalar(0, 255, 0), 1, 8);
             else
                 for( j = 0; j < canvas.cols; j += 16 )
                     line(canvas, Point(j, 0), Point(j, canvas.rows), Scalar(0, 255, 0), 1, 8);
             imshow("rectified", canvas);

}


         skip_frames--;
         if (skip_frames < 0) skip_frames = 0;
         int wait = cvWaitKey(10) & 255;

         if( wait == 27 )
         {
             break;
         }
     }

    return 0;
}
