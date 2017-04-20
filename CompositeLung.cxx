#include "CompositeLung.h"

template <typename TInputImage, typename TOutputImage>
CompositeLung<TInputImage, TOutputImage>::CompositeLung() {
	gaussianFilter = filterType::New();
	thresholdFilter = ThresholdImageFilterType::New();
}

template <typename TInputImage, typename TOutputImage> 
CompositeLung<TInputImage, TOutputImage>::~CompositeLung() {

}

template <typename TInputImage, typename TOutputImage> 
void CompositeLung<TInputImage, TOutputImage>::GenerateData() {
	typename ImageType::Pointer img = ImageType::New();
	img->Graft(this->GetInput());

	// Create and setup a Gaussian filter
	filterType::Pointer gaussianFilter = filterType::New();
	gaussianFilter->SetInput(img);
	gaussianFilter->SetVariance(this->GetVariance());

	// Threshold 
	typedef itk::ThresholdImageFilter <TOutputImage> ThresholdImageFilterType;
	ThresholdImageFilterType::Pointer thresholdFilter = ThresholdImageFilterType::New();
	thresholdFilter->SetInput(gaussianFilter->GetOutput());
	thresholdFilter->ThresholdBelow(this->GetThreshold());
	thresholdFilter->SetOutsideValue(0);

	thresholdFilter->GraftOutput(this->GetOutput());
	thresholdFilter->Update();

	this->GraftOutput(thresholdFilter->GetOutput());




}