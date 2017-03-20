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



int main(int argc, char **argv) {
    // Define types
    typedef itk::Image<double, 3> ImageType;
    typedef itk::ImageSeriesReader<ImageType> ReaderType;
    typedef itk::NumericSeriesFileNames NameGeneratorType;
    typedef itk::AffineTransform<double, 3> TransformType;
    typedef itk::GradientDescentOptimizer   OptimizerType;
    typedef itk::LinearInterpolateImageFunction<ImageType, double>  InterpolatorType;
    typedef itk::ImageRegistrationMethod<ImageType, ImageType>  RegistrationType;
    typedef itk::MutualInformationImageToImageMetric<ImageType, ImageType> MetricType;
    typedef itk::NormalizeImageFilter<ImageType, ImageType> NormalizeType;
    typedef itk::DiscreteGaussianImageFilter<ImageType, ImageType> GaussianFilterType;
    
    
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

    
    // Accept input or display usage message
    if (argc != 7) {
        // TODO: UPDATE MESSAGE
        std::cout << "USAGE: " << std::endl;
        std::cout << "LungChangeDetector.exe <File Path Template for Set 1> <Start Index> <End Index> <File Path Template for Set 2> <Start Index <End Index>" << std::endl;
        std::cout << "File Path Template X -- A standardized file name/path for each numbered image" << std::endl;
        std::cout << "      with \"%d\" standing in for the number, such as \"C:\\foo %d.tif\" for" << std::endl;
        std::cout << "      files foo 1.tif, foo 2.tif, etc." << std::endl;
        std::cout << "Start Index -- Number of the first image." << std::endl;
        std::cout << "End Index -- Number of the last image." << std::endl << std::endl;
        std::cout << "For Example:" << std::endl;
        std::cout << "LungChangeDetector.exe \"C:\\images\\baseline (%d).tif\" 1 404 \"C:\\images\\later (%d).tif\" 1 404" << std::endl;
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
    registration->SetFixedImage(laterGaussianFilter->GetOutput());

    //update the normalize
    baselineNormalize->Update();

    

	return 0;
}
