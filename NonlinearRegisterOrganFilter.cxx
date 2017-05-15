#include "NonlinearRegisterOrganFilter.h"
#include "itkCommand.h"

using namespace std;

class CommandIterationUpdate : public itk::Command
{
    public:
    typedef  CommandIterationUpdate                     Self;
    typedef  itk::Command                               Superclass;
    typedef  itk::SmartPointer<CommandIterationUpdate>  Pointer;

    itkNewMacro( CommandIterationUpdate );

    protected:
    CommandIterationUpdate() {};

    typedef itk::Image<float, 3>            InternalImageType;
    typedef itk::Vector<float, 3>           VectorPixelType;
    typedef itk::Image<VectorPixelType, 3>  DisplacementFieldType;
    typedef itk::DemonsRegistrationFilter<InternalImageType, InternalImageType, DisplacementFieldType>   RegistrationFilterType;

    public:

    void Execute(itk::Object *caller, const itk::EventObject & event) ITK_OVERRIDE
    {
        Execute((const itk::Object *)caller, event);
    }

    void Execute(const itk::Object * object, const itk::EventObject & event) ITK_OVERRIDE
    {
        const RegistrationFilterType * filter = static_cast<const RegistrationFilterType *>(object);
        if(!(itk::IterationEvent().CheckEvent(&event))) {
            return;
        }
        std::cout <<  ": " << filter->GetMetric() << std::endl;
    }
};

template <typename TInputImage, typename TOutputImage>
NonlinearRegisterOrganFilter<TInputImage, TOutputImage>::NonlinearRegisterOrganFilter()
{
    downsampleBaseline = DownsampleType::New();
    downsampleLater = DownsampleType::New();
    baselineNormalize = NormalizeType::New();
    laterNormalize = NormalizeType::New();
    matcher = MatchingFilterType::New();
    filter = RegistrationFilterType::New();
    warper = WarperType::New();
    interpolator = InterpolatorType::New();
    transform = IdentityTransformType::New();
    resample = ResampleFilterType::New();
}

template <typename TInputImage, typename TOutputImage>
NonlinearRegisterOrganFilter<TInputImage, TOutputImage>::~NonlinearRegisterOrganFilter()
{
    //
}

template <typename TInputImage, typename TOutputImage>
void NonlinearRegisterOrganFilter<TInputImage, TOutputImage>::GenerateData() {
    // Graft fixed image input
    typename ImageType::Pointer fixed = ImageType::New();
    fixed->Graft(this->GetFixedImage());

    // Graft moving image input
    typename ImageType::Pointer moving = ImageType::New();
    moving->Graft(this->GetMovingImage());

    cout << "Start downsample" << endl;
    // Set up downsampling
    downsampleBaseline->SetInput(fixed);
    downsampleBaseline->SetShrinkFactor(0, 4);
    downsampleBaseline->SetShrinkFactor(1, 4);
    downsampleBaseline->SetShrinkFactor(2, 1);

    downsampleLater->SetInput(moving);
    downsampleLater->SetShrinkFactor(0, 4);
    downsampleLater->SetShrinkFactor(1, 4);
    downsampleLater->SetShrinkFactor(2, 1);

    cout << "downsample update" << endl;
    downsampleBaseline->Update();
    downsampleLater->Update();
    cout << "downsample done, start norm" << endl;

    // Set up normalize
    baselineNormalize->SetInput(downsampleBaseline->GetOutput());
    laterNormalize->SetInput(downsampleLater->GetOutput());
    cout << "norm update" << endl;
    baselineNormalize->Update();
    laterNormalize->Update();
    cout << "norm done, start matching" << endl;

    // Set up histogram matcher
    matcher->SetInput( laterNormalize->GetOutput() );
    matcher->SetReferenceImage( baselineNormalize->GetOutput() );
    matcher->SetNumberOfHistogramLevels( 1024 );
    matcher->SetNumberOfMatchPoints( 10000 );
    matcher->ThresholdAtMeanIntensityOn();

    CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
    filter->AddObserver( itk::IterationEvent(), observer );

    filter->SetFixedImage( baselineNormalize->GetOutput() );
    filter->SetMovingImage( matcher->GetOutput() );
    filter->SetNumberOfIterations( 500 );
    filter->SetStandardDeviations( 12.0 );
    cout << "update matching filter" << endl;
    filter->UpdateLargestPossibleRegion();
    cout << "matching filter done, start warp" << endl;

    // Set up B-Spline interpolator
    interpolator->SetSplineOrder(3);

    // Set up the final resampler to scale up the warp vector field
    const double origin[3]  = { 0.0, 0.0, 0.0 };
    const ImageType::SizeType& size = fixed->GetLargestPossibleRegion().GetSize();
    const ImageType::SpacingType& spacing = fixed->GetSpacing();
    transform->SetIdentity();
    resample->SetTransform(transform);
    //resample->SetInterpolator(interpolator);
    resample->SetOutputOrigin(origin);
    resample->SetInput(filter->GetOutput());
 
    // Calculate new spacing
    double outputSpacing[3];
    outputSpacing[0] = spacing[0] * (double) size[0] / (double) 512;
    outputSpacing[1] = spacing[1] * (double) size[1] / (double) 512;
    outputSpacing[2] = spacing[2];
    resample->SetOutputSpacing(outputSpacing);

    // Set new size
    itk::Size<3> outputSize = { {512, 512, size[2]} };
    resample->SetSize(outputSize);
 
    resample->Update();

    // Warp the moving image with the larger displacement field
    warper->SetInput( moving );
    warper->SetInterpolator( interpolator );
    warper->SetOutputSpacing( moving->GetSpacing() );
    warper->SetOutputOrigin( moving->GetOrigin() );
    warper->SetOutputDirection( moving->GetDirection() );
    warper->SetDisplacementField( resample->GetOutput() );

    // Graft outputs at end of the pipeline
    cout << "graft warp" << endl;
    warper->GraftOutput(this->GetOutput());
    cout << "update warp" << endl;
    warper->UpdateLargestPossibleRegion();
    cout << "warp updated, graft out" << endl;
    this->GraftOutput(warper->GetOutput());
}

template<typename TInputImage, typename TOutputImage>
void NonlinearRegisterOrganFilter<TInputImage, TOutputImage>::SetFixedImage(const TInputImage *image) {
    if (image != static_cast<ImageType*>(this->ProcessObject::GetInput(0))) { 
        this->ProcessObject::SetNthInput(0, const_cast<ImageType*>(image));
        this->Modified();
    }
}

template<typename TInputImage, typename TOutputImage>
void NonlinearRegisterOrganFilter<TInputImage, TOutputImage>::SetMovingImage(const ImageType *image) {
    if (image != static_cast<ImageType*>(this->ProcessObject::GetInput(1))) {
        this->ProcessObject::SetNthInput(1, const_cast<ImageType*>(image));
        this->Modified();
    }
}

template<typename TInputImage, typename TOutputImage>
const TInputImage *
NonlinearRegisterOrganFilter<TInputImage, TOutputImage>::GetFixedImage() {
    return static_cast<const ImageType*>(this->ProcessObject::GetInput(0));
}

template<typename TInputImage, typename TOutputImage>
const TInputImage *
NonlinearRegisterOrganFilter<TInputImage, TOutputImage>::GetMovingImage() {
    return static_cast<const ImageType*>(this->ProcessObject::GetInput(1));
}
