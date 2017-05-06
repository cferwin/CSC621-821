#include "CompositeLung.h"

template <typename TInputImage, typename TOutputImage>
CompositeLung<TInputImage, TOutputImage>::CompositeLung() {
	gaussianFilter = FilterType::New();
	thresholdFilter = ThresholdImageFilterType::New();
	invertFilter = InvertFilterType::New();
	openingFilter = OpeningFilterType::New();
	closingFilter = ClosingFilterType::New();
}

template <typename TInputImage, typename TOutputImage> 
CompositeLung<TInputImage, TOutputImage>::~CompositeLung() {

}

template <typename TInputImage, typename TOutputImage> 
void CompositeLung<TInputImage, TOutputImage>::GenerateData() {
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

	// Invert
	invertFilter->SetInput(openingFilter->GetOutput());
	invertFilter->SetMaximum(255);
	
	invertFilter->GraftOutput(this->GetOutput());
	invertFilter->Update();
	this->GraftOutput(invertFilter->GetOutput());
}