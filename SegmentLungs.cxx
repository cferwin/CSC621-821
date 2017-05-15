#include "CompositeLung.cxx"
#include "CompositeLung.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkThresholdImageFilter.h"
#include "QuickView.h"
#include "itkMaskImageFilter.h"
#include "itkMaskNegatedImageFilter.h"
#include "itkAddImageFilter.h"
#include "itkBinaryFillholeImageFilter.h"
#include "itkInvertIntensityImageFilter.h"

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
	typedef itk::Image< unsigned char, 2 >         MaskType;

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

    /*
    // Invert mask
    itk::InvertIntensityImageFilter<FloatImageType, FloatImageType>::Pointer invertFilter = itk::InvertIntensityImageFilter<FloatImageType, FloatImageType>::New();
    invertFilter->SetMaximum(255);
    invertFilter->SetInput(comp->GetOutput());
    //invertFilter->Update();

    // Fill holes in mask -- full body mask
    itk::BinaryFillholeImageFilter<FloatImageType>::Pointer fillHoles = itk::BinaryFillholeImageFilter<FloatImageType>::New();
    fillHoles->SetInput(invertFilter->GetOutput());
    fillHoles->SetForegroundValue( 255 );
    fillHoles->Update();

    // Invert mask back
    itk::InvertIntensityImageFilter<FloatImageType, FloatImageType>::Pointer invertFilter2 = itk::InvertIntensityImageFilter<FloatImageType, FloatImageType>::New();
    invertFilter2->SetMaximum(255);
    invertFilter2->SetInput(fillHoles->GetOutput());
    //invertFilter2->Update();
    */
	
    // Mask for the body region
    itk::MaskImageFilter<FloatImageType, FloatImageType, FloatImageType>::Pointer mask = itk::MaskImageFilter<FloatImageType, FloatImageType, FloatImageType>::New();
    mask->SetMaskImage(comp->GetOutput());
    mask->SetInput(reader->GetOutput());
    mask->SetMaskingValue(0);
    mask->Update();
    
    /*
    //
    itk::InvertIntensityImageFilter<MaskType, MaskType>::Pointer invertFilter3 = itk::InvertIntensityImageFilter<MaskType, MaskType>::New();
    invertFilter3->SetMaximum(255);
    invertFilter3->SetInput(comp->GetOutput());
    //invertFilter3->Update();

    // Invert and mask for lung regions
    itk::MaskImageFilter<FloatImageType, MaskType, FloatImageType>::Pointer lungMask = itk::MaskImageFilter<FloatImageType, MaskType, FloatImageType>::New();
    lungMask->SetMaskImage(invertFilter3->GetOutput());
    lungMask->SetInput(mask->GetOutput());
    lungMask->SetMaskingValue(255);
    lungMask->Update();
    */

	// Display
	QuickView viewer;
    viewer.AddImage<FloatImageType>(reader->GetOutput());
	viewer.AddImage<FloatImageType>(mask->GetOutput());
    /*
	viewer.AddImage<MaskType>(comp->GetOutput());
	viewer.AddImage<MaskType>(invertFilter->GetOutput());
	viewer.AddImage<MaskType>(fillHoles->GetOutput());
	viewer.AddImage<MaskType>(invertFilter2->GetOutput());
	viewer.AddImage<FloatImageType>(mask->GetOutput());
	viewer.AddImage<MaskType>(invertFilter3->GetOutput());
	viewer.AddImage<FloatImageType>(lungMask->GetOutput());
    */
	viewer.Visualize();
	
	return EXIT_SUCCESS;
	

}
