#include "SegmentLungVolume.h"

template <typename TInputImage, typename TOutputImage>
SegmentLungVolume<TInputImage, TOutputImage>::SegmentLungVolume() {
	gaussianFilter = FilterType::New();
	thresholdFilter = ThresholdImageFilterType::New();
	invertFilter = InvertFilterType::New();
	openingFilter = OpeningFilterType::New();
	closingFilter = ClosingFilterType::New();
}

template <typename TInputImage, typename TOutputImage> 
SegmentLungVolume<TInputImage, TOutputImage>::~SegmentLungVolume() {
}

template <typename TInputImage, typename TOutputImage> 
void SegmentLungVolume<TInputImage, TOutputImage>::GenerateData() {
	typename ImageType::Pointer img = ImageType::New();
	img->Graft(this->GetInput());

	// Create and setup a Gaussian filter
	gaussianFilter->SetInput(img);
	gaussianFilter->SetVariance(this->GetVariance());

	// Threshold
	thresholdFilter->SetInput(img);
	thresholdFilter->SetLowerThreshold(0);
	thresholdFilter->SetUpperThreshold(this->GetThreshold());
	thresholdFilter->SetOutsideValue(0);
	thresholdFilter->Update();

	// Structuring
	structureFilter.SetRadius(3);
	structureFilter.CreateStructuringElement();

	// Closing
	closingFilter->SetInput(thresholdFilter->GetOutput());
	closingFilter->SetKernel(structureFilter);

	// Opening
	openingFilter->SetInput(closingFilter->GetOutput());
	openingFilter->SetKernel(structureFilter);
	openingFilter->Update();

    // Convert to output type
    typedef itk::CastImageFilter< TInputImage, TOutputImage > CastFilterType;
    CastFilterType::Pointer castFilter = CastFilterType::New();
    castFilter->SetInput(openingFilter->GetOutput());

    /*
	openingFilter->GraftOutput(this->GetOutput());
	openingFilter->Update();
	this->GraftOutput(openingFilter->GetOutput());
    */
	castFilter->GraftOutput(this->GetOutput());
	castFilter->Update();
	this->GraftOutput(castFilter->GetOutput());
}