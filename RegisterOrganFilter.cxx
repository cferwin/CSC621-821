#include "RegisterOrganFilter.h"

template <typename TInputImage, typename TOutputImage>
RegisterOrganFilter<TInputImage, TOutputImage>::RegisterOrganFilter()
{
    downsampleBaseline = DownsampleType::New();
    downsampleLater = DownsampleType::New();
    transform = TransformType::New();
    optimizer = OptimizerType::New();
    interpolator = InterpolatorType::New();
    registration = RegistrationType::New();
    metric = MetricType::New();
    baselineNormalize = NormalizeType::New();
    laterNormalize = NormalizeType::New();
    baselineGaussianFilter = GaussianFilterType::New();
    laterGaussianFilter = GaussianFilterType::New();
    transformInitializer = TransformInitializerType::New();
    finalTransform = TransformType::New();
    resample = ResampleFilterType::New();

    // Set up downsampling
    downsampleBaseline->SetShrinkFactor(0, 4);
    downsampleBaseline->SetShrinkFactor(1, 4);
    downsampleBaseline->SetShrinkFactor(2, 4);

    downsampleLater->SetShrinkFactor(0, 4);
    downsampleLater->SetShrinkFactor(1, 4);
    downsampleLater->SetShrinkFactor(2, 4);

    // Set up metric
    metric->SetFixedImageStandardDeviation(0.4);
    metric->SetMovingImageStandardDeviation(0.4);

    // Set up optimizer
    optimizer->SetMaximumStepLength(0.1);
    optimizer->SetMinimumStepLength(0.01);
    optimizer->SetNumberOfIterations(500);
    optimizer->MaximizeOn();

   
    resample->SetDefaultPixelValue(100);
}

template <typename TInputImage, typename TOutputImage>
RegisterOrganFilter<TInputImage, TOutputImage>::~RegisterOrganFilter()
{
    //
}

template <typename TInputImage, typename TOutputImage>
void RegisterOrganFilter<TInputImage, TOutputImage>::GenerateData() {
    // Graft fixed image input
    typename ImageType::Pointer fixed = ImageType::New();
    fixed->Graft(this->GetFixedImage());

    // Graft moving image input
    typename ImageType::Pointer moving = ImageType::New();
    moving->Graft(this->GetMovingImage());


    // Attach inputs to the correct filters at the beginning of the composite pipeline
    downsampleBaseline->SetInput(fixed);
    downsampleLater->SetInput(moving);
    downsampleBaseline->Update();
    downsampleLater->Update();

    // Set up normalize
    baselineNormalize->SetInput(downsampleBaseline->GetOutput());
    laterNormalize->SetInput(downsampleLater->GetOutput());

    // Set up GaussianFilter
    baselineGaussianFilter->SetVariance(2.0);
    laterGaussianFilter->SetVariance(2.0);

    baselineGaussianFilter->SetInput(baselineNormalize->GetOutput());
    laterGaussianFilter->SetInput(laterNormalize->GetOutput());
    baselineGaussianFilter->Update();
    laterGaussianFilter->Update();

    // Set up registration
    registration->SetOptimizer(optimizer);
    registration->SetTransform(transform);
    registration->SetMetric(metric);
    registration->SetInterpolator(interpolator);
    registration->SetFixedImage(baselineGaussianFilter->GetOutput());
    registration->SetMovingImage(laterGaussianFilter->GetOutput());

    // Set up baseline region
    ImageType::RegionType baselineRegion = baselineNormalize->GetOutput()->GetBufferedRegion();
    registration->SetFixedImageRegion(baselineRegion);
    
    // Initialize transform
    transformInitializer->SetFixedImage(fixed);
    transformInitializer->SetMovingImage(moving);
    transformInitializer->SetTransform(transform);
    transformInitializer->MomentsOn();
    transformInitializer->InitializeTransform();
    registration->SetInitialTransformParameters(transform->GetParameters());

    // Calculate and set the number of samples used
    const unsigned int numSamples = static_cast<unsigned int>(baselineRegion.GetNumberOfPixels() * 0.01);
    metric->SetNumberOfSpatialSamples(numSamples);
    registration->Update();

    // Final transform
    RegistrationType::ParametersType finalParameters = registration->GetLastTransformParameters();
    finalTransform->SetParameters(finalParameters);
    finalTransform->SetFixedParameters(transform->GetFixedParameters());

    // Resample
    resample->SetTransform(finalTransform);
    resample->SetSize(this->GetFixedImage()->GetLargestPossibleRegion().GetSize());
    resample->SetInput(this->GetMovingImage());
    resample->SetOutputOrigin(this->GetFixedImage()->GetOrigin());
    resample->SetOutputSpacing(this->GetFixedImage()->GetSpacing());
    resample->SetOutputDirection(this->GetFixedImage()->GetDirection());

    // Graft outputs at end of the pipeline
    resample->GraftOutput(this->GetOutput());
    resample->Update();
    this->GraftOutput(resample->GetOutput());
}

template<typename TInputImage, typename TOutputImage>
void RegisterOrganFilter<TInputImage, TOutputImage>::SetFixedImage(const TInputImage *image) {
    if (image != static_cast<ImageType*>(this->ProcessObject::GetInput(0))) { 
        this->ProcessObject::SetNthInput(0, const_cast<ImageType*>(image));
        this->Modified();
    }
}

template<typename TInputImage, typename TOutputImage>
void RegisterOrganFilter<TInputImage, TOutputImage>::SetMovingImage(const ImageType *image) {
    if (image != static_cast<ImageType*>(this->ProcessObject::GetInput(1))) {
        this->ProcessObject::SetNthInput(1, const_cast<ImageType*>(image));
        this->Modified();
    }
}

template<typename TInputImage, typename TOutputImage>
const TInputImage *
RegisterOrganFilter<TInputImage, TOutputImage>::GetFixedImage() {
    return static_cast<const ImageType*>(this->ProcessObject::GetInput(0));
}

template<typename TInputImage, typename TOutputImage>
const TInputImage *
RegisterOrganFilter<TInputImage, TOutputImage>::GetMovingImage() {
    return static_cast<const ImageType*>(this->ProcessObject::GetInput(1));
}