#pragma once
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkThresholdImageFilter.h"
#include "QuickView.h"

template<typename TInputImage, typename TOutputImage> 
class CompositeLung : public itk::ImageToImageFilter<TInputImage, TOutputImage> {
public: 
	typedef CompositeLung <TInputImage, TOutputImage> Self;
	typedef itk::ImageToImageFilter<TInputImage, TOutputImage> SuperClass;
	typedef itk::SmartPointer<Self> Pointer;
	typedef itk::SmartPointer<const Self> ConstPoint;
	typedef TInputImage ImageType;
	typedef typename ImageType::PixelType PixelType;
	itkNewMacro(Self);
	itkSetMacro(Threshold, int);
	itkSetMacro(Variance, float);
	itkGetMacro(Threshold, int);
	itkGetMacro(Variance, float);

	CompositeLung();
	~CompositeLung();

	void GenerateData();
	
protected: 
	typedef itk::Image< unsigned char, 2 > UnsignedCharImageType;
	typedef itk::Image< float, 2 >         FloatImageType;
	
	typedef itk::DiscreteGaussianImageFilter<TInputImage, TOutputImage >  filterType;
	typedef itk::ThresholdImageFilter <TOutputImage> ThresholdImageFilterType;
	
	typedef typename itk::DiscreteGaussianImageFilter<TInputImage, TOutputImage >::Pointer filterTypePointer;
	typedef typename itk::ThresholdImageFilter <TOutputImage>::Pointer ThresholdImageFilterTypePointer;

private:
	filterTypePointer gaussianFilter;
	ThresholdImageFilterTypePointer thresholdFilter;
	int m_Threshold;
	float m_Variance;


};