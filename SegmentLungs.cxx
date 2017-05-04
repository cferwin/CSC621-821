#include "CompositeLung.cxx"
#include "CompositeLung.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkThresholdImageFilter.h"
#include "QuickView.h"


int main(int argc, char *argv[]) {
	// Parse command line argumentsa
	std::string inputFilename = argv[1];
	int threshold = 50;
	double variance = 2.0;
	int invert = 255;
	if (argc > 2)
	{
		variance = atof(argv[2]);
		threshold = atoi(argv[3]);
	}

	// Setup types
	typedef itk::Image< unsigned char, 2 > UnsignedCharImageType;
	typedef itk::Image< float, 2 >         FloatImageType;

	typedef itk::ImageFileReader< FloatImageType >  readerType;

	typedef itk::DiscreteGaussianImageFilter<FloatImageType, FloatImageType >  filterType;

	// Create and setup a reader
	readerType::Pointer reader = readerType::New();
	reader->SetFileName(argv[1]);

	CompositeLung<FloatImageType, FloatImageType>::Pointer comp = CompositeLung<FloatImageType, FloatImageType>::New();
	comp->SetInput(reader->GetOutput());
	comp->SetThreshold(threshold);
	comp->SetVariance(variance);
	comp->Update();
	

	// Display
	QuickView viewer;
	viewer.AddImage<FloatImageType>(reader->GetOutput());
	viewer.AddImage<FloatImageType>(comp->GetOutput());
	viewer.Visualize();
	
	return EXIT_SUCCESS;
	

}
