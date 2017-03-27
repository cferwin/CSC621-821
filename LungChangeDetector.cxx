#include <iostream>
#include <vector>
#include <string>
#include <itkImage.h>
#include <itkImageSeriesReader.h>
#include <itkNumericSeriesFileNames.h>
#include <itkAffineTransform.h>
#include <itkGradientDescentOptimizer.h>
#include <itkMutualInformationImageToImageMetric.h>
#include <itkDiscreteGaussianImageFilter.h>
#include <itkImageRegistrationMethod.h>
#include <itkNormalizeImageFilter.h>
#include <itkResampleImageFilter.h>
#include <itkImageSeriesWriter.h>
#include <itkCommand.h>
#include <itkTranslationTransform.h>

class CommandIterationUpdate : public itk::Command {
public:
    typedef CommandIterationUpdate Self;
    typedef itk::Command Superclass;
    typedef itk::SmartPointer<Self> Pointer;
    itkNewMacro(Self);

protected:
    CommandIterationUpdate() {};

public:
    typedef itk::GradientDescentOptimizer OptimizerType;
    typedef const OptimizerType* OptimizerPointer;

    void Execute(itk::Object *caller, const itk::EventObject &event) ITK_OVERRIDE {
        Execute((const itk::Object*) caller, event);
    }

    void Execute(const itk::Object *object, const itk::EventObject &event) ITK_OVERRIDE {
        OptimizerPointer optimizer = static_cast<OptimizerPointer>(object);
        if (!itk::IterationEvent().CheckEvent(&event)) {
            return;
        }

        std::cout << optimizer->GetCurrentIteration() << "   ";
        std::cout << optimizer->GetValue() << "    ";
        std::cout << optimizer->GetCurrentPosition() << std::endl;
    }
};

#define DIMENSION 3
#define OUT_DIMENSION 2

