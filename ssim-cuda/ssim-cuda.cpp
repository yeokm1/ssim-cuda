#include <iostream> // for standard I/O
#include <string>   // for strings
#include <iomanip>  // for controlling float print precision
#include <sstream>  // string to number conversion

#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O
#include <opencv2/gpu/gpu.hpp>

#define STATUS_PER_NUM_FRAMES 200

using namespace std;
using namespace cv;

struct BufferMSSIM                                     // Optimized GPU versions
{   // Data allocations are very expensive on GPU. Use a buffer to solve: allocate once reuse later.
	gpu::GpuMat gI1, gI2, gs, t1, t2;

	gpu::GpuMat I1_2, I2_2, I1_I2;
	vector<gpu::GpuMat> vI1, vI2;

	gpu::GpuMat mu1, mu2;
	gpu::GpuMat mu1_2, mu2_2, mu1_mu2;

	gpu::GpuMat sigma1_2, sigma2_2, sigma12;
	gpu::GpuMat t3;

	gpu::GpuMat ssim_map;

	gpu::GpuMat buf;
};

Scalar getMSSIM_GPU_optimized(const Mat& i1, const Mat& i2, BufferMSSIM& b);

int main(int argc, char *argv[])
{

	char c;


	if (argc < 6)
	{
		cout << "Not enough parameters" << endl;
		cout << "Command: ssim reference_video.avi test_video.avi w x y [z]" << endl;
		cout << "Where w is the start frame of reference video and x is the start frame of test video." << endl;
		cout << "y is the ms delay between frames. If delay is 0, video will not be shown on screen but calculation will be a lot faster. A value of 10 is recommended." << endl;
		cout << "[z] (optional) is the number of frames to process. If not specified, app will process to end of any video." << endl;

		return -1;
	}


	const string sourceReference = argv[1], sourceCompareWith = argv[2];


	unsigned long refFramesToWait = strtol(argv[3], NULL, 10);
	unsigned long testFramesToWait = strtol(argv[4], NULL, 10);
	int delayBetweenFrames = strtol(argv[5], NULL, 10);

	unsigned long framesToProcess = -1;

	if (argc >= 7){
		framesToProcess = strtol(argv[6], NULL, 10);
	}

	unsigned long refFrameNum = 0;
	unsigned long testFrameNum = 0;
	unsigned long comparisonFrameNum = 0;          // Frame counter
	unsigned long long redChannelTotal = 0;
	unsigned long long blueChannelTotal = 0;
	unsigned long long greenChannelTotal = 0;

	VideoCapture captRefrnc(sourceReference), captUndTst(sourceCompareWith);

	if (!captRefrnc.isOpened())
	{
		cout << "Could not open reference " << sourceReference << endl;
		return -1;
	}

	if (!captUndTst.isOpened())
	{
		cout << "Could not open case test " << sourceCompareWith << endl;
		return -1;
	}

	Size refS = Size((int)captRefrnc.get(CV_CAP_PROP_FRAME_WIDTH),
		(int)captRefrnc.get(CV_CAP_PROP_FRAME_HEIGHT)),
		uTSi = Size((int)captUndTst.get(CV_CAP_PROP_FRAME_WIDTH),
		(int)captUndTst.get(CV_CAP_PROP_FRAME_HEIGHT));

	if (refS != uTSi)
	{
		cout << "Inputs have different size!!! Closing." << endl;
		return -1;
	}

	string refFilename = argv[1];
	string testFilename = argv[2];

	string refHeader = "Reference: " + refFilename;
	string testHeader = "Test: " + testFilename;


	const char* WIN_RF = refHeader.c_str();
	const char* WIN_UT = testHeader.c_str();

	// Windows
	namedWindow(WIN_RF, CV_WINDOW_AUTOSIZE);
	namedWindow(WIN_UT, CV_WINDOW_AUTOSIZE);
	cvMoveWindow(WIN_RF, 400, 0);         //750,  2 (bernat =0)
	cvMoveWindow(WIN_UT, refS.width, 0);         //1500, 2

	cout << "Reference frame resolution: Width=" << refS.width << "  Height=" << refS.height
		<< " of nr#: " << captRefrnc.get(CV_CAP_PROP_FRAME_COUNT) << endl;


	Mat frameReference, frameUnderTest;
	Scalar mssimV;

	BufferMSSIM bufferMSSIM;

	double time = (double)getTickCount();


	for (;;) //Show the image captured in the window and repeat
	{
		//Skip frames until both streams have synchronised on their starting frames
		if (refFrameNum <= refFramesToWait || testFrameNum <= testFramesToWait)
		{
			if (refFrameNum <= refFramesToWait){
				captRefrnc >> frameReference;
				refFrameNum++;
			}

			if (testFrameNum <= testFramesToWait){
				captUndTst >> frameUnderTest;
				testFrameNum++;
			}



			continue;
		}


		captRefrnc >> frameReference;
		captUndTst >> frameUnderTest;

		if (frameReference.empty() || frameUnderTest.empty())
		{
			cout << "Program ended" << endl;
			break;
		}


		mssimV = getMSSIM_GPU_optimized(frameReference, frameUnderTest, bufferMSSIM);

		++comparisonFrameNum;

		redChannelTotal += (unsigned long long) (mssimV.val[2] * 10000); //To store up to 2 decimal places
		greenChannelTotal += (unsigned long long) (mssimV.val[1] * 10000); //To store up to 2 decimal places
		blueChannelTotal += (unsigned long long)  (mssimV.val[0] * 10000); //To store up to 2 decimal places

		if ((comparisonFrameNum % STATUS_PER_NUM_FRAMES) == 0){
			cout << "Finished " << setfill('0') << setw(6) << comparisonFrameNum << " frames,";

			double redSimilarity = (redChannelTotal / comparisonFrameNum) / 100.0;
			double greenSimilarity =  (greenChannelTotal / comparisonFrameNum) / 100.0;
			double blueSimilarity =  (blueChannelTotal / comparisonFrameNum) / 100.0;

			cout << " Similarity so far: "
				<< " R " << setiosflags(ios::fixed) << setprecision(2) << redSimilarity << "%"
				<< " G " << setiosflags(ios::fixed) << setprecision(2) << greenSimilarity << "%"
				<< " B " << setiosflags(ios::fixed) << setprecision(2) << blueSimilarity << "%";

			cout << endl;

		}
		////////////////////////////////// Show Image /////////////////////////////////////////////
		imshow(WIN_RF, frameReference);
		imshow(WIN_UT, frameUnderTest);

		if(delayBetweenFrames > 0){
			c = (char)cvWaitKey(delayBetweenFrames);
			if (c == 27) break;
		}

		if (framesToProcess >= 0 && comparisonFrameNum >= framesToProcess){
			break;
		}
	}

	time = ((double)getTickCount() - time)/ getTickFrequency();

	double redSimilarity = (redChannelTotal / comparisonFrameNum) / 100.0;
	double greenSimilarity = (greenChannelTotal / comparisonFrameNum) / 100.0;
	double blueSimilarity = (blueChannelTotal / comparisonFrameNum) / 100.0;

	cout << "Processing time: " << time << " seconds." << endl;
	cout << "Ref start frame: " << refFramesToWait << ", Test start frame: " << testFramesToWait << endl;
	cout << "Final Results" << endl;

	cout << "R " << setiosflags(ios::fixed) << setprecision(2) << redSimilarity << "%"
		<< " G " << setiosflags(ios::fixed) << setprecision(2) << greenSimilarity << "%"
		<< " B " << setiosflags(ios::fixed) << setprecision(2) << blueSimilarity << "%";

	cout << endl;

	double average = (redSimilarity + greenSimilarity + blueSimilarity) / 3;

	cout << "Similarity = " << average << "%" << endl;



	return 0;
}

Scalar getMSSIM_GPU_optimized(const Mat& i1, const Mat& i2, BufferMSSIM& b)
{
	const float C1 = 6.5025f, C2 = 58.5225f;
	/***************************** INITS **********************************/

	b.gI1.upload(i1);
	b.gI2.upload(i2);

	gpu::Stream stream;

	stream.enqueueConvert(b.gI1, b.t1, CV_32F);
	stream.enqueueConvert(b.gI2, b.t2, CV_32F);

	gpu::split(b.t1, b.vI1, stream);
	gpu::split(b.t2, b.vI2, stream);
	Scalar mssim;

	gpu::GpuMat buf;

	for (int i = 0; i < b.gI1.channels(); ++i)
	{
		gpu::multiply(b.vI2[i], b.vI2[i], b.I2_2, stream);        // I2^2
		gpu::multiply(b.vI1[i], b.vI1[i], b.I1_2, stream);        // I1^2
		gpu::multiply(b.vI1[i], b.vI2[i], b.I1_I2, stream);       // I1 * I2

		gpu::GaussianBlur(b.vI1[i], b.mu1, Size(11, 11), buf, 1.5, 0, BORDER_DEFAULT, -1, stream);
		gpu::GaussianBlur(b.vI2[i], b.mu2, Size(11, 11), buf, 1.5, 0, BORDER_DEFAULT, -1, stream);

		gpu::multiply(b.mu1, b.mu1, b.mu1_2, stream);
		gpu::multiply(b.mu2, b.mu2, b.mu2_2, stream);
		gpu::multiply(b.mu1, b.mu2, b.mu1_mu2, stream);

		gpu::GaussianBlur(b.I1_2, b.sigma1_2, Size(11, 11), buf, 1.5, 0, BORDER_DEFAULT, -1, stream);
		gpu::subtract(b.sigma1_2, b.mu1_2, b.sigma1_2, gpu::GpuMat(), -1, stream);
		//b.sigma1_2 -= b.mu1_2;  - This would result in an extra data transfer operation

		gpu::GaussianBlur(b.I2_2, b.sigma2_2, Size(11, 11), buf, 1.5, 0, BORDER_DEFAULT, -1, stream);
		gpu::subtract(b.sigma2_2, b.mu2_2, b.sigma2_2, gpu::GpuMat(), -1, stream);
		//b.sigma2_2 -= b.mu2_2;

		gpu::GaussianBlur(b.I1_I2, b.sigma12, Size(11, 11), buf, 1.5, 0, BORDER_DEFAULT, -1, stream);
		gpu::subtract(b.sigma12, b.mu1_mu2, b.sigma12, gpu::GpuMat(), -1, stream);
		//b.sigma12 -= b.mu1_mu2;

		//here too it would be an extra data transfer due to call of operator*(Scalar, Mat)
		gpu::multiply(b.mu1_mu2, 2, b.t1, 1, -1, stream); //b.t1 = 2 * b.mu1_mu2 + C1;
		gpu::add(b.t1, C1, b.t1, gpu::GpuMat(), -1, stream);
		gpu::multiply(b.sigma12, 2, b.t2, 1, -1, stream); //b.t2 = 2 * b.sigma12 + C2;
		gpu::add(b.t2, C2, b.t2, gpu::GpuMat(), -12, stream);

		gpu::multiply(b.t1, b.t2, b.t3, 1, -1, stream);     // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))

		gpu::add(b.mu1_2, b.mu2_2, b.t1, gpu::GpuMat(), -1, stream);
		gpu::add(b.t1, C1, b.t1, gpu::GpuMat(), -1, stream);

		gpu::add(b.sigma1_2, b.sigma2_2, b.t2, gpu::GpuMat(), -1, stream);
		gpu::add(b.t2, C2, b.t2, gpu::GpuMat(), -1, stream);


		gpu::multiply(b.t1, b.t2, b.t1, 1, -1, stream);     // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))
		gpu::divide(b.t3, b.t1, b.ssim_map, 1, -1, stream);      // ssim_map =  t3./t1;

		stream.waitForCompletion();

		Scalar s = gpu::sum(b.ssim_map, b.buf);
		mssim.val[i] = s.val[0] / (b.ssim_map.rows * b.ssim_map.cols);

	}
	return mssim;
}
