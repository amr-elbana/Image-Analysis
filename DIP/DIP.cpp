#include "stdafx.h"
#include <iostream>
#include <vector> 
#include <cmath>
#include <opencv2/opencv.hpp>
#include<random>
using namespace std;
using namespace cv; 
int thresholdVal = 128;
int k = 3;
/*
vector<vector<double>> imageDataToData(const Mat& src) {
    vector<vector<double>> data;
    for (int y = 0; y < src.rows; ++y) {
        for (int x = 0; x < src.cols; ++x) {
            data.push_back({ double(x), double(y), double(src.at<uchar>(y, x)) });
        }
    }
    return data;
}
// Function to convert data format to image
Mat dataToImage(const vector<vector<double>>& data, const Size& imageSize) {
    Mat image(imageSize, CV_8UC1, Scalar(0));
    for (const auto& point : data) {
        int x = static_cast<int>(point[0]);
        int y = static_cast<int>(point[1]);
        uchar value = static_cast<uchar>(point[2]);
        image.at<uchar>(y, x) = value;
    }
    return image;
}
double euclideanDistance(const vector<double>& point1, const vector<double>& point2) {
    double distance = 0.0;
    for (size_t i = 0; i < point1.size(); ++i) {
        distance += pow(point1[i] - point2[i], 2);
    }
    return sqrt(distance);
}
vector<vector<double>> initializeCentroids(const vector<vector<double>>& data) {
    vector<vector<double>> centroids;
    srand(time(0));
    for (int i = 0; i < k; ++i) {
        srand(rand());
        int randomIdx = rand() % data.size();
        centroids.push_back(data[randomIdx]);
    }
    return centroids;
}
vector<int> assignPointsToCentroids(const vector<vector<double>>& data, const vector<vector<double>>& centroids) {
    vector<int> assignments(data.size());
    for (size_t i = 0; i < data.size(); ++i) {
        double minDistance = numeric_limits<double>::max();
        int closestCentroidIdx = -1;
        for (size_t j = 0; j < centroids.size(); ++j) {
            double distance = euclideanDistance(data[i], centroids[j]);
            if (distance < minDistance) {
                minDistance = distance;
                closestCentroidIdx = j;
            }
        }
        assignments[i] = closestCentroidIdx;
    }
    return assignments;
}

// Function to update centroids based on assigned points
vector<vector<double>> updateCentroids(const vector<vector<double>>& data, const vector<int>& assignments) {
    vector<vector<double>> newCentroids(k, vector<double>(data[0].size(), 0.0));
    vector<int> counts(k, 0);
    for (size_t i = 0; i < data.size(); ++i) {
        int centroidIdx = assignments[i];
        for (size_t j = 0; j < data[i].size(); ++j) {
            newCentroids[centroidIdx][j] += data[i][j];
        }
        counts[centroidIdx]++;
    }
    for (int i = 0; i < k; ++i) {
        for (size_t j = 0; j < data[0].size(); ++j) {
            newCentroids[i][j] /= counts[i];
        }
    }
    return newCentroids;
}

// Function to check if centroids have converged
bool centroidsConverged(const vector<vector<double>>& oldCentroids, const vector<vector<double>>& newCentroids, double threshold) {
    for (size_t i = 0; i < oldCentroids.size(); ++i) {
        if (euclideanDistance(oldCentroids[i], newCentroids[i]) > threshold) {
            return false;
        }
    }
    return true;
}
// Thresholding
*/
void thresholdImage(const Mat& src, int thresholdVal, Mat& dstShapes, Mat& dstIndexed) {
    for (int y = 0; y < src.rows; y++) {
        for (int x = 0; x < src.cols; x++) {
            if (src.at<uchar>(y, x) > thresholdVal) {
                dstShapes.at<uchar>(y, x) = 255;
            }
            else {
                dstShapes.at<uchar>(y, x) = 0;
            }
            dstIndexed.at<uchar>(y, x) = 0;
        }
    }
}

vector<array<double, 4>> shapeV;
vector<array<double, 4>> temp;

// indexing
class indexImage{
public:
    
    int x = 0, y = 0;
    int circumference = 0;
    int currentIndex;
    double area = 0;
    vector<vector<bool>> visited;
    vector<vector<bool>> visited_2;
    long double m00 = 0, m01 = 0, m10 = 0, Xt = 0, Yt = 0, u00 = 0, u20 = 0, u02 = 0, u11 = 0, F1 = 0, F2 = 0;
    int num_rows = 5;
    
    indexImage(int xCoordinates , int yCoordinates) {
        currentIndex += 10;
        visited = vector<vector<bool>>(xCoordinates, vector<bool>(yCoordinates, false));
        visited_2 = vector<vector<bool>>(xCoordinates, vector<bool>(yCoordinates, false));
    }