int main(int argc, char **argv) {
    // Define types
    typedef itk::Image<double, DIMENSION> ImageType;
    typedef itk::ImageSeriesReader<ImageType> ReaderType;
    typedef itk::NumericSeriesFileNames NameGeneratorType;
    //typedef itk::AffineTransform<double, DIMENSION> TransformType;
    typedef itk::TranslationTransform<double, DIMENSION> TransformType;
    typedef itk::GradientDescentOptimizer   OptimizerType;
    typedef itk::LinearInterpolateImageFunction<ImageType, double>  InterpolatorType;
    typedef itk::ImageRegistrationMethod<ImageType, ImageType>  RegistrationType;
    typedef itk::MutualInformationImageToImageMetric<ImageType, ImageType> MetricType;
    typedef itk::NormalizeImageFilter<ImageType, ImageType> NormalizeType;
    typedef itk::DiscreteGaussianImageFilter<ImageType, ImageType> GaussianFilterType;
    typedef itk::ResampleImageFilter<ImageType, ImageType> ResampleFilterType;
    typedef itk::Image<unsigned char, OUT_DIMENSION> OutputImageType; // JPG writer only supports int and unsigned char
    typedef itk::ImageSeriesWriter<ImageType, OutputImageType> WriterType;
    
    // Define variables
    NameGeneratorType::Pointer nameGenerator = NameGeneratorType::New();
    TransformType::Pointer transform = TransformType::New();
    OptimizerType::Pointer optimizer = OptimizerType::New();
    InterpolatorType::Pointer interpolator = InterpolatorType::New();
    RegistrationType::Pointer registration = RegistrationType::New();
    MetricType::Pointer metric = MetricType::New();
    NormalizeType::Pointer baselineNormalize = NormalizeType::New();
    NormalizeType::Pointer laterNormalize = NormalizeType::New();
    GaussianFilterType::Pointer baselineGaussianFilter = GaussianFilterType::New();
    GaussianFilterType::Pointer laterGaussianFilter = GaussianFilterType::New();
    TransformType::Pointer finalTransform = TransformType::New();
    ResampleFilterType::Pointer resample = ResampleFilterType::New();
    WriterType::Pointer writer = WriterType::New();
    
    // Accept input or display usage message
    if (argc != 8) {
        // TODO: UPDATE MESSAGE
        std::cout << "USAGE: " << std::endl;
        std::cout << "LungChangeDetector.exe <File Path Template for Set 1> <Start Index> <End Index> <File Path Template for Set 2> <Start Index> <End Index> <Output Path Template>" << std::endl;
        std::cout << "File Path Template X -- A standardized file name/path for each numbered image" << std::endl;
        std::cout << "      with \"%d\" standing in for the number, such as \"C:\\foo %d.tif\" for" << std::endl;
        std::cout << "      files foo 1.tif, foo 2.tif, etc." << std::endl;
        std::cout << "Start Index -- Number of the first image." << std::endl;
        std::cout << "End Index -- Number of the last image." << std::endl << std::endl;
        std::cout << "For Example:" << std::endl;
        std::cout << "LungChangeDetector.exe \"C:\\images\\baseline (%d).tif\" 1 404 \"C:\\images\\later (%d).tif\" 1 404 \"C:\\images\\output%d.tif\"" << std::endl;
        std::cout << "To load files baseline (1).tif through baseline (404).tif" << std::endl;
        std::cout << "and compare with later (1).tif through later (404).tif" << std::endl;
        return 1;
    }

    // Import Baseline Series
    //
    ReaderType::Pointer baselineReader = ReaderType::New();

    // Generate file paths
    nameGenerator->SetSeriesFormat(argv[1]);
    nameGenerator->SetStartIndex(std::stoi(argv[2]));
    nameGenerator->SetEndIndex(std::stoi(argv[3]));
    nameGenerator->SetIncrementIndex(1);
    std::vector<std::string> filePaths = nameGenerator->GetFileNames();

    // Load slice image files into memory with series reader.
    baselineReader->SetFileNames(filePaths);
    // TODO
    //baselineReader->SetFileName(argv[1]);
    baselineReader->Update();

    // Import Later Series
    //
    ReaderType::Pointer laterReader = ReaderType::New();

    // Generate file paths
    nameGenerator->SetSeriesFormat(argv[4]);
    nameGenerator->SetStartIndex(std::stoi(argv[5]));
    nameGenerator->SetEndIndex(std::stoi(argv[6]));
    nameGenerator->SetIncrementIndex(1);
    filePaths = nameGenerator->GetFileNames();

    // Load slice image files into memory with series reader.
    laterReader->SetFileNames(filePaths);
    // TODO
    //laterReader->SetFileName(argv[4]);
    laterReader->Update();

    //set up metric
    metric->SetFixedImageStandardDeviation(0.4);
    metric->SetMovingImageStandardDeviation(0.4);
    
    //set up normalize
    baselineNormalize->SetInput(baselineReader->GetOutput());
    laterNormalize->SetInput(laterReader->GetOutput());
    
    //set up GaussianFilter
    baselineGaussianFilter->SetVariance(2.0);
    laterGaussianFilter->SetVariance(2.0);
    
    baselineGaussianFilter->SetInput(baselineNormalize->GetOutput());
    laterGaussianFilter->SetInput(laterNormalize->GetOutput());
    
    
    // set up registration
    registration->SetOptimizer(optimizer);
    registration->SetTransform(transform);
    registration->SetMetric(metric);
    registration->SetInterpolator(interpolator);
    registration->SetFixedImage(baselineGaussianFilter->GetOutput());
    registration->SetMovingImage(laterGaussianFilter->GetOutput());

    // Set up baseline region
    baselineNormalize->Update();
    ImageType::RegionType baselineRegion = baselineNormalize->GetOutput()->GetBufferedRegion();
    registration->SetFixedImageRegion(baselineRegion);

    // Set up initial offset parameters
    RegistrationType::ParametersType initialParameters(transform->GetNumberOfParameters());
    initialParameters[0] = 0.0;
    initialParameters[1] = 0.0;
    initialParameters[2] = 0.0;

    // Calculate and set the number of samples used
    const unsigned int numSamples = static_cast<unsigned int>(baselineRegion.GetNumberOfPixels() * 0.01);
    metric->SetNumberOfSpatialSamples(numSamples);
    registration->SetInitialTransformParameters(initialParameters);

    // Set up optimizer
    optimizer->SetLearningRate(15.0);
    optimizer->SetNumberOfIterations(200);
    optimizer->MaximizeOn();

    // Add observer
    CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
    optimizer->AddObserver(itk::IterationEvent(), observer);
    
    // Set up the stop condition and transforms
    try {
        registration->Update();
    }
    catch (itk::ExceptionObject e) {
        std::cout << e.GetDescription() << std::endl;
    }
    RegistrationType::ParametersType finalParameters = registration->GetLastTransformParameters();
    finalTransform->SetParameters(finalParameters);
    finalTransform->SetFixedParameters(transform->GetFixedParameters());

    // Resample
    ImageType::Pointer baseline = baselineReader->GetOutput();
    resample->SetTransform(finalTransform);
    resample->SetInput(laterReader->GetOutput());
    resample->SetSize(baseline->GetLargestPossibleRegion().GetSize());
    resample->SetOutputOrigin(baseline->GetOrigin());
    resample->SetOutputSpacing(baseline->GetSpacing());
    resample->SetOutputDirection(baseline->GetDirection());
    resample->SetDefaultPixelValue(100);

    // Generate output file paths
    nameGenerator->SetSeriesFormat(argv[7]);
    nameGenerator->SetStartIndex(1);
    nameGenerator->SetEndIndex(std::stoi(argv[3]));
    nameGenerator->SetIncrementIndex(1);
    filePaths = nameGenerator->GetFileNames();

    // Write output image
    writer->SetFileNames(filePaths);
    writer->SetInput(resample->GetOutput());
    writer->Update();

	return 0;
}
