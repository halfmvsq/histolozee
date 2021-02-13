#ifndef IMAGEIO_IMAGE_LOADER_H
#define IMAGEIO_IMAGE_LOADER_H

#include "HZeeTypes.hpp"

#include <memory>
#include <optional>
#include <string>
#include <vector>


namespace imageio
{

class ImageDataFactory;
class ImageCpuRecord;
class ImageTransformations;
class ParcellationCpuRecord;


class ImageLoader
{
public:

    /**
     * @brief Register all known pixel component types with the \c ImageData<T> factory.
     * There is variadic template magic that could make this code more elegant.
     */
    ImageLoader( const ComponentTypeCastPolicy& castPolicy = ComponentTypeCastPolicy::Identity );

    ImageLoader( const ImageLoader& ) = delete;
    ImageLoader& operator=( const ImageLoader& ) = delete;

    ImageLoader( ImageLoader&& ) = delete;
    ImageLoader& operator=( ImageLoader&& ) = delete;

    ~ImageLoader();


    /**
     * @brief Load an image into this class and get back the UID used to identify it with
     * the class. An optional series UID can be supplied for DICOM images.
     *
     * @param[in] inputFileName Input image file name
     * @param[in] inputDicomSeriesUID Optional input DICOM series UID
     * @param[in] normalizationPolicy Policy for normalizing components
     *
     * @return Boolean flag equal to true iff the image was successfully loaded
     */
    std::unique_ptr<ImageCpuRecord> load(
            const std::string& inputFileName,
            const std::optional<std::string>& inputDicomSeriesUID,
            const ComponentNormalizationPolicy& normalizationPolicy ) const;


    /**
     * @brief generateClearParcellationRecord
     *
     * @param[in] sourceRecord
     *
     * @return
     */
    std::unique_ptr<ParcellationCpuRecord>
    generateClearParcellationRecord( const ImageCpuRecord* sourceRecord ) const;


private:

    /**
     * @brief Perform the actual loading of an image, which is potentially spread
     * across multiple input files.
     *
     * @param[in] fileNames Input image file names
     * @param[in] isDicom Flag equal to true iff the image is DICOM
     * @param[in] normalizationPolicy Policy for normalizing components
     *
     * @return Boolean flag equal to true iff the image was successfully loaded
     */
    std::unique_ptr<ImageCpuRecord> doLoadFiles(
            const std::vector< std::string >& fileNames,
            bool isDicom,
            const ComponentNormalizationPolicy& normalizationPolicy ) const;


    std::unique_ptr<ImageDataFactory> m_imageDataFactory;
};

} // namespace imageio

#endif // IMAGEIO_IMAGE_LOADER_H