    void floodFillRecursive(const Mat& binaryImage, Mat& indexedImage, int x, int y, int& index) {
    if (x < 0 || x >= binaryImage.cols || y < 0 || y >= binaryImage.rows || binaryImage.at<uchar>(y, x) < thresholdVal)
        return;

    if (visited[y][x]) {
        return;
    }

    visited[y][x] = true;
    indexedImage.at<uchar>(y, x) = index;

    // Check if the current pixel is on the object's boundary
    bool isBoundaryPixel = false;
    if (binaryImage.at<uchar>(y - 1, x) < thresholdVal ||
        binaryImage.at<uchar>(y + 1, x) < thresholdVal ||
        binaryImage.at<uchar>(y, x - 1) < thresholdVal ||
        binaryImage.at<uchar>(y, x + 1) < thresholdVal) {
        isBoundaryPixel = true;
    }

    area++;
    m00 += 1;
    m10 += x;
    m01 += y;

    if (isBoundaryPixel) {
        circumference++;
    }
    floodFillRecursive(binaryImage, indexedImage, x + 1, y, index); // Right
    floodFillRecursive(binaryImage, indexedImage, x - 1, y, index); // Left
    floodFillRecursive(binaryImage, indexedImage, x, y + 1, index); // Down
    floodFillRecursive(binaryImage, indexedImage, x, y - 1, index); // Up
    floodFillRecursive(binaryImage, indexedImage, x + 1, y + 1, index); // Right-Down
    floodFillRecursive(binaryImage, indexedImage, x - 1, y + 1, index); // Left-Down
    floodFillRecursive(binaryImage, indexedImage, x + 1, y - 1, index); // Right-Up
    floodFillRecursive(binaryImage, indexedImage, x - 1, y - 1, index); // Left-Up
}
    void floodFillRecursiveU(const Mat& binaryImage, Mat& indexedImage, int x, int y, int xt, int yt, int& index) {
    if (x < 0 || x >= binaryImage.cols || y < 0 || y >= binaryImage.rows || binaryImage.at<uchar>(y, x) < thresholdVal)
        return;

    if (visited_2[y][x]) {
        return;
    }
    int Xd = x - Xt;
    int Yd = y - Yt;
    visited_2[y][x] = true;
    indexedImage.at<uchar>(y, x) = index;
    u00 += 1;
    u20 += pow(Xd,2);
    u02 += pow(Yd,2);
    u11 += Yd * Xd;

    floodFillRecursiveU(binaryImage, indexedImage, x + 1, y,xt,yt ,index); // Right
    floodFillRecursiveU(binaryImage, indexedImage, x - 1, y ,xt,yt, index); // Left
    floodFillRecursiveU(binaryImage, indexedImage, x, y + 1 ,xt,yt, index); // Down
    floodFillRecursiveU(binaryImage, indexedImage, x, y - 1 ,xt,yt ,index); // Up
    floodFillRecursiveU(binaryImage, indexedImage, x + 1, y + 1,xt,yt,index); // Right-Down
    floodFillRecursiveU(binaryImage, indexedImage, x - 1, y + 1,xt,yt, index); // Left-Down
    floodFillRecursiveU(binaryImage, indexedImage, x + 1, y - 1,xt,yt, index); // Right-Up
    floodFillRecursiveU(binaryImage, indexedImage, x - 1, y - 1,xt,yt, index); // Left-Up
}
    void indexing(const Mat& shapes_8uc1_img, Mat& indexedImage) {
    for (int y = 0; y < shapes_8uc1_img.rows; ++y) {
        for (int x = 0; x < shapes_8uc1_img.cols; ++x) {
            if (shapes_8uc1_img.at<uchar>(y, x) > thresholdVal && !visited[y][x]) {
                floodFillRecursive(shapes_8uc1_img, indexedImage, x, y, currentIndex);
                currentIndex += 10;
                Xt = m10 / m00;
                Yt = m01 / m00;
                floodFillRecursiveU(shapes_8uc1_img, indexedImage, x, y,Xt, Yt ,currentIndex);
                long double Umax = 0.5 * (u20 + u02) + 0.5 * sqrt((4ll * pow(u11, 2ll)) + (pow(u20 - u02, 2ll)));
                long double Umin = 0.5 * (u20 + u02) - ( 0.5 * sqrt((4ll * pow(u11, 2ll)) + (pow(u20 - u02, 2ll))));
                F1 = (pow(circumference, 2)) / (100 * area);
                F2 = Umin/Umax;
                array<double, 4> shapeA = { F1, F2, Xt, Yt };
                shapeV.push_back(shapeA);
                print();
                zero();
            }
        }
    }
    }
    void print() {
        cout << "|Area (u00): " << area;
        cout << "|Circumference: " << circumference;
        cout << "|m01: " << m01;
        cout << "|m10: " << m10;
        cout << "|Xt : " << Xt << "|Yt : " << Yt;
        cout << "|F1 : " << F1;
        cout << "|F2 : " << F2 << endl;
    }
    void zero() {
        area = m00 = m00 = m01 = m10 = circumference = Xt = Yt = u00 = u20 = u02 = u11 = 0;
    }
 };

