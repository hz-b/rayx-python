#include <Beamline/StringConversion.h>
#include <Core.h>
#include <Rml/Importer.h>
#include <Rml/Locate.h>
#include <Tracer/Tracer.h>
#include <Variant.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <concepts>
#include <filesystem>

#include "reflection.hpp"

using namespace pybind11::literals;

std::complex<double> toStdComplex(const rayx::complex::Complex& c) { return std::complex<double>(c.real(), c.imag()); }

std::filesystem::path getModulePath() {
    pybind11::gil_scoped_acquire acquire;
    pybind11::object rayx = pybind11::module::import("rayxdata");
    return std::filesystem::path(rayx.attr("__file__").cast<std::string>()).parent_path().parent_path();
}

namespace reflect {

template <>
struct info<glm::dvec3> {
    static constexpr const char* type_name = "dvec3";
    static constexpr auto fields = std::make_tuple(field_info{&glm::dvec3::x, "x"}, field_info{&glm::dvec3::y, "y"}, field_info{&glm::dvec3::z, "z"});
};

template <>
struct info<glm::dvec4> {
    static constexpr const char* type_name = "dvec4";
    static constexpr auto fields = std::make_tuple(field_info{&glm::dvec4::x, "x"}, field_info{&glm::dvec4::y, "y"}, field_info{&glm::dvec4::z, "z"},
                                                   field_info{&glm::dvec4::w, "w"});
};

template <>
struct info<rayx::detail::SurfaceTypes::Plane> {
    static constexpr const char* type_name = "Plane";
    static constexpr auto fields = std::make_tuple();
};

template <>
struct info<rayx::detail::SurfaceTypes::Quadric> {
    static constexpr const char* type_name = "Quadric";
    static constexpr auto fields = std::make_tuple(
        field_info{&rayx::detail::SurfaceTypes::Quadric::m_icurv, "icurv"}, field_info{&rayx::detail::SurfaceTypes::Quadric::m_a11, "a11"},
        field_info{&rayx::detail::SurfaceTypes::Quadric::m_a12, "a12"}, field_info{&rayx::detail::SurfaceTypes::Quadric::m_a13, "a13"},
        field_info{&rayx::detail::SurfaceTypes::Quadric::m_a14, "a14"}, field_info{&rayx::detail::SurfaceTypes::Quadric::m_a22, "a22"},
        field_info{&rayx::detail::SurfaceTypes::Quadric::m_a23, "a23"}, field_info{&rayx::detail::SurfaceTypes::Quadric::m_a24, "a24"},
        field_info{&rayx::detail::SurfaceTypes::Quadric::m_a33, "a33"}, field_info{&rayx::detail::SurfaceTypes::Quadric::m_a34, "a34"},
        field_info{&rayx::detail::SurfaceTypes::Quadric::m_a44, "a44"});
};

template <>
struct info<rayx::detail::SurfaceTypes::Toroid> {
    static constexpr const char* type_name = "Toroid";
    static constexpr auto fields = std::make_tuple(field_info{&rayx::detail::SurfaceTypes::Toroid::m_longRadius, "longRadius"},
                                                   field_info{&rayx::detail::SurfaceTypes::Toroid::m_shortRadius, "shortRadius"},
                                                   field_info{&rayx::detail::SurfaceTypes::Toroid::m_toroidType, "toroidType"});
};

template <>
struct info<rayx::detail::SurfaceTypes::Cubic> {
    static constexpr const char* type_name = "Cubic";
    static constexpr auto fields =
        std::make_tuple(field_info{&rayx::detail::SurfaceTypes::Cubic::m_a11, "a11"}, field_info{&rayx::detail::SurfaceTypes::Cubic::m_a12, "a12"},
                        field_info{&rayx::detail::SurfaceTypes::Cubic::m_a13, "a13"}, field_info{&rayx::detail::SurfaceTypes::Cubic::m_a14, "a14"},
                        field_info{&rayx::detail::SurfaceTypes::Cubic::m_a22, "a22"}, field_info{&rayx::detail::SurfaceTypes::Cubic::m_a23, "a23"},
                        field_info{&rayx::detail::SurfaceTypes::Cubic::m_a24, "a24"}, field_info{&rayx::detail::SurfaceTypes::Cubic::m_a33, "a33"},
                        field_info{&rayx::detail::SurfaceTypes::Cubic::m_a34, "a34"}, field_info{&rayx::detail::SurfaceTypes::Cubic::m_a44, "a44"},
                        field_info{&rayx::detail::SurfaceTypes::Cubic::m_b12, "b12"}, field_info{&rayx::detail::SurfaceTypes::Cubic::m_b13, "b13"},
                        field_info{&rayx::detail::SurfaceTypes::Cubic::m_b21, "b21"}, field_info{&rayx::detail::SurfaceTypes::Cubic::m_b23, "b23"},
                        field_info{&rayx::detail::SurfaceTypes::Cubic::m_b31, "b31"}, field_info{&rayx::detail::SurfaceTypes::Cubic::m_b32, "b32"},
                        field_info{&rayx::detail::SurfaceTypes::Cubic::m_psi, "psi"});
};

template <>
struct info<rayx::detail::CutoutTypes::Unlimited> {
    static constexpr const char* type_name = "Unlimited";
    static constexpr auto fields = std::make_tuple();
};

template <>
struct info<rayx::detail::CutoutTypes::Rect> {
    static constexpr const char* type_name = "Rect";
    static constexpr auto fields = std::make_tuple(field_info{&rayx::detail::CutoutTypes::Rect::m_width, "width"},
                                                   field_info{&rayx::detail::CutoutTypes::Rect::m_length, "length"});
};

template <>
struct info<rayx::detail::CutoutTypes::Elliptical> {
    static constexpr const char* type_name = "Elliptical";
    static constexpr auto fields = std::make_tuple(field_info{&rayx::detail::CutoutTypes::Elliptical::m_diameter_x, "diameter_x"},
                                                   field_info{&rayx::detail::CutoutTypes::Elliptical::m_diameter_z, "diameter_z"});
};

template <>
struct info<rayx::detail::CutoutTypes::Trapezoid> {
    static constexpr const char* type_name = "Trapezoid";
    static constexpr auto fields = std::make_tuple(field_info{&rayx::detail::CutoutTypes::Trapezoid::m_widthA, "widthA"},
                                                   field_info{&rayx::detail::CutoutTypes::Trapezoid::m_widthB, "widthB"},
                                                   field_info{&rayx::detail::CutoutTypes::Trapezoid::m_length, "length"});
};

template <>
struct info<rayx::SlopeError> {
    static constexpr const char* type_name = "SlopeError";
    static constexpr auto fields = std::make_tuple(field_info{&rayx::SlopeError::m_sag, "sag"}, field_info{&rayx::SlopeError::m_mer, "mer"},
                                                   field_info{&rayx::SlopeError::m_thermalDistortionAmp, "thermalDistortionAmp"},
                                                   field_info{&rayx::SlopeError::m_thermalDistortionSigmaX, "thermalDistortionSigmaX"},
                                                   field_info{&rayx::SlopeError::m_thermalDistortionSigmaZ, "thermalDistortionSigmaZ"},
                                                   field_info{&rayx::SlopeError::m_cylindricalBowingAmp, "cylindricalBowingAmp"},
                                                   field_info{&rayx::SlopeError::m_cylindricalBowingRadius, "cylindricalBowingRadius"});
};

template <>
struct info<rayx::Rad> {
    static constexpr const char* type_name = "Rad";
    static constexpr auto fields = std::make_tuple(field_info{&rayx::Rad::rad, "rad"});
};

template <>
struct info<rayx::Surface> {
    static constexpr const char* type_name = "Surface";
};

template <>
struct info<rayx::Cutout> {
    static constexpr const char* type_name = "Cutout";
};

static_assert(Variant<rayx::Surface>);
static_assert(Variant<rayx::Variant<rayx::detail::SurfaceTypes, rayx::detail::SurfaceTypes::Plane, rayx::detail::SurfaceTypes::Quadric,
                                    rayx::detail::SurfaceTypes::Toroid, rayx::detail::SurfaceTypes::Cubic>>);
static_assert(Variant<rayx::Cutout>);

template <>
struct info<rayx::DesignElement> {
    static constexpr const char* type_name = "DesignElement";
    static constexpr auto fields = std::make_tuple(
        prop_info{&rayx::DesignElement::getName, &rayx::DesignElement::setName, "name"},
        prop_info{&rayx::DesignElement::getType, &rayx::DesignElement::setType, "type"},
        prop_info{&rayx::DesignElement::getPosition, &rayx::DesignElement::setPosition, "position"},
        prop_info{&rayx::DesignElement::getOrientation, &rayx::DesignElement::setOrientation, "orientation"},
        prop_info{&rayx::DesignElement::getSlopeError, &rayx::DesignElement::setSlopeError, "slopeError"},
        prop_info{&rayx::DesignElement::getAzimuthalAngle, &rayx::DesignElement::setAzimuthalAngle, "azimuthalAngle"},
        prop_info{&rayx::DesignElement::getMaterial, &rayx::DesignElement::setMaterial, "material"},
        prop_info{&rayx::DesignElement::getCutout, &rayx::DesignElement::setCutout, "cutout"},
        // prop_info{&rayx::DesignElement::getVLSParameters, &rayx::DesignElement::setVLSParameters, "vlsParameters"},
        prop_info{&rayx::DesignElement::getExpertsOptics, &rayx::DesignElement::setExpertsOptics, "expertsOptics"},
        prop_info{&rayx::DesignElement::getExpertsCubic, &rayx::DesignElement::setExpertsCubic, "expertsCubic"},
        prop_info{&rayx::DesignElement::getDistancePreceding, &rayx::DesignElement::setDistancePreceding, "distancePreceding"},
        prop_info{&rayx::DesignElement::getTotalHeight, &rayx::DesignElement::setTotalHeight, "totalHeight"},
        prop_info{&rayx::DesignElement::getOpeningShape, &rayx::DesignElement::setOpeningShape, "openingShape"},
        prop_info{&rayx::DesignElement::getOpeningWidth, &rayx::DesignElement::setOpeningWidth, "openingWidth"},
        prop_info{&rayx::DesignElement::getOpeningHeight, &rayx::DesignElement::setOpeningHeight, "openingHeight"},
        prop_info{&rayx::DesignElement::getCentralBeamstop, &rayx::DesignElement::setCentralBeamstop, "centralBeamstop"},
        prop_info{&rayx::DesignElement::getStopWidth, &rayx::DesignElement::setStopWidth, "stopWidth"},
        prop_info{&rayx::DesignElement::getStopHeight, &rayx::DesignElement::setStopHeight, "stopHeight"},
        prop_info{&rayx::DesignElement::getTotalWidth, &rayx::DesignElement::setTotalWidth, "totalWidth"},
        prop_info{&rayx::DesignElement::getProfileKind, &rayx::DesignElement::setProfileKind, "profileKind"},
        prop_info{&rayx::DesignElement::getProfileFile, &rayx::DesignElement::setProfileFile, "profileFile"},
        prop_info{&rayx::DesignElement::getTotalLength, &rayx::DesignElement::setTotalLength, "totalLength"},
        prop_info{&rayx::DesignElement::getGrazingIncAngle, &rayx::DesignElement::setGrazingIncAngle, "grazingIncAngle"},
        prop_info{&rayx::DesignElement::getDeviationAngle, &rayx::DesignElement::setDeviationAngle, "deviationAngle"},
        prop_info{&rayx::DesignElement::getEntranceArmLength, &rayx::DesignElement::setEntranceArmLength, "entranceArmLength"},
        prop_info{&rayx::DesignElement::getExitArmLength, &rayx::DesignElement::setExitArmLength, "exitArmLength"},
        prop_info{&rayx::DesignElement::getRadiusDirection, &rayx::DesignElement::setRadiusDirection, "radiusDirection"},
        prop_info{&rayx::DesignElement::getRadius, &rayx::DesignElement::setRadius, "radius"},
        prop_info{&rayx::DesignElement::getDesignGrazingIncAngle, &rayx::DesignElement::setDesignGrazingIncAngle, "designGrazingIncAngle"},
        prop_info{&rayx::DesignElement::getLongHalfAxisA, &rayx::DesignElement::setLongHalfAxisA, "longHalfAxisA"},
        prop_info{&rayx::DesignElement::getShortHalfAxisB, &rayx::DesignElement::setShortHalfAxisB, "shortHalfAxisB"},
        prop_info{&rayx::DesignElement::getParameterA11, &rayx::DesignElement::setParameterA11, "parameterA11"},
        prop_info{&rayx::DesignElement::getFigureRotation, &rayx::DesignElement::setFigureRotation, "figureRotation"},
        prop_info{&rayx::DesignElement::getArmLength, &rayx::DesignElement::setArmLength, "armLength"},
        prop_info{&rayx::DesignElement::getParameterP, &rayx::DesignElement::setParameterP, "parameterP"},
        prop_info{&rayx::DesignElement::getParameterPType, &rayx::DesignElement::setParameterPType, "parameterPType"},
        prop_info{&rayx::DesignElement::getLineDensity, &rayx::DesignElement::setLineDensity, "lineDensity"},
        prop_info{&rayx::DesignElement::getShortRadius, &rayx::DesignElement::setShortRadius, "shortRadius"},
        prop_info{&rayx::DesignElement::getLongRadius, &rayx::DesignElement::setLongRadius, "longRadius"},
        prop_info{&rayx::DesignElement::getFresnelZOffset, &rayx::DesignElement::setFresnelZOffset, "fresnelZOffset"},
        prop_info{&rayx::DesignElement::getDesignAlphaAngle, &rayx::DesignElement::setDesignAlphaAngle, "designAlphaAngle"},
        prop_info{&rayx::DesignElement::getDesignBetaAngle, &rayx::DesignElement::setDesignBetaAngle, "designBetaAngle"},
        prop_info{&rayx::DesignElement::getDesignOrderOfDiffraction, &rayx::DesignElement::setDesignOrderOfDiffraction, "designOrderOfDiffraction"},
        prop_info{&rayx::DesignElement::getDesignEnergy, &rayx::DesignElement::setDesignEnergy, "designEnergy"},
        prop_info{&rayx::DesignElement::getDesignSagittalEntranceArmLength, &rayx::DesignElement::setDesignSagittalEntranceArmLength,
                  "designSagittalEntranceArmLength"},
        prop_info{&rayx::DesignElement::getDesignSagittalExitArmLength, &rayx::DesignElement::setDesignSagittalExitArmLength,
                  "designSagittalExitArmLength"},
        prop_info{&rayx::DesignElement::getDesignMeridionalEntranceArmLength, &rayx::DesignElement::setDesignMeridionalEntranceArmLength,
                  "designMeridionalEntranceArmLength"},
        prop_info{&rayx::DesignElement::getDesignMeridionalExitArmLength, &rayx::DesignElement::setDesignMeridionalExitArmLength,
                  "designMeridionalExitArmLength"},
        prop_info{&rayx::DesignElement::getOrderOfDiffraction, &rayx::DesignElement::setOrderOfDiffraction, "orderOfDiffraction"},
        prop_info{&rayx::DesignElement::getAdditionalOrder, &rayx::DesignElement::setAdditionalOrder, "additionalOrder"},
        prop_info{&rayx::DesignElement::getImageType, &rayx::DesignElement::setImageType, "imageType"},
        prop_info{&rayx::DesignElement::getCurvatureType, &rayx::DesignElement::setCurvatureType, "curvatureType"},
        prop_info{&rayx::DesignElement::getBehaviourType, &rayx::DesignElement::setBehaviourType, "behaviourType"},
        prop_info{&rayx::DesignElement::getCrystalType, &rayx::DesignElement::setCrystalType, "crystalType"},
        // prop_info{&rayx::DesignElement::getCrystalMaterial, &rayx::DesignElement::setCrystalMaterial, "crystalMaterial"},
        prop_info{&rayx::DesignElement::getOffsetAngle, &rayx::DesignElement::setOffsetAngle, "offsetAngle"},
        prop_info{&rayx::DesignElement::getStructureFactorReF0, &rayx::DesignElement::setStructureFactorReF0, "structureFactorReF0"},
        prop_info{&rayx::DesignElement::getStructureFactorImF0, &rayx::DesignElement::setStructureFactorImF0, "structureFactorImF0"},
        prop_info{&rayx::DesignElement::getStructureFactorReFH, &rayx::DesignElement::setStructureFactorReFH, "structureFactorReFH"},
        prop_info{&rayx::DesignElement::getStructureFactorImFH, &rayx::DesignElement::setStructureFactorImFH, "structureFactorImFH"},
        prop_info{&rayx::DesignElement::getStructureFactorReFHC, &rayx::DesignElement::setStructureFactorReFHC, "structureFactorReFHC"},
        prop_info{&rayx::DesignElement::getStructureFactorImFHC, &rayx::DesignElement::setStructureFactorImFHC, "structureFactorImFHC"},
        prop_info{&rayx::DesignElement::getUnitCellVolume, &rayx::DesignElement::setUnitCellVolume, "unitCellVolume"},
        prop_info{&rayx::DesignElement::getDSpacing2, &rayx::DesignElement::setDSpacing2, "dSpacing2"},
        prop_info{&rayx::DesignElement::getThicknessSubstrate, &rayx::DesignElement::setThicknessSubstrate, "thicknessSubstrate"},
        prop_info{&rayx::DesignElement::getRoughnessSubstrate, &rayx::DesignElement::setRoughnessSubstrate, "roughnessSubstrate"},
        prop_info{&rayx::DesignElement::getDesignPlane, &rayx::DesignElement::setDesignPlane, "designPlane"},
        prop_info{&rayx::DesignElement::getSurfaceCoatingType, &rayx::DesignElement::setSurfaceCoatingType, "surfaceCoatingType"},
        prop_info{&rayx::DesignElement::getMaterialCoating, &rayx::DesignElement::setMaterialCoating, "materialCoating"},
        prop_info{&rayx::DesignElement::getThicknessCoating, &rayx::DesignElement::setThicknessCoating, "thicknessCoating"},
        prop_info{&rayx::DesignElement::getRoughnessCoating, &rayx::DesignElement::setRoughnessCoating, "roughnessCoating"});
};

static_assert(Structure<rayx::DesignElement>);

template <>
struct info<rayx::DesignSource> {
    static constexpr const char* type_name = "Source";

