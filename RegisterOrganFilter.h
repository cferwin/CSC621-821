#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <itkImage.h>
#include <itkImageSeriesReader.h>
#include <itkNumericSeriesFileNames.h>
#include <itkAffineTransform.h>
#include <itkRegularStepGradientDescentOptimizer.h>
#include <itkMutualInformationImageToImageMetric.h>
#include <itkDiscreteGaussianImageFilter.h>
#include <itkImageRegistrationMethod.h>
#include <itkNormalizeImageFilter.h>
#include <itkResampleImageFilter.h>
#include <itkImageSeriesWriter.h>
#include <itkCommand.h>
#include <itkTranslationTransform.h>
#include <itkCenteredTransformInitializer.h>
#include <itkShrinkImageFilter.h>
#include <itkBinaryFunctorImageFilter.h>
#include <itkImageToImageFilter.h>

#define DIMENSION 3
#define OUT_DIMENSION 3

template <typename TInputImage, typename TOutputImage>
class RegisterOrganFilter : public itk::ImageToImageFilter<TInputImage, TOutputImage>
{
public:
    typedef RegisterOrganFilter<TInputImage, TOutputImage> Self;
    typedef itk::ImageToImageFilter<TInputImage, TOutputImage> Superclass;
    typedef itk::SmartPointer<Self> Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;
    typedef TInputImage ImageType;
    typedef typename ImageType::PixelType PixelType;

    itkNewMacro(Self);
    //itkGetMacro(Threshold, PixelType);
    //itkSetMacro(Threshold, PixelType);

    RegisterOrganFilter();
    ~RegisterOrganFilter();
    void GenerateData();
    void RegisterOrganFilter<TInputImage, TOutputImage>::SetFixedImage(const TInputImage *image);
    void SetMovingImage(const ImageType *image);
    const ImageType* GetFixedImage();
    const ImageType* GetMovingImage();

protected:
    // Define types
    typedef itk::AffineTransform<double, DIMENSION> TransformType;
    typedef typename TransformType::Pointer TransformTypePointer;
    typedef itk::RegularStepGradientDescentOptimizer OptimizerType;
    typedef typename OptimizerType::Pointer OptimizerTypePointer;
    typedef itk::LinearInterpolateImageFunction<TInputImage, double>  InterpolatorType;
    typedef typename InterpolatorType::Pointer InterpolatorTypePointer;
    typedef itk::ImageRegistrationMethod<TInputImage, TInputImage>  RegistrationType;
    typedef typename RegistrationType::Pointer RegistrationTypePointer;
    typedef itk::MutualInformationImageToImageMetric<TInputImage, TInputImage> MetricType;
    typedef typename MetricType::Pointer MetricTypePointer;
    typedef itk::NormalizeImageFilter<TInputImage, TInputImage> NormalizeType;
    typedef typename NormalizeType::Pointer NormalizeTypePointer;
    typedef itk::DiscreteGaussianImageFilter<TInputImage, TInputImage> GaussianFilterType;
    typedef typename GaussianFilterType::Pointer GaussianFilterTypePointer;
    typedef itk::ResampleImageFilter<TInputImage, TOutputImage> ResampleFilterType;
    typedef typename ResampleFilterType::Pointer ResampleFilterTypePointer;
    typedef itk::CenteredTransformInitializer<TransformType, TInputImage, TInputImage> TransformInitializerType;
    typedef typename TransformInitializerType::Pointer TransformInitializerTypePointer;
    typedef itk::ShrinkImageFilter<TInputImage, TInputImage> DownsampleType;
    typedef typename DownsampleType::Pointer DownsampleTypePointer;

private:
    // Downsample the images to make registration faster
    // TODO: Replace this with a resample filter to 128x128
    DownsampleTypePointer downsampleBaseline;
    DownsampleTypePointer downsampleLater;
    TransformTypePointer transform;
    OptimizerTypePointer optimizer;
    InterpolatorTypePointer interpolator;
    RegistrationTypePointer registration;
    MetricTypePointer metric;
    NormalizeTypePointer baselineNormalize;
    NormalizeTypePointer laterNormalize;
    GaussianFilterTypePointer baselineGaussianFilter;
    GaussianFilterTypePointer laterGaussianFilter;
    TransformInitializerTypePointer transformInitializer;
    TransformTypePointer finalTransform;
    ResampleFilterTypePointer resample;
};