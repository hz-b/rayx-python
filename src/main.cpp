#include <Beamline/StringConversion.h>
#include <Core.h>
#include <Rml/Importer.h>
#include <Rml/Locate.h>
#include <Tracer/Tracer.h>
#include <Variant.h>
#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include <concepts>
#include <filesystem>

#include "reflection.hpp"

std::complex<double> toStdComplex(const rayx::complex::Complex& c) { return std::complex<double>(c.real(), c.imag()); }

std::filesystem::path getModulePath(py::module_ m) {
    py::gil_scoped_acquire acquire;
    return std::filesystem::path(py::cast<std::string>(m.attr("__file__"))).parent_path();
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
        prop_info{&rayx::DesignElement::getCrystalMaterial, &rayx::DesignElement::setCrystalMaterial, "crystalMaterial"},
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

// TODO: dmat4x4
// TODO: rays struct
// TODO: LayerCoating
// TODO: CurvatureType

template <>
struct py::detail::dtype_traits<rayx::EventType> {
    static constexpr dlpack::dtype value = py::detail::dtype_traits<uint32_t>::value;
    static constexpr auto name = py::detail::dtype_traits<uint32_t>::name;
};

template <typename T>
py::ndarray<py::numpy, T, py::ndim<1>> to_numpy(std::vector<T>& v) {
    return py::ndarray<py::numpy, T, py::ndim<1>>(v.data(), {v.size()});
}

NB_MODULE(core, m) {
    std::filesystem::path module_path = getModulePath(m);
    rayx::ResourceHandler::getInstance().addLookUpPath(module_path);

    m.doc() = "rayx module";

    m.def("get_module_path", [=]() { return module_path.string(); }, "Get the path to the rayx module");

    reflect::register_type<rayx::DesignElement>(m);
    reflect::register_type<rayx::DesignSource>(m);

    py::enum_<rayx::Material>(m, "Material").value("VACUUM", rayx::Material::VACUUM).value("REFLECTIVE", rayx::Material::REFLECTIVE)
#define X(e, z, a, rho) .value(#e, rayx::Material::e)
#include <Material/materials.xmacro>
#undef X
        ;
    py::enum_<rayx::SourceDist>(m, "SourceDist")
        .value("UNIFORM", rayx::SourceDist::Uniform)
        .value("GAUSSIAN", rayx::SourceDist::Gaussian)
        .value("THIRDS", rayx::SourceDist::Thirds)
        .value("CIRCLE", rayx::SourceDist::Circle);

    py::enum_<rayx::SpreadType>(m, "SpreadType")
        .value("HARD_EDGE", rayx::SpreadType::HardEdge)
        .value("SOFT_EDGE", rayx::SpreadType::SoftEdge)
        .value("SEPARATE_ENERGIES", rayx::SpreadType::SeparateEnergies);

    py::enum_<rayx::EnergyDistributionType>(m, "EnergyDistributionType")
        .value("FILE", rayx::EnergyDistributionType::File)
        .value("VALUES", rayx::EnergyDistributionType::Values)
        .value("TOTAL", rayx::EnergyDistributionType::Total)
        .value("PARAM", rayx::EnergyDistributionType::Param);

    py::enum_<rayx::EnergySpreadUnit>(m, "EnergySpreadUnit")
        .value("EV", rayx::EnergySpreadUnit::EU_eV)
        .value("PERCENT", rayx::EnergySpreadUnit::EU_PERCENT);

    py::enum_<rayx::ElectronEnergyOrientation>(m, "ElectronEnergyOrientation")
        .value("Clockwise", rayx::ElectronEnergyOrientation::Clockwise)
        .value("Counterclockwise", rayx::ElectronEnergyOrientation::Counterclockwise);

    py::enum_<rayx::ToroidType>(m, "ToroidType").value("Convex", rayx::ToroidType::Convex).value("Concave", rayx::ToroidType::Concave);

    py::enum_<rayx::CutoutType>(m, "CutoutType")
        .value("Unlimited", rayx::CutoutType::Unlimited)
        .value("Rect", rayx::CutoutType::Rect)
        .value("Trapezoid", rayx::CutoutType::Trapezoid)
        .value("Elliptical", rayx::CutoutType::Elliptical);

    py::enum_<rayx::CentralBeamstop>(m, "CentralBeamstop")
        .value("None", rayx::CentralBeamstop::None)
        .value("Rectangle", rayx::CentralBeamstop::Rectangle)
        .value("Elliptical", rayx::CentralBeamstop::Elliptical);

    py::enum_<rayx::CylinderDirection>(m, "CylinderDirection")
        .value("LongRadiusR", rayx::CylinderDirection::LongRadiusR)
        .value("ShortRadiusRho", rayx::CylinderDirection::ShortRadiusRho);

    py::enum_<rayx::FigureRotation>(m, "FigureRotation")
        .value("Yes", rayx::FigureRotation::Yes)
        .value("Plane", rayx::FigureRotation::Plane)
        .value("A11", rayx::FigureRotation::A11);

    py::enum_<rayx::CurvatureType>(m, "CurvatureType")
        .value("Plane", rayx::CurvatureType::Plane)
        .value("Toroidal", rayx::CurvatureType::Toroidal)
        .value("Spherical", rayx::CurvatureType::Spherical)
        .value("Cubic", rayx::CurvatureType::Cubic)
        .value("Cone", rayx::CurvatureType::Cone)
        .value("Cylinder", rayx::CurvatureType::Cylinder)
        .value("Ellipsoid", rayx::CurvatureType::Ellipsoid)
        .value("Paraboloid", rayx::CurvatureType::Paraboloid)
        .value("Quadric", rayx::CurvatureType::Quadric)
        .value("RzpSphere", rayx::CurvatureType::RzpSphere);

    py::enum_<rayx::DesignPlane>(m, "DesignPlane").value("XY", rayx::DesignPlane::XY).value("XZ", rayx::DesignPlane::XZ);

    py::enum_<rayx::BehaviourType>(m, "BehaviourType")
        .value("Mirror", rayx::BehaviourType::Mirror)
        .value("Grating", rayx::BehaviourType::Grating)
        .value("Slit", rayx::BehaviourType::Slit)
        .value("Rzp", rayx::BehaviourType::Rzp)
        .value("ImagePlane", rayx::BehaviourType::ImagePlane)
        .value("Crystal", rayx::BehaviourType::Crystal)
        .value("Foil", rayx::BehaviourType::Foil);

    py::enum_<rayx::SurfaceCoatingType>(m, "SurfaceCoatingType")
        .value("SubstrateOnly", rayx::SurfaceCoatingType::SubstrateOnly)
        .value("OneCoating", rayx::SurfaceCoatingType::OneCoating)
        .value("MultipleCoatings", rayx::SurfaceCoatingType::MultipleCoatings);

    py::enum_<rayx::SigmaType>(m, "SigmaType").value("Standard", rayx::SigmaType::ST_STANDARD).value("Accurate", rayx::SigmaType::ST_ACCURATE);

    py::enum_<rayx::ElementType>(m, "ElementType")
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
        .value("RAY_LIST_SOURCE", rayx::ElementType::RayListSource);

    py::enum_<rayx::EventType>(m, "EventType")
        .value("UNINITIALIZED", rayx::EventType::Uninitialized)
        .value("EMITTED", rayx::EventType::Emitted)
        .value("HIT_ELEMENT", rayx::EventType::HitElement)
        .value("FATAL_ERROR", rayx::EventType::FatalError)
        .value("ABSORBED", rayx::EventType::Absorbed)
        .value("BEYOND_HORIZON", rayx::EventType::BeyondHorizon)
        .value("TOO_MANY_EVENTS", rayx::EventType::TooManyEvents);

    py::class_<rayx::Rays>(m, "Rays")
        .def_prop_ro("path_id", [](rayx::Rays& rays) { return to_numpy(rays.path_id); })
        .def_prop_ro("path_event_id", [](rayx::Rays& rays) { return to_numpy(rays.path_event_id); })
        .def_prop_ro(
            "position_x", [](rayx::Rays& rays) { return to_numpy(rays.position_x); }, py::rv_policy::reference_internal)
        .def_prop_ro(
            "position_y", [](rayx::Rays& rays) { return to_numpy(rays.position_y); }, py::rv_policy::reference_internal)
        .def_prop_ro(
            "position_z", [](rayx::Rays& rays) { return to_numpy(rays.position_z); }, py::rv_policy::reference_internal)
        .def_prop_ro(
            "direction_x", [](rayx::Rays& rays) { return to_numpy(rays.direction_x); }, py::rv_policy::reference_internal)
        .def_prop_ro(
            "direction_y", [](rayx::Rays& rays) { return to_numpy(rays.direction_y); }, py::rv_policy::reference_internal)
        .def_prop_ro(
            "direction_z", [](rayx::Rays& rays) { return to_numpy(rays.direction_z); }, py::rv_policy::reference_internal)
        .def_prop_ro(
            "electric_field_x", [](rayx::Rays& rays) { return to_numpy(rays.electric_field_x); }, py::rv_policy::reference_internal)
        .def_prop_ro(
            "electric_field_y", [](rayx::Rays& rays) { return to_numpy(rays.electric_field_y); }, py::rv_policy::reference_internal)
        .def_prop_ro(
            "electric_field_z", [](rayx::Rays& rays) { return to_numpy(rays.electric_field_z); }, py::rv_policy::reference_internal)
        .def_prop_ro(
            "optical_path_length", [](rayx::Rays& rays) { return to_numpy(rays.optical_path_length); }, py::rv_policy::reference_internal)
        .def_prop_ro(
            "energy", [](rayx::Rays& rays) { return to_numpy(rays.energy); }, py::rv_policy::reference_internal)
        .def_prop_ro(
            "order", [](rayx::Rays& rays) { return to_numpy(rays.order); }, py::rv_policy::reference_internal)
        .def_prop_ro(
            "object_id", [](rayx::Rays& rays) { return to_numpy(rays.object_id); }, py::rv_policy::reference_internal)
        .def_prop_ro(
            "source_id", [](rayx::Rays& rays) { return to_numpy(rays.source_id); }, py::rv_policy::reference_internal)
        .def_prop_ro("event_type", [](rayx::Rays& rays) { return to_numpy(rays.event_type); }, py::rv_policy::reference_internal);

    py::class_<rayx::Beamline>(m, "Beamline")
        .def_prop_ro("elements", &rayx::Beamline::getElements)
        .def_prop_ro("sources", &rayx::Beamline::getSources)
        .def("trace",
             [](rayx::Beamline& bl) {
                 rayx::DeviceConfig deviceConfig = rayx::DeviceConfig().enableBestDevice();
                 rayx::Tracer tracer = rayx::Tracer(deviceConfig);
                 rayx::ObjectMask obj_mask = rayx::ObjectMask::all();
                 rayx::RayAttrMask attr_mask = rayx::RayAttrMask::All;
                 rayx::Rays rays = tracer.trace(bl, rayx::Sequential::No, obj_mask, attr_mask, std::nullopt, std::nullopt);
                 return rays;
             })
        .def("__getitem__", [](rayx::Beamline& bl, const std::string& name) {
            for (auto element : bl.getElements()) {
                if (element->getName() == name) {
                    return py::cast(element);
                }
            }
            for (auto source : bl.getSources()) {
                if (source->getName() == name) {
                    return py::cast(source);
                }
            }
            throw std::runtime_error("No element or source with name '" + name + "' found in beamline.");
        });

    m.def("import_beamline", [](std::string path) { return rayx::importBeamline(path); }, "Import a beamline from an RML file", py::arg("path"));
}