struct ClassF1 {
    double squareSum;
    double starSum;
    double recSum;
};

struct ClassF2 {
    double squareSum;
    double starSum;
    double recSum;
};

vector <vector<double long>> f1Class{
// Square
    { 0.111052, 0.155572 ,  0.150963, 0.153282 },
// Stars    
    {  0.708833, 0.761471, 0.725034, 0.707275},
// Rectangle    
    {0.166867 ,0.219282 ,0.219144 ,0.219657}
};

vector <vector<double long>> f2Class{
// Square
    { 0.929567 , 0.965502 , 0.958525 ,0.949481 },
// Stars
    {0.918333 , 0.967087 ,0.922176 ,0.900752 },
// Rectangle
    { 0.0803754 , 0.0662656 ,  0.0636217,  0.0663265}
};
vector<vector<double>> initializeCentroids(const vector<vector<double>>& data) {
    vector<vector<double>> centroids;
    srand(time(0));
    for (int i = 0; i < k; ++i) {
        srand(rand());
        int randomIdx = rand() % data.size();
        centroids.push_back(data[randomIdx]);
    }
    return centroids;
}

int main() {
    Mat src_8uc3_img = imread("images/train.png", IMREAD_GRAYSCALE);
    Mat src2_8uc3_img = imread("images/test02.png", IMREAD_GRAYSCALE);

    Mat shapes_8uc1_img(158, 264, CV_8UC1);
    Mat shapes2_8uc1_img(src2_8uc3_img.rows, src2_8uc3_img.cols, CV_8UC1);

    Mat indexedImage(158, 264, CV_8UC1);
    Mat indexedImage2(src2_8uc3_img.rows, src2_8uc3_img.cols, CV_8UC1);


    struct ClassF1 CF1 {};
    struct ClassF2 CF2 {};

    if (src_8uc3_img.empty() || src2_8uc3_img.empty()) {
        cout << "Unable to read input file" << endl;
        return -1;
    }
    thresholdImage(src_8uc3_img, thresholdVal, shapes_8uc1_img, indexedImage);
    thresholdImage(src2_8uc3_img, thresholdVal, shapes2_8uc1_img, indexedImage2);

    indexImage train(158, 264);
    train.indexing(shapes_8uc1_img, indexedImage);



    // Calulating the sum of each class 

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            switch (i) {
            case 0:
                CF1.squareSum += f1Class[i][j];
                break;
            case 1:
                CF1.starSum += f1Class[i][j];

                break;
            case 2:
                CF1.recSum += f1Class[i][j];
                break;

            }
        }
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            switch (i) {
            case 0:
                CF2.squareSum += f2Class[i][j];
                break;
            case 1:
                CF2.starSum += f2Class[i][j];

                break;
            case 2:
                CF2.recSum += f2Class[i][j];
                break;
            }
        }
    }
    double sqmeanF1 = CF1.squareSum / 4;
    double sqmeanF2 = CF2.squareSum / 4;

    double starmeanF1 = CF1.starSum / 4;
    double starmeanF2 = CF2.starSum / 4;

    double recmeanF1 = CF1.recSum / 4;
    double recmeanF2 = CF2.recSum / 4;


    vector<tuple<String, double long, double long>> ethalonTuple{
        // Square
        make_tuple("square",sqmeanF1, sqmeanF2),
        // Stars
        make_tuple("Stars",starmeanF1, starmeanF2),
        // Rectangle
        make_tuple("Rectangle",recmeanF1, recmeanF2),
    };
    vector<tuple<String, double long, double long>> unEthalonTuple{
        make_tuple("unkown", 0.147994, 0.911442),
        make_tuple("unkown",  0.140105, 0.162934),
        make_tuple("unkown", 0.181424, 0.157038),
        make_tuple("unkown", 0.132987, 0.895508),
        make_tuple("unkown",  0.599484,  0.8053),
    };
    vector<std::vector<double>> unKown = {
        {41.0498  , 43.5107},
        { 198.473  ,81.194},
        { 80.1806  ,162.093},
        { 240.623  ,200.398},
        { 111.056  ,250.282}
    };

    vector<std::vector<double>> F_unKown = {
        {0.147994, 0.911442},
        {0.181424, 0.157038},
        {0.140105, 0.162934 },
        {0.132987, 0.895508},
        {0.599484,  0.8053}
    };

    cout << "-----------------------------------------------------------------------------------------------------------" << endl << endl;

    cout << "Square Mean: " << sqmeanF1 << " " << sqmeanF2 << endl;
    cout << "Star Mean: " << starmeanF1 << " " << starmeanF2 << endl;
    cout << "Rectangle Mean: " << recmeanF1 << " " << recmeanF2 << endl;
    /*Excersise 3*/
    double minDistance = DBL_MAX;
    for (int i = 0; i < unEthalonTuple.size();i++) {
        for (int j = 0;j < ethalonTuple.size();j++) {
            double distance = sqrt(pow(get<1>(unEthalonTuple[i]) - get<1>(ethalonTuple[j]), 2) + pow(get<2>(unEthalonTuple[i]) - get<2>(ethalonTuple[j]), 2));

            if (distance < minDistance) {
                minDistance = distance;
                unEthalonTuple[i]._Myfirst = ethalonTuple[j]._Myfirst;

            }

        }
        cout << get<0>(unEthalonTuple[i]) << endl;
        minDistance = DBL_MAX;
    }

    /*
    std::cout << "Vector elements:" << std::endl;
    for (const auto& array : shapeV) {
        for (double value : array) {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }
    */
    /******* K-means ********/
    int k = 3;
    std::vector<std::pair<long double, long double>> Centeroids;
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(0, 100000);


    for (int i = 0; i < k; i++) {
        if (i < shapeV.size()) { // Check if i is within the bounds of shapeV
            array<double, 4> sh = shapeV[dist6(rng) % shapeV.size()];
            Centeroids.push_back({ sh[0] , sh[1] }); // Access sh elements using []
        }
    }
    for (int l = 0; l < 100; l++) {

        std::vector<std::vector<array<double, 4>>> Classes(k);

        for (int i = 0; i < shapeV.size(); i++) {
            long double minDist = DBL_MAX;
            int cls = 0;
            for (int j = 0; j < k; j++) {

                long double dist = sqrt(powl(shapeV[i][0] - (Centeroids[j].first), 2) + powl(shapeV[i][1] - (Centeroids[j].second), 2));

                if (dist < minDist) {
                    minDist = dist;
                    cls = j;
                }
            }
            Classes[cls].push_back(shapeV[i]);

        }
        //Calculate Mean
        for (int j = 0; j < k; j++) {
            long double sumX = 0;
            long double sumY = 0;

            for (int i = 0; i < Classes[j].size(); i++) {  // Use j as the index
                sumX += Classes[j][i][0];
                sumY += Classes[j][i][1];
            }

            if (!Classes[j].empty()) {  // Avoid division by zero
                sumX /= Classes[j].size();
                sumY /= Classes[j].size();
                Centeroids[j] = { sumX, sumY };  // Use j as the index to update the centroid
            }
        }
    }
    double font_size = 0.5;
    cv::Scalar font_Color(128, 128, 128);

    for (int i = 0; i < shapeV.size(); i++) {

        cv::Point text_position(shapeV[i][2], shapeV[i][3]);
        std::string s;
        s += std::to_string(i + 1);
        putText(indexedImage, s, text_position, cv::HersheyFonts::FONT_HERSHEY_COMPLEX, font_size, font_Color, 1);
    }

    // first Image
    imshow("Original", src_8uc3_img);
    imshow("Threshold", shapes_8uc1_img);
    imshow("indexed", indexedImage);
    
    shapeV.clear();
    indexImage train2(shapes2_8uc1_img.rows, indexedImage2.cols);
    train2.indexing(shapes2_8uc1_img, indexedImage2);
    
    
    for (int i = 0; i < shapeV.size(); i++) {
        int index = 0;
        long double minDist = DBL_MAX;
        for (int j = 0; j < Centeroids.size(); j++) {
            long double dist = sqrt(powl(shapeV[i][0] - Centeroids[j].first, 2) + powl(shapeV[i][1] - Centeroids[j].second, 2));

            if (dist < minDist) {
                minDist = dist;
                index = (j + 1);

            }
        }
        std::cout << index << std::endl;

        cv::Point text_position(shapeV[i][2], shapeV[i][3]);
        std::string s = to_string(index);

        putText(indexedImage2, s, text_position, cv::HersheyFonts::FONT_HERSHEY_COMPLEX, font_size, font_Color, 1);

        }

        //-----------K-Means----------//
    

        // Second image
        imshow("Original2", src2_8uc3_img);
        imshow("Threshold2", shapes2_8uc1_img);
        imshow("indexed2", indexedImage2);


        waitKey(0);
    
        return 0;
}
