#include "itkdetails/ImageUtility.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/epsilon.hpp>

#include <vnl/vnl_matrix_fixed.h>

#include <itkGDCMImageIO.h>
#include <itkFileTools.h>

#include <boost/filesystem.hpp>

#include <iostream>
#include <streambuf>
#include <tuple>
#include <stdio.h>


namespace itkdetails
{

namespace utility
{

std::pair< std::string, bool >
getSpiralCodeFromDirectionMatrix( const vnl_matrix_fixed< double, 3, 3 >& matrix )
{
    /// @note Fourth character is null-terminating character
    char spiralCode[4] = "???";

    /// LPS positive
    static const char codes[3][2] = {
        { 'R', 'L'},
        { 'A', 'P'},
        { 'I', 'S'} };

    bool isOblique = false;

    for ( uint8_t col = 0; col < 3; ++col )
    {
        /// @internal Get the direction cosine for voxel direction \c col
        const vnl_vector_fixed< double, 3 > dirCos = matrix.get_column( col );

        const double dirAbsMax = dirCos.inf_norm();

        for ( uint8_t row = 0; row < 3; ++row )
        {
            /// @note In Convert3D, the assignment of \c dsgn to 1 or 0 is flipped
            const int dsgn = ( dirCos[row] > 0 ) ? 1 : 0;
            const double dabs = std::abs( dirCos[row] );

            if ( glm::epsilonEqual( 1.0, dabs, glm::epsilon<double>() ) )
            {
                spiralCode[col] = codes[row][dsgn];
            }
            else if ( glm::epsilonEqual( dirAbsMax, dabs, glm::epsilon<double>() ) )
            {
                isOblique = true;
                spiralCode[col] = codes[row][dsgn];
            }
        }
    }

    return std::make_pair( std::string{ spiralCode }, isOblique );
}


namespace dicom
{

/**
 * @brief dicomSeriesSearch
 * @param directory
 * @return
 */
/// @internal Identify from a given directory the set of file names that belong together
/// to the same volumetric image. \c GDCMSeriesFileNames will explore the directory and
/// will generate a sequence of filenames for DICOM files for one study/series.
/// The \c GDCMSeriesFileNames object first identifies the list of DICOM series
/// present in the given directory.
///
/// @internal We use additional DICOM information (tag 0008 0021 : DA 1 Series Date)
/// to sub-refine each seriesto distinguish unique volumes within the directory.
/// This is useful, for example, if a DICOM device assigns the same SeriesID to a scout
/// scan and its 3D volume; by using additional DICOM information the scout scan will not be
/// included as part of the 3D volume.
///
/// By default @code{SetUseSeriesDetails(true)} will use the following DICOM tags to sub-refine
/// a set of files into multiple series:
///
/// @begin{description}
/// @item[0020 0011] Series Number
/// @item[0018 0024] Sequence Name
/// @item[0018 0050] Slice Thickness
/// @item[0028 0010] Rows
/// @item[0028 0011] Columns
/// @end{description}
std::pair< std::vector< std::string >, NameGeneratorType::Pointer >
seriesSearch( const char* directory )
{
    /// @internal Directory in which to search for the series
//        const std::string seriesDirectory =
//                ( ::itksys::SystemTools::FileIsDirectory( fileName ) )
//                ? fileName
//                : ::itksys::SystemTools::GetParentDirectory( fileName );

    const std::string seriesDirectory =
            ( boost::filesystem::is_directory( directory ) )
            ? directory
            : boost::filesystem::path( directory ).parent_path().c_str();

    typename NameGeneratorType::Pointer nameGenerator = NameGeneratorType::New();
    nameGenerator->SetUseSeriesDetails( true );
    nameGenerator->AddSeriesRestriction( "0008|0021" ); //!< Series date
    nameGenerator->SetDirectory( seriesDirectory );

    std::vector< std::string > seriesUIDs;

    try
    {
        seriesUIDs = nameGenerator->GetSeriesUIDs();

#if 0
        if ( seriesUIDs.empty() )
        {
            std::cerr << "No DICOM series in directory '"
                      << seriesDirectory << "'" << std::endl;
        }
        else
        {
            std::cout << "The directory '" << seriesDirectory
                      << "' contains the following DICOM series: " << std::endl;

            for ( const auto& i : seriesUIDs )
            {
                std::cout << "\t" << i << std::endl;
            }
            std::cout << std::endl;
        }
#endif
    }
    catch ( const itk::ExceptionObject& e )
    {
        std::cerr << "Exception when generating DICOM series UIDs: "
                  << e.what() << std::endl;

        nameGenerator = ITK_NULLPTR;
    }
    catch ( ... )
    {
        std::cerr << "Exception when generating DICOM series UIDs." << std::endl;

        nameGenerator = ITK_NULLPTR;
    }

    return std::make_pair( seriesUIDs, nameGenerator );
}


/**
  * @return Filenames associated with series
  */
::itk::ImageIOBase::Pointer
createDicomImageIO( const char* firstFileName )
{
    try
    {
        const ::itk::GDCMImageIO::Pointer dicomIO = ::itk::GDCMImageIO::New();

        if ( dicomIO.IsNull() )
        {
            std::cerr << "GDCM cannot create the DICOM I/O object." << std::endl;
            return ITK_NULLPTR;
        }

        dicomIO->SetFileName( firstFileName );
        dicomIO->ReadImageInformation();

        /// @internal Return pointer to base class (\c ImageIOBase)
        const ::itk::ImageIOBase::Pointer imageIO = dicomIO.GetPointer();

        return imageIO;
    }
    catch ( const ::itk::ExceptionObject& e )
    {
        std::cerr << "Exception while creating GDCMImageIO: " << e.what() << std::endl;

        return ITK_NULLPTR;
    }
    catch ( ... )
    {
        std::cerr << "Exception while creating GDCMImageIO." << std::endl;

        return ITK_NULLPTR;
    }
}

} // namespace dicom


/**
 * Delegate creation of I/O object to factory function
 */
::itk::ImageIOBase::Pointer
createStandardImageIO( const char* fileName )
{
    class redirect_stderror
    {
    public:
        redirect_stderror()
            : m_descriptor( dup( fileno(stderr) ) )
        {
            freopen( "/tmp/stderr_log.txt","a", stderr );
        }

        ~redirect_stderror()
        {
            dup2( m_descriptor, fileno(stderr) );
            close( m_descriptor );
        }

        int m_descriptor;
    } redirector;


    try
    {
        const ::itk::ImageIOBase::Pointer imageIO =
                ::itk::ImageIOFactory::CreateImageIO(
                    fileName, ::itk::ImageIOFactory::ReadMode );

        if ( imageIO.IsNull() )
        {
            /// @internal None of the registered ImageIO classes can read the file
            std::cerr << "ITK image I/O factory could not create the I/O object for image '"
                      << fileName << "'" << std::endl;

            return ITK_NULLPTR;
        }

        imageIO->SetFileName( fileName );
        imageIO->ReadImageInformation();

        return imageIO;
    }
    catch ( const ::itk::ExceptionObject& e )
    {
        std::cerr << "Exception while creating ImageIOBase: " << e.what() << std::endl;

        return ITK_NULLPTR;
    }
    catch ( ... )
    {
        std::cerr << "Exception while creating ImageIOBase." << std::endl;

        return ITK_NULLPTR;
    }

    return ITK_NULLPTR;
}


/**
 * @brief Logic needed to determine the image type.
 * Could be extended to support other types, like TIFF and JPEG.
 *
 * @param path
 *
 * @return
 */
ImageFileType getImageFileType( const char* path )
{
    const ::itk::ImageIOBase::Pointer imageIO = createStandardImageIO( path );

    if ( imageIO.IsNotNull() )
    {
        /// @internal Path is a file that ITK's I/O factory can read
        return ImageFileType::SingleImage;
    }
    else
    {
        if ( ! dicom::seriesSearch( path ).first.empty() )
        {
            /// @internal The path is either a directory containing one or more
            /// DICOM series, or the path is a file whose parent directory
            /// contains one or more DICOM series
            return ImageFileType::DICOMSeries;
        }
    }

    return ImageFileType::Undefined;
}

} // namespace utility

} // namespace itkdetails


std::ostream& operator<< ( std::ostream& os, const ::itkdetails::utility::PixelStatistics<double>& stats )
{
    os << "Minimum: "   << stats.m_minimum << std::endl
       << "Maximum: "   << stats.m_maximum << std::endl
       << "Mean: "      << stats.m_mean << std::endl
       << "Std. dev: "  << stats.m_stdDeviation << std::endl
       << "Variance: "  << stats.m_variance << std::endl
       << "Sum: "       << stats.m_sum << std::endl
       << "Quartiles: " << stats.m_quantiles[25] << ", "
                        << stats.m_quantiles[50] << ", "
                        << stats.m_quantiles[75] << std::endl
       << std::endl;

    return os;
}
