#pragma once
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkInvertIntensityImageFilter.h"
#include "itkBinaryMorphologicalOpeningImageFilter.h"
#include "itkBinaryMorphologicalClosingImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
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
	
	typedef itk::DiscreteGaussianImageFilter<TInputImage, TOutputImage >  FilterType;
	typedef itk::BinaryThresholdImageFilter <TInputImage, TOutputImage> ThresholdImageFilterType;
	typedef itk::InvertIntensityImageFilter<TOutputImage> InvertFilterType;
	typedef itk::BinaryBallStructuringElement<PixelType, 2> StructureFilterType;
	typedef itk::BinaryMorphologicalOpeningImageFilter<TInputImage, TOutputImage, StructureFilterType> OpeningFilterType;
	typedef itk::BinaryMorphologicalClosingImageFilter<TInputImage, TOutputImage, StructureFilterType> ClosingFilterType;
	
	typedef typename FilterType::Pointer FilterTypePointer;
	typedef typename ThresholdImageFilterType::Pointer ThresholdImageFilterTypePointer;
	typedef typename InvertFilterType::Pointer InvertIntensityImageFilterPointer;
	typedef typename OpeningFilterType::Pointer OpeningFilterPointer;
	typedef typename ClosingFilterType::Pointer ClosingFilterPointer;
	
private:
	FilterTypePointer gaussianFilter;
	ThresholdImageFilterTypePointer thresholdFilter;
	InvertIntensityImageFilterPointer invertFilter;
	OpeningFilterPointer openingFilter;
	ClosingFilterPointer closingFilter;
	StructureFilterType structureFilter;
	int m_Threshold;
	float m_Variance;
	int m_invert;


};