#include <iostream>
#include <vector>
#include <string>
#include <itkImage.h>
#include <itkImageSeriesReader.h>
#include <itkNumericSeriesFileNames.h>

int main(int argc, char **argv) {
    // Define types
    typedef itk::Image<double, 2> ImageType;
    typedef itk::ImageSeriesReader<ImageType> ReaderType;
    typedef itk::NumericSeriesFileNames NameGeneratorType;

    // Define variables
    NameGeneratorType::Pointer nameGenerator = NameGeneratorType::New();
    ReaderType::Pointer reader = ReaderType::New();

    // Accept input or display usage message
    if (argc == 4) {
        nameGenerator->SetSeriesFormat(argv[1]);
        nameGenerator->SetStartIndex(std::stoi(argv[2]));
        nameGenerator->SetEndIndex(std::stoi(argv[3]));
    }
    else {
        std::cout << "USAGE: " << std::endl;
        std::cout << "LungChangeDetector.exe <File Path Template> <Start Index> <End Index>" << std::endl;
        std::cout << "File Path Template -- A standardized file name/path for each numbered image" << std::endl;
        std::cout << "      with \"%d\" standing in for the number, such as \"C:\\foo %d.tif\" for" << std::endl;
        std::cout << "      files foo 1.tif, foo 2.tif, etc." << std::endl;
        std::cout << "Start Index -- Number of the first image." << std::endl;
        std::cout << "End Index -- Number of the last image." << std::endl << std::endl;
        std::cout << "For Example:" << std::endl;
        std::cout << "LungChangeDetector.exe \"C:\\images\\img (%d).tif\" 1 404" << std::endl;
        std::cout << "To load files img (1).tif through img (404).tif" << std::endl;
        return 1;
    }

    // Generate file paths for the reader
    nameGenerator->SetIncrementIndex(1);
    std::vector<std::string> filePaths = nameGenerator->GetFileNames();

    // Load slice image files into memory with series reader.
    reader->SetFileNames(filePaths);
    reader->Update();

	return 0;
}