    static constexpr auto fields = std::make_tuple(
        prop_info{&rayx::DesignSource::getName, &rayx::DesignSource::setName, "name"},
        prop_info{&rayx::DesignSource::getType, &rayx::DesignSource::setType, "type"},
        prop_info{&rayx::DesignSource::getWidthDist, &rayx::DesignSource::setWidthDist, "widthDist"},
        prop_info{&rayx::DesignSource::getHeightDist, &rayx::DesignSource::setHeightDist, "heightDist"},
        prop_info{&rayx::DesignSource::getHorDist, &rayx::DesignSource::setHorDist, "horDist"},
        prop_info{&rayx::DesignSource::getVerDist, &rayx::DesignSource::setVerDist, "verDist"},
        prop_info{&rayx::DesignSource::getHorDivergence, &rayx::DesignSource::setHorDivergence, "horDivergence"},
        prop_info{&rayx::DesignSource::getVerDivergence, &rayx::DesignSource::setVerDivergence, "verDivergence"},
        prop_info{&rayx::DesignSource::getVerEBeamDivergence, &rayx::DesignSource::setVerEBeamDivergence, "verEBeamDivergence"},
        prop_info{&rayx::DesignSource::getSourceDepth, &rayx::DesignSource::setSourceDepth, "sourceDepth"},
        prop_info{&rayx::DesignSource::getSourceHeight, &rayx::DesignSource::setSourceHeight, "sourceHeight"},
        prop_info{&rayx::DesignSource::getSourceWidth, &rayx::DesignSource::setSourceWidth, "sourceWidth"},
        prop_info{&rayx::DesignSource::getBendingRadius, &rayx::DesignSource::setBendingRadius, "bendingRadius"},
        prop_info{&rayx::DesignSource::getEnergySpread, &rayx::DesignSource::setEnergySpread, "energySpread"},
        prop_info{&rayx::DesignSource::getEnergySpreadType, &rayx::DesignSource::setEnergySpreadType, "energySpreadType"},
        prop_info{&rayx::DesignSource::getEnergyDistributionType, &rayx::DesignSource::setEnergyDistributionType, "energyDistributionType"},
        prop_info{&rayx::DesignSource::getEnergySpreadUnit, &rayx::DesignSource::setEnergySpreadUnit, "energySpreadUnit"},
        prop_info{&rayx::DesignSource::getElectronEnergy, &rayx::DesignSource::setElectronEnergy, "electronEnergy"},
        prop_info{&rayx::DesignSource::getElectronEnergyOrientation, &rayx::DesignSource::setElectronEnergyOrientation, "electronEnergyOrientation"},
        prop_info{&rayx::DesignSource::getNumberOfSeparateEnergies, &rayx::DesignSource::setNumberOfSeparateEnergies, "numberOfSeparateEnergies"},
        prop_info{&rayx::DesignSource::getEnergy, &rayx::DesignSource::setEnergy, "energy"},
        prop_info{&rayx::DesignSource::getPhotonFlux, &rayx::DesignSource::setPhotonFlux, "photonFlux"},
        prop_info{&rayx::DesignSource::getNumberOfRays, &rayx::DesignSource::setNumberOfRays, "numberOfRays"},
        prop_info{&rayx::DesignSource::getPosition, &rayx::DesignSource::setPosition, "position"},
        prop_info{&rayx::DesignSource::getOrientation, &rayx::DesignSource::setOrientation, "orientation"},
        prop_info{&rayx::DesignSource::getNumOfCircles, &rayx::DesignSource::setNumOfCircles, "numOfCircles"},
        prop_info{&rayx::DesignSource::getMaxOpeningAngle, &rayx::DesignSource::setMaxOpeningAngle, "maxOpeningAngle"},
        prop_info{&rayx::DesignSource::getMinOpeningAngle, &rayx::DesignSource::setMinOpeningAngle, "minOpeningAngle"},
        prop_info{&rayx::DesignSource::getDeltaOpeningAngle, &rayx::DesignSource::setDeltaOpeningAngle, "deltaOpeningAngle"},
        prop_info{&rayx::DesignSource::getSigmaType, &rayx::DesignSource::setSigmaType, "sigmaType"},
        prop_info{&rayx::DesignSource::getUndulatorLength, &rayx::DesignSource::setUndulatorLength, "undulatorLength"},
        prop_info{&rayx::DesignSource::getElectronSigmaX, &rayx::DesignSource::setElectronSigmaX, "electronSigmaX"},
        prop_info{&rayx::DesignSource::getElectronSigmaXs, &rayx::DesignSource::setElectronSigmaXs, "electronSigmaXs"},
        prop_info{&rayx::DesignSource::getElectronSigmaY, &rayx::DesignSource::setElectronSigmaY, "electronSigmaY"},
        prop_info{&rayx::DesignSource::getElectronSigmaYs, &rayx::DesignSource::setElectronSigmaYs, "electronSigmaYs"});
    // prop_info{&rayx::DesignSource::getRayList, &rayx::DesignSource::setRayList, "rayList"});
};

static_assert(Structure<rayx::DesignSource>);

}  // namespace reflect

