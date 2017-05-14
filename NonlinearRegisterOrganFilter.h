#pragma once
#include <iostream>
#include "itkImageRegionIterator.h"
#include <itkAffineTransform.h>
#include <itkTranslationTransform.h>
#include <itkGradientDescentOptimizer.h>
#include <itkMutualInformationImageToImageMetric.h>
#include <itkImageRegistrationMethod.h>
#include <itkCenteredTransformInitializer.h>
#include <itkResampleImageFilter.h>
#include "itkDemonsRegistrationFilter.h"
#include "itkHistogramMatchingImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkWarpImageFilter.h"
#include "itkShrinkImageFilter.h"
#include <itkNormalizeImageFilter.h>

#define DIMENSION 3
#define OUT_DIMENSION 3

template <typename TInputImage, typename TOutputImage>
class NonlinearRegisterOrganFilter : public itk::ImageToImageFilter<TInputImage, TOutputImage>
{
public:
    typedef NonlinearRegisterOrganFilter<TInputImage, TOutputImage> Self;
    typedef itk::ImageToImageFilter<TInputImage, TOutputImage> Superclass;
    typedef itk::SmartPointer<Self> Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;
    typedef TInputImage ImageType;
    typedef typename ImageType::PixelType PixelType;

    itkNewMacro(Self);

    NonlinearRegisterOrganFilter();
    ~NonlinearRegisterOrganFilter();
    void GenerateData();
    void NonlinearRegisterOrganFilter<TInputImage, TOutputImage>::SetFixedImage(const TInputImage *image);
    void SetMovingImage(const ImageType *image);
    const ImageType* GetFixedImage();
    const ImageType* GetMovingImage();

protected:
    // Define types
    typedef itk::ShrinkImageFilter<ImageType, ImageType> DownsampleType;
    typedef typename DownsampleType::Pointer DownsampleTypePointer;
    typedef itk::NormalizeImageFilter<ImageType, ImageType> NormalizeType;
    typedef typename NormalizeType::Pointer NormalizeTypePointer;
    typedef itk::HistogramMatchingImageFilter<ImageType, ImageType> MatchingFilterType;
    typedef typename itk::HistogramMatchingImageFilter<ImageType, ImageType>::Pointer MatchingFilterTypePointer;
    typedef itk::Vector<float, DIMENSION> VectorPixelType;
    typedef itk::Image<VectorPixelType, DIMENSION> DisplacementFieldType;
    typedef itk::DemonsRegistrationFilter<ImageType, ImageType, DisplacementFieldType> RegistrationFilterType;
    typedef typename itk::DemonsRegistrationFilter<ImageType, ImageType, DisplacementFieldType>::Pointer RegistrationFilterTypePointer;
    typedef itk::WarpImageFilter<itk::Image<float, DIMENSION>, ImageType, DisplacementFieldType> WarperType;
    typedef typename itk::WarpImageFilter<itk::Image<float, DIMENSION>, ImageType, DisplacementFieldType>::Pointer WarperTypePointer;
    typedef itk::BSplineInterpolateImageFunction<ImageType, double, double> InterpolatorType;
    typedef typename itk::BSplineInterpolateImageFunction<ImageType, double, double>::Pointer InterpolatorTypePointer;
    typedef itk::IdentityTransform<double, DIMENSION> IdentityTransformType;
    typedef typename itk::IdentityTransform<double, DIMENSION>::Pointer IdentityTransformTypePointer;
    typedef itk::ResampleImageFilter<itk::Image<VectorPixelType, DIMENSION>, itk::Image<VectorPixelType, DIMENSION>> ResampleFilterType;
    typedef typename itk::ResampleImageFilter<itk::Image<VectorPixelType, DIMENSION>, itk::Image<VectorPixelType, DIMENSION>>::Pointer ResampleFilterTypePointer;

private:
    // Define variables
    DownsampleTypePointer downsampleBaseline;
    DownsampleTypePointer downsampleLater;
    NormalizeTypePointer baselineNormalize;
    NormalizeTypePointer laterNormalize;
    MatchingFilterTypePointer matcher;
    RegistrationFilterTypePointer filter;
    WarperTypePointer warper;
    InterpolatorTypePointer interpolator;
    IdentityTransformTypePointer transform;
    ResampleFilterTypePointer resample;
};
