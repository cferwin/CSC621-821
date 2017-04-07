#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkThresholdImageFilter.h"
#include "QuickView.h"


int main(int argc, char *argv[]) {
	// Check argument count
	if (argc != 5) {
		return EXIT_FAILURE;
	}

	// Parse command line argumentsa
	std::string inputFilename = argv[1];
	int lowerThreshold = 10;
	int upperThreshold = 50;
	double variance = 2.0;
	if (argc > 2)
	{
		variance = atof(argv[2]);
		lowerThreshold = atoi(argv[3]);
		upperThreshold = atoi(argv[4]);
	}

	// Setup types
	typedef itk::Image< unsigned char, 2 > UnsignedCharImageType;
	typedef itk::Image< float, 2 >         FloatImageType;

	typedef itk::ImageFileReader< UnsignedCharImageType >  readerType;

	typedef itk::DiscreteGaussianImageFilter<UnsignedCharImageType, FloatImageType >  filterType;

	// Create and setup a reader
	readerType::Pointer reader = readerType::New();
	reader->SetFileName(argv[1]);

	// Create and setup a Gaussian filter
	filterType::Pointer gaussianFilter = filterType::New();
	gaussianFilter->SetInput(reader->GetOutput());
	gaussianFilter->SetVariance(variance);

	// Threshold 
	typedef itk::ThresholdImageFilter <FloatImageType> ThresholdImageFilterType;
	ThresholdImageFilterType::Pointer thresholdFilter = ThresholdImageFilterType::New();
	thresholdFilter->SetInput(gaussianFilter->GetOutput());
	thresholdFilter->ThresholdOutside(lowerThreshold, upperThreshold);
	thresholdFilter->SetOutsideValue(0);

	// Display
	QuickView viewer;
	viewer.AddImage<UnsignedCharImageType>(reader->GetOutput());
	viewer.AddImage<FloatImageType>(thresholdFilter->GetOutput());
	viewer.Visualize();
	
	return EXIT_SUCCESS;
	

}