template <typename T>
struct data_buffer {
    std::vector<T> data;

    pybind11::buffer_info buffer_info() { return pybind11::buffer_info(data.data(), data.size()); }

    pybind11::object as_numpy_array() const {
        pybind11::gil_scoped_acquire acquire;
        pybind11::object np = pybind11::module::import("numpy");
        return np.attr("array")(this, "copy"_a = false);
    }
};

struct Rays {
    data_buffer<double> position_x;
    data_buffer<double> position_y;
    data_buffer<double> position_z;
    data_buffer<double> direction_x;
    data_buffer<double> direction_y;
    data_buffer<double> direction_z;
    data_buffer<double> energy;
    data_buffer<std::complex<double>> electric_field_x;
    data_buffer<std::complex<double>> electric_field_y;
    data_buffer<std::complex<double>> electric_field_z;
    data_buffer<double> path_length;
    data_buffer<int32_t> order;
    data_buffer<rayx::EventType> event_type;
    data_buffer<int32_t> last_element_id;
    data_buffer<int32_t> source_id;
    data_buffer<int32_t> ray_id;
    data_buffer<int32_t> path_event_id;

    Rays(rayx::Rays&& rays) {
        position_x.data = std::move(rays.position_x);
        position_y.data = std::move(rays.position_y);
        position_z.data = std::move(rays.position_z);
        direction_x.data = std::move(rays.direction_x);
        direction_y.data = std::move(rays.direction_y);
        direction_z.data = std::move(rays.direction_z);
        energy.data = std::move(rays.energy);
        electric_field_x.data.resize(rays.electric_field_x.size());
        electric_field_y.data.resize(rays.electric_field_y.size());
        electric_field_z.data.resize(rays.electric_field_z.size());
        for (size_t i = 0; i < rays.electric_field_x.size(); i++) {
            electric_field_x.data[i] = toStdComplex(rays.electric_field_x[i]);
            electric_field_y.data[i] = toStdComplex(rays.electric_field_y[i]);
            electric_field_z.data[i] = toStdComplex(rays.electric_field_z[i]);
        }
        path_length.data = std::move(rays.optical_path_length);
        order.data = std::move(rays.order);
        event_type.data = std::move(rays.event_type);
        last_element_id.data = std::move(rays.object_id);
        source_id.data = std::move(rays.source_id);
        ray_id.data = std::move(rays.path_id);
        path_event_id.data = std::move(rays.path_event_id);
    }
};

