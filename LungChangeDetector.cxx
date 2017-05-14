#include "RegisterOrganFilter.h"
#include "RegisterOrganFilter.cxx"
#include "NonlinearRegisterOrganFilter.h"
#include "NonlinearRegisterOrganFilter.cxx"
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

#define DIMENSION 3
#define OUT_DIMENSION 3

int main(int argc, char **argv) {
    // Define types
    typedef itk::Image<float, DIMENSION> ImageType;
    typedef itk::ImageSeriesReader<ImageType> ReaderType;
    typedef itk::NumericSeriesFileNames NameGeneratorType;
    typedef itk::Image<float, OUT_DIMENSION> OutputImageType;
    typedef itk::ImageSeriesWriter<ImageType, OutputImageType> WriterType;

    
    // Define variables
    NameGeneratorType::Pointer nameGenerator = NameGeneratorType::New();
    WriterType::Pointer writer = WriterType::New();
    
    // Accept input or display usage message
    if (argc != 8) {
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

    

    // Generate output file paths
    // TODO: Can clean up if sticking with 3D output images
    nameGenerator->SetSeriesFormat(argv[7]);
    nameGenerator->SetStartIndex(1);
    //nameGenerator->SetEndIndex(std::stoi(argv[3]));
    nameGenerator->SetEndIndex(1);
    nameGenerator->SetIncrementIndex(1);
    filePaths = nameGenerator->GetFileNames();

    try {
        RegisterOrganFilter<ImageType, OutputImageType>::Pointer reg = RegisterOrganFilter<ImageType, OutputImageType>::New();
        reg->SetFixedImage(baselineReader->GetOutput());
        reg->SetMovingImage(laterReader->GetOutput());
        reg->Update();

        NonlinearRegisterOrganFilter<ImageType, OutputImageType>::Pointer nonlinearReg = NonlinearRegisterOrganFilter<ImageType, OutputImageType>::New();
        nonlinearReg->SetFixedImage(baselineReader->GetOutput());
        nonlinearReg->SetMovingImage(reg->GetOutput());
        std::cout << "start nonlinear update" << std::endl;
        nonlinearReg->Update();
        std::cout << "nonlinear update done, start writer" << std::endl;

        // Write output image
        writer->SetFileNames(filePaths);
        writer->SetInput(nonlinearReg->GetOutput());
        std::cout << "writer set up" << std::endl;
        writer->Update();
    }
    catch (itk::ExceptionObject e) {
        std::cout << e.GetDescription() << std::endl;
    }

	return 0;
}