class Module {
  public:
    Module() {
        std::filesystem::path data_dir = getModulePath();
        rayx::ResourceHandler::getInstance().addLookUpPath(data_dir);
    }
};

PYBIND11_MODULE(_core, m) {
    static Module module_instance;

    m.doc() = "rayx module";

    m.def("get_module_path", []() { return getModulePath().string(); }, "Get the path to the rayx module");

    // element.cutout.width = 10.0

    reflect::register_type<rayx::DesignElement>(m);
    reflect::register_type<rayx::DesignSource>(m);

    pybind11::enum_<rayx::Material>(m, "Material")
        .value("VACUUM", rayx::Material::VACUUM)
        .value("REFLECTIVE", rayx::Material::REFLECTIVE)
#define X(e, z, a, rho) .value(#e, rayx::Material::e)
#include <Material/materials.xmacro>
#undef X
        .export_values();

    pybind11::enum_<rayx::SourceDist>(m, "SourceDist")
        .value("UNIFORM", rayx::SourceDist::Uniform)
        .value("GAUSSIAN", rayx::SourceDist::Gaussian)
        .value("THIRDS", rayx::SourceDist::Thirds)
        .value("CIRCLE", rayx::SourceDist::Circle)
        .export_values();

    pybind11::enum_<rayx::SpreadType>(m, "SpreadType")
        .value("HARD_EDGE", rayx::SpreadType::HardEdge)
        .value("SOFT_EDGE", rayx::SpreadType::SoftEdge)
        .value("SEPARATE_ENERGIES", rayx::SpreadType::SeparateEnergies)
        .export_values();

    pybind11::enum_<rayx::EnergyDistributionType>(m, "EnergyDistributionType")
        .value("FILE", rayx::EnergyDistributionType::File)
        .value("VALUES", rayx::EnergyDistributionType::Values)
        .value("TOTAL", rayx::EnergyDistributionType::Total)
        .value("PARAM", rayx::EnergyDistributionType::Param)
        .export_values();

    pybind11::enum_<rayx::EnergySpreadUnit>(m, "EnergySpreadUnit")
        .value("EV", rayx::EnergySpreadUnit::EU_eV)
        .value("PERCENT", rayx::EnergySpreadUnit::EU_PERCENT)
        .export_values();

    pybind11::enum_<rayx::ElectronEnergyOrientation>(m, "ElectronEnergyOrientation")
        .value("Clockwise", rayx::ElectronEnergyOrientation::Clockwise)
        .value("Counterclockwise", rayx::ElectronEnergyOrientation::Counterclockwise)
        .export_values();

    pybind11::enum_<rayx::ElementType>(m, "ElementType")
        .value("UNDEFINED", rayx::ElementType::Undefined)
        .value("IMAGE_PLANE", rayx::ElementType::ImagePlane)
        .value("CONE_MIRROR", rayx::ElementType::ConeMirror)
        .value("CRYSTAL", rayx::ElementType::Crystal)
        .value("CYLINDRICAL_MIRROR", rayx::ElementType::CylinderMirror)
        .value("ELLIPSOID_MIRROR", rayx::ElementType::EllipsoidMirror)
        .value("EXPERTS_MIRROR", rayx::ElementType::ExpertsMirror)
        .value("FOIL", rayx::ElementType::Foil)
        .value("PARABOLOID_MIRROR", rayx::ElementType::ParaboloidMirror)
        .value("PLANE_GRATING", rayx::ElementType::PlaneGrating)
        .value("PLANE_MIRROR", rayx::ElementType::PlaneMirror)
        .value("REFLECTION_ZONEPLATE", rayx::ElementType::ReflectionZoneplate)
        .value("SLIT", rayx::ElementType::Slit)
        .value("SPHERE_GRATING", rayx::ElementType::SphereGrating)
        .value("SPHERE", rayx::ElementType::Sphere)
        .value("SPHERE_MIRROR", rayx::ElementType::SphereMirror)
        .value("TOROID_MIRROR", rayx::ElementType::ToroidMirror)
        .value("TOROID_GRATING", rayx::ElementType::ToroidGrating)
        .value("POINT_SOURCE", rayx::ElementType::PointSource)
        .value("MATRIX_SOURCE", rayx::ElementType::MatrixSource)
        .value("DIPOLE_SOURCE", rayx::ElementType::DipoleSource)
        .value("PIXEL_SOURCE", rayx::ElementType::PixelSource)
        .value("CIRCLE_SOURCE", rayx::ElementType::CircleSource)
        .value("SIMPLE_UNDULATOR_SOURCE", rayx::ElementType::SimpleUndulatorSource)
        .value("RAY_LIST_SOURCE", rayx::ElementType::RayListSource)
        .export_values();

    pybind11::enum_<rayx::EventType>(m, "EventType")
        .value("UNINITIALIZED", rayx::EventType::Uninitialized)
        .value("EMITTED", rayx::EventType::Emitted)
        .value("HIT_ELEMENT", rayx::EventType::HitElement)
        .value("FATAL_ERROR", rayx::EventType::FatalError)
        .value("ABSORBED", rayx::EventType::Absorbed)
        .value("BEYOND_HORIZON", rayx::EventType::BeyondHorizon)
        .value("TOO_MANY_EVENTS", rayx::EventType::TooManyEvents)
        .export_values();

    pybind11::class_<data_buffer<double>>(m, "Array[double]", pybind11::buffer_protocol())
        .def_buffer([](data_buffer<double>& db) { return db.buffer_info(); })
        .def("as_numpy", &data_buffer<double>::as_numpy_array);
    pybind11::class_<data_buffer<std::complex<double>>>(m, "Array[complex[double]]", pybind11::buffer_protocol())
        .def_buffer([](data_buffer<std::complex<double>>& db) { return db.buffer_info(); })
        .def("as_numpy", &data_buffer<std::complex<double>>::as_numpy_array);
    pybind11::class_<data_buffer<int>>(m, "Array[int]", pybind11::buffer_protocol())
        .def_buffer([](data_buffer<int>& db) { return db.buffer_info(); })
        .def("as_numpy", &data_buffer<int>::as_numpy_array);
    pybind11::class_<data_buffer<rayx::EventType>>(m, "Array[event_type]", pybind11::buffer_protocol())
        .def_buffer([](data_buffer<rayx::EventType>& db) { return db.buffer_info(); })
        .def("as_numpy", &data_buffer<rayx::EventType>::as_numpy_array);

    pybind11::class_<Rays>(m, "Rays")
        .def_property_readonly("position_x", [](const Rays& r) { return r.position_x.as_numpy_array(); })
        .def_property_readonly("position_y", [](const Rays& r) { return r.position_y.as_numpy_array(); })
        .def_property_readonly("position_z", [](const Rays& r) { return r.position_z.as_numpy_array(); })
        .def_property_readonly("direction_x", [](const Rays& r) { return r.direction_x.as_numpy_array(); })
        .def_property_readonly("direction_y", [](const Rays& r) { return r.direction_y.as_numpy_array(); })
        .def_property_readonly("direction_z", [](const Rays& r) { return r.direction_z.as_numpy_array(); })
        .def_property_readonly("energy", [](const Rays& r) { return r.energy.as_numpy_array(); })
        .def_property_readonly("electric_field_x", [](const Rays& r) { return r.electric_field_x.as_numpy_array(); })
        .def_property_readonly("electric_field_y", [](const Rays& r) { return r.electric_field_y.as_numpy_array(); })
        .def_property_readonly("electric_field_z", [](const Rays& r) { return r.electric_field_z.as_numpy_array(); })
        .def_property_readonly("path_length", [](const Rays& r) { return r.path_length.as_numpy_array(); })
        .def_property_readonly("order", [](const Rays& r) { return r.order.as_numpy_array(); })
        .def_property_readonly("event_type", [](const Rays& r) { return r.event_type.as_numpy_array(); })
        .def_property_readonly("last_element_id", [](const Rays& r) { return r.last_element_id.as_numpy_array(); })
        .def_property_readonly("source_id", [](const Rays& r) { return r.source_id.as_numpy_array(); })
        .def_property_readonly("ray_id", [](const Rays& r) { return r.ray_id.as_numpy_array(); })
        .def_property_readonly("path_event_id", [](const Rays& r) { return r.path_event_id.as_numpy_array(); });

    pybind11::class_<rayx::Beamline>(m, "Beamline")
        .def_property_readonly("elements", &rayx::Beamline::getElements)
        .def_property_readonly("sources", &rayx::Beamline::getSources)
        .def("trace",
             [](rayx::Beamline& bl) {
                 rayx::DeviceConfig deviceConfig = rayx::DeviceConfig().enableBestDevice();
                 rayx::Tracer tracer = rayx::Tracer(deviceConfig);
                 rayx::ObjectMask obj_mask = rayx::ObjectMask::all();
                 rayx::RayAttrMask attr_mask = rayx::RayAttrMask::All;
                 rayx::Rays rays = tracer.trace(bl, rayx::Sequential::No, obj_mask, attr_mask, std::nullopt, std::nullopt);
                 return Rays(std::move(rays));
             })
        .def("__getitem__", [](rayx::Beamline& bl, const std::string& name) {
            for (auto element : bl.getElements()) {
                if (element->getName() == name) {
                    return pybind11::cast(element);
                }
            }
            for (auto source : bl.getSources()) {
                if (source->getName() == name) {
                    return pybind11::cast(source);
                }
            }
            throw std::runtime_error("No element or source with name '" + name + "' found in beamline.");
        });

    m.def(
        "import_beamline", [](std::string path) { return rayx::importBeamline(path); }, "Import a beamline from an RML file", pybind11::arg("path"));
}
