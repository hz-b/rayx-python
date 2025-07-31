#include <Core.h>
#include <Rml/Importer.h>
#include <Rml/Locate.h>
#include <Tracer/Tracer.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <filesystem>

using namespace pybind11::literals;

std::complex<double> toStdComplex(const RAYX::complex::Complex& c) { return std::complex<double>(c.real(), c.imag()); }

std::filesystem::path getModulePath() {
    pybind11::gil_scoped_acquire acquire;
    pybind11::object rayx = pybind11::module::import("rayxdata");
    return std::filesystem::path(rayx.attr("__file__").cast<std::string>()).parent_path();
}

struct Pos {
    glm::dvec4 position;
    std::function<void(glm::dvec4)> setter;

    Pos(glm::dvec4 data, std::function<void(glm::dvec4)> setter) : position(data), setter(setter) {}

    void setX(double x) {
        position.x = x;
        setter(position);
    }

    double getX() { return position.x; }

    void setY(double y) {
        position.y = y;
        setter(position);
    }

    double getY() { return position.y; }

    void setZ(double z) {
        position.z = z;
        setter(position);
    }

    double getZ() { return position.z; }

    void setW(double w) {
        position.w = w;
        setter(position);
    }

    double getW() { return position.w; }
};

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
    data_buffer<RAYX::Order> order;
    data_buffer<RAYX::EventType> event_type;
    data_buffer<RAYX::Order> last_element_id;
    data_buffer<RAYX::Order> source_id;
    data_buffer<int> ray_id;
    data_buffer<int> event_id;

    Rays(RAYX::RaySoA&& rays) {
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
        path_length.data = std::move(rays.path_length);
        order.data = std::move(rays.order);
        event_type.data = std::move(rays.event_type);
        last_element_id.data = std::move(rays.element_id);
        source_id.data = std::move(rays.source_id);
        ray_id.data = std::move(rays.path_id);
    }

    Rays(RAYX::BundleHistory& bundles) {
        size_t n_rays = 0;
        for (auto&& bundle : bundles) {
            n_rays += bundle.size();
        }

        position_x.data.resize(n_rays);
        position_y.data.resize(n_rays);
        position_z.data.resize(n_rays);
        direction_x.data.resize(n_rays);
        direction_y.data.resize(n_rays);
        direction_z.data.resize(n_rays);
        energy.data.resize(n_rays);
        electric_field_x.data.resize(n_rays);
        electric_field_y.data.resize(n_rays);
        electric_field_z.data.resize(n_rays);
        path_length.data.resize(n_rays);
        order.data.resize(n_rays);
        event_type.data.resize(n_rays);
        last_element_id.data.resize(n_rays);
        source_id.data.resize(n_rays);
        ray_id.data.resize(n_rays);
        event_id.data.resize(n_rays);

        size_t index = 0;
        for (size_t i = 0; i < bundles.size(); i++) {
            for (size_t j = 0; j < bundles[i].size(); j++) {
                RAYX::Ray ray = bundles[i][j];
                position_x.data[index] = ray.m_position.x;
                position_y.data[index] = ray.m_position.y;
                position_z.data[index] = ray.m_position.z;
                direction_x.data[index] = ray.m_direction.x;
                direction_y.data[index] = ray.m_direction.y;
                direction_z.data[index] = ray.m_direction.z;
                energy.data[index] = ray.m_energy;
                electric_field_x.data[index] = toStdComplex(ray.m_field.x);
                electric_field_y.data[index] = toStdComplex(ray.m_field.y);
                electric_field_z.data[index] = toStdComplex(ray.m_field.z);
                path_length.data[index] = ray.m_pathLength;
                order.data[index] = ray.m_order;
                event_type.data[index] = ray.m_eventType;
                last_element_id.data[index] = ray.m_lastElement;
                source_id.data[index] = ray.m_sourceID;
                ray_id.data[index] = i;
                event_id.data[index] = j;
                index++;
            }
        }
    };
};

// std::string elementTypeToString(RAYX::ElementType type) { return RAYX::ElementStringMap.find(type)->second; }

class Module {
  public:
    Module() {
        std::filesystem::path data_dir = getModulePath();
        RAYX::ResourceHandler::getInstance().addLookUpPath(data_dir);
    }
};

PYBIND11_MODULE(rayx, m) {
    static Module module_instance;

    m.doc() = "rayx module";

    // Add bindings here
    pybind11::class_<Pos>(m, "position")
        .def_property("x", &Pos::getX, &Pos::setX)
        .def_property("y", &Pos::getY, &Pos::setY)
        .def_property("z", &Pos::getZ, &Pos::setZ)
        .def_property("w", &Pos::getW, &Pos::setW);

    pybind11::class_<glm::dvec3>(m, "dvec3").def_readwrite("x", &glm::dvec3::x).def_readwrite("y", &glm::dvec3::y).def_readwrite("z", &glm::dvec3::z);

    pybind11::class_<glm::dvec4>(m, "dvec4")
        .def_readwrite("x", &glm::dvec4::x)
        .def_readwrite("y", &glm::dvec4::y)
        .def_readwrite("z", &glm::dvec4::z)
        .def_readwrite("w", &glm::dvec4::w);

    pybind11::enum_<RAYX::Material>(m, "Material")
        .value("VACUUM", RAYX::Material::VACUUM)
        .value("REFLECTIVE", RAYX::Material::REFLECTIVE)
#define X(e, z, a, rho) .value(#e, RAYX::Material::e)
#include <Material/materials.xmacro>
#undef X
        .export_values();

    pybind11::enum_<RAYX::SourceDist>(m, "SourceDist")
        .value("UNIFORM", RAYX::SourceDist::Uniform)
        .value("GAUSSIAN", RAYX::SourceDist::Gaussian)
        .value("THIRDS", RAYX::SourceDist::Thirds)
        .value("CIRCLE", RAYX::SourceDist::Circle)
        .export_values();

    pybind11::enum_<RAYX::SpreadType>(m, "SpreadType")
        .value("HARD_EDGE", RAYX::SpreadType::HardEdge)
        .value("SOFT_EDGE", RAYX::SpreadType::SoftEdge)
        .value("SEPARATE_ENERGIES", RAYX::SpreadType::SeparateEnergies)
        .export_values();

    pybind11::enum_<RAYX::EnergyDistributionType>(m, "EnergyDistributionType")
        .value("FILE", RAYX::EnergyDistributionType::File)
        .value("VALUES", RAYX::EnergyDistributionType::Values)
        .value("TOTAL", RAYX::EnergyDistributionType::Total)
        .value("PARAM", RAYX::EnergyDistributionType::Param)
        .export_values();

    pybind11::enum_<RAYX::EnergySpreadUnit>(m, "EnergySpreadUnit")
        .value("EV", RAYX::EnergySpreadUnit::EU_eV)
        .value("PERCENT", RAYX::EnergySpreadUnit::EU_PERCENT)
        .export_values();

    pybind11::enum_<RAYX::ElectronEnergyOrientation>(m, "ElectronEnergyOrientation")
        .value("Clockwise", RAYX::ElectronEnergyOrientation::Clockwise)
        .value("Counterclockwise", RAYX::ElectronEnergyOrientation::Counterclockwise)
        .export_values();

    pybind11::class_<data_buffer<double>>(m, "double_array", pybind11::buffer_protocol())
        .def_buffer([](data_buffer<double>& db) { return db.buffer_info(); })
        .def("as_numpy", &data_buffer<double>::as_numpy_array);
    pybind11::class_<data_buffer<std::complex<double>>>(m, "complex_array", pybind11::buffer_protocol())
        .def_buffer([](data_buffer<std::complex<double>>& db) { return db.buffer_info(); })
        .def("as_numpy", &data_buffer<std::complex<double>>::as_numpy_array);
    pybind11::class_<data_buffer<int>>(m, "int_array", pybind11::buffer_protocol())
        .def_buffer([](data_buffer<int>& db) { return db.buffer_info(); })
        .def("as_numpy", &data_buffer<int>::as_numpy_array);
    pybind11::class_<data_buffer<RAYX::Order>>(m, "order_array", pybind11::buffer_protocol())
        .def_buffer([](data_buffer<RAYX::Order>& db) { return db.buffer_info(); })
        .def("as_numpy", &data_buffer<RAYX::Order>::as_numpy_array);
    pybind11::class_<data_buffer<RAYX::EventType>>(m, "event_type_array", pybind11::buffer_protocol())
        .def_buffer([](data_buffer<RAYX::EventType>& db) { return db.buffer_info(); })
        .def("as_numpy", &data_buffer<RAYX::EventType>::as_numpy_array);
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
        .def_property_readonly("event_id", [](const Rays& r) { return r.event_id.as_numpy_array(); });

    pybind11::class_<RAYX::DesignElement>(m, "Element")
        .def_property_readonly("name", &RAYX::DesignElement::getName)
        .def("__repr__",
             [](RAYX::DesignElement& e) { return "Element(name = " + e.getName() + ", type = " + RAYX::elementTypeToString(e.getType()) + ")"; })
        .def_property_readonly("position",
                               [](RAYX::DesignElement& e) {
                                   Pos p(e.getPosition(), [&](glm::dvec4 pos) { e.setPosition(pos); });
                                   return p;
                               })
        .def_property(
            "position_x", [](RAYX::DesignElement& e) { return e.getPosition().x; },
            [](RAYX::DesignElement& e, double x) {
                glm::dvec4 pos = e.getPosition();
                pos.x = x;
                e.setPosition(pos);
            })
        .def_property(
            "position_y", [](RAYX::DesignElement& e) { return e.getPosition().y; },
            [](RAYX::DesignElement& e, double y) {
                glm::dvec4 pos = e.getPosition();
                pos.y = y;
                e.setPosition(pos);
            })
        .def_property(
            "position_z", [](RAYX::DesignElement& e) { return e.getPosition().z; },
            [](RAYX::DesignElement& e, double z) {
                glm::dvec4 pos = e.getPosition();
                pos.z = z;
                e.setPosition(pos);
            })
        .def_property(
            "misalignment_translation_x", [](RAYX::DesignElement& e) { return e.getMisalignment().m_translationXerror; },
            [](RAYX::DesignElement& e, double x) {
                RAYX::Misalignment m = e.getMisalignment();
                m.m_translationXerror = x;
                e.setMisalignment(m);
            })
        .def_property(
            "misalignment_translation_y", [](RAYX::DesignElement& e) { return e.getMisalignment().m_translationYerror; },
            [](RAYX::DesignElement& e, double y) {
                RAYX::Misalignment m = e.getMisalignment();
                m.m_translationYerror = y;
                e.setMisalignment(m);
            })
        .def_property(
            "misalignment_translation_z", [](RAYX::DesignElement& e) { return e.getMisalignment().m_translationZerror; },
            [](RAYX::DesignElement& e, double z) {
                RAYX::Misalignment m = e.getMisalignment();
                m.m_translationZerror = z;
                e.setMisalignment(m);
            })
        .def_property(
            "misalignment_rotation_x", [](RAYX::DesignElement& e) { return e.getMisalignment().m_rotationXerror.rad; },
            [](RAYX::DesignElement& e, double x) {
                RAYX::Misalignment m = e.getMisalignment();
                m.m_rotationXerror.rad = x;
                e.setMisalignment(m);
            })
        .def_property(
            "misalignment_rotation_y", [](RAYX::DesignElement& e) { return e.getMisalignment().m_rotationYerror.rad; },
            [](RAYX::DesignElement& e, double y) {
                RAYX::Misalignment m = e.getMisalignment();
                m.m_rotationYerror.rad = y;
                e.setMisalignment(m);
            })
        .def_property(
            "misalignment_rotation_z", [](RAYX::DesignElement& e) { return e.getMisalignment().m_rotationZerror.rad; },
            [](RAYX::DesignElement& e, double z) {
                RAYX::Misalignment m = e.getMisalignment();
                m.m_rotationZerror.rad = z;
                e.setMisalignment(m);
            })
        .def_property(
            "slope_error_sag", [](RAYX::DesignElement& e) { return e.getSlopeError().m_sag; },
            [](RAYX::DesignElement& e, double s) {
                RAYX::SlopeError se = e.getSlopeError();
                se.m_sag = s;
                e.setSlopeError(se);
            })
        .def_property(
            "slope_error_mer", [](RAYX::DesignElement& e) { return e.getSlopeError().m_mer; },
            [](RAYX::DesignElement& e, double m) {
                RAYX::SlopeError se = e.getSlopeError();
                se.m_mer = m;
                e.setSlopeError(se);
            })
        .def_property(
            "slope_error_thermal_distortion_amp", [](RAYX::DesignElement& e) { return e.getSlopeError().m_thermalDistortionAmp; },
            [](RAYX::DesignElement& e, double tda) {
                RAYX::SlopeError se = e.getSlopeError();
                se.m_thermalDistortionAmp = tda;
                e.setSlopeError(se);
            })
        .def_property(
            "slope_error_thermal_distortion_sigma_x", [](RAYX::DesignElement& e) { return e.getSlopeError().m_thermalDistortionSigmaX; },
            [](RAYX::DesignElement& e, double tdsx) {
                RAYX::SlopeError se = e.getSlopeError();
                se.m_thermalDistortionSigmaX = tdsx;
                e.setSlopeError(se);
            })
        .def_property(
            "slope_error_thermal_distortion_sigma_z", [](RAYX::DesignElement& e) { return e.getSlopeError().m_thermalDistortionSigmaZ; },
            [](RAYX::DesignElement& e, double tdsz) {
                RAYX::SlopeError se = e.getSlopeError();
                se.m_thermalDistortionSigmaZ = tdsz;
                e.setSlopeError(se);
            })
        .def_property(
            "slope_error_cylindrical_bowing_amp", [](RAYX::DesignElement& e) { return e.getSlopeError().m_cylindricalBowingAmp; },
            [](RAYX::DesignElement& e, double cba) {
                RAYX::SlopeError se = e.getSlopeError();
                se.m_cylindricalBowingAmp = cba;
                e.setSlopeError(se);
            })
        .def_property(
            "slope_error_cylindrical_bowing_radius", [](RAYX::DesignElement& e) { return e.getSlopeError().m_cylindricalBowingRadius; },
            [](RAYX::DesignElement& e, double cbr) {
                RAYX::SlopeError se = e.getSlopeError();
                se.m_cylindricalBowingRadius = cbr;
                e.setSlopeError(se);
            })
        .def_property(
            "azimuthal_angle", [](RAYX::DesignElement& e) { return e.getAzimuthalAngle(); },
            [](RAYX::DesignElement& e, double a) { e.setAzimuthalAngle(a); })
        .def_property(
            "material", [](RAYX::DesignElement& e) { return e.getMaterial(); }, [](RAYX::DesignElement& e, RAYX::Material m) { e.setMaterial(m); })
        .def_property_readonly("orientation", &RAYX::DesignElement::getOrientation);
    pybind11::class_<RAYX::DesignSource>(m, "Source")
        .def_property_readonly("name", &RAYX::DesignSource::getName)
        .def_property(
            "position_x", [](RAYX::DesignSource& s) { return s.getPosition().x; },
            [](RAYX::DesignSource& s, double x) {
                glm::dvec4 pos = s.getPosition();
                pos.x = x;
                s.setPosition(pos);
            })
        .def_property(
            "position_y", [](RAYX::DesignSource& s) { return s.getPosition().y; },
            [](RAYX::DesignSource& s, double y) {
                glm::dvec4 pos = s.getPosition();
                pos.y = y;
                s.setPosition(pos);
            })
        .def_property(
            "position_z", [](RAYX::DesignSource& s) { return s.getPosition().z; },
            [](RAYX::DesignSource& s, double z) {
                glm::dvec4 pos = s.getPosition();
                pos.z = z;
                s.setPosition(pos);
            })
        .def_property(
            "misalignment_translation_x", [](RAYX::DesignSource& s) { return s.getMisalignment().m_translationXerror; },
            [](RAYX::DesignSource& s, double x) {
                RAYX::Misalignment m = s.getMisalignment();
                m.m_translationXerror = x;
                s.setMisalignment(m);
            })
        .def_property(
            "misalignment_translation_y", [](RAYX::DesignSource& s) { return s.getMisalignment().m_translationYerror; },
            [](RAYX::DesignSource& s, double y) {
                RAYX::Misalignment m = s.getMisalignment();
                m.m_translationYerror = y;
                s.setMisalignment(m);
            })
        .def_property(
            "misalignment_translation_z", [](RAYX::DesignSource& s) { return s.getMisalignment().m_translationZerror; },
            [](RAYX::DesignSource& s, double z) {
                RAYX::Misalignment m = s.getMisalignment();
                m.m_translationZerror = z;
                s.setMisalignment(m);
            })
        .def_property(
            "misalignment_rotation_x", [](RAYX::DesignSource& s) { return s.getMisalignment().m_rotationXerror.rad; },
            [](RAYX::DesignSource& s, double x) {
                RAYX::Misalignment m = s.getMisalignment();
                m.m_rotationXerror.rad = x;
                s.setMisalignment(m);
            })
        .def_property(
            "misalignment_rotation_y", [](RAYX::DesignSource& s) { return s.getMisalignment().m_rotationYerror.rad; },
            [](RAYX::DesignSource& s, double y) {
                RAYX::Misalignment m = s.getMisalignment();
                m.m_rotationYerror.rad = y;
                s.setMisalignment(m);
            })
        .def_property(
            "misalignment_rotation_z", [](RAYX::DesignSource& s) { return s.getMisalignment().m_rotationZerror.rad; },
            [](RAYX::DesignSource& s, double z) {
                RAYX::Misalignment m = s.getMisalignment();
                m.m_rotationZerror.rad = z;
                s.setMisalignment(m);
            })
        .def_property("width_distribution", &RAYX::DesignSource::getWidthDist, &RAYX::DesignSource::setWidthDist)
        .def_property("height_distribution", &RAYX::DesignSource::getHeightDist, &RAYX::DesignSource::setHeightDist)
        .def_property("horizontal_distribution", &RAYX::DesignSource::getHorDist, &RAYX::DesignSource::setHorDist)
        .def_property("vertical_distribution", &RAYX::DesignSource::getVerDist, &RAYX::DesignSource::setVerDist)
        .def_property("horizontal_divergence", &RAYX::DesignSource::getHorDivergence, &RAYX::DesignSource::setHorDivergence)
        .def_property("vertical_divergence", &RAYX::DesignSource::getVerDivergence, &RAYX::DesignSource::setVerDivergence)
        .def_property("vertical_electron_beam_divergence", &RAYX::DesignSource::getVerEBeamDivergence, &RAYX::DesignSource::setVerEBeamDivergence)
        .def_property("source_depth", &RAYX::DesignSource::getSourceDepth, &RAYX::DesignSource::setSourceDepth)
        .def_property("source_height", &RAYX::DesignSource::getSourceHeight, &RAYX::DesignSource::setSourceHeight)
        .def_property("source_width", &RAYX::DesignSource::getSourceWidth, &RAYX::DesignSource::setSourceWidth)
        .def_property("bending_radius", &RAYX::DesignSource::getBendingRadius, &RAYX::DesignSource::setBendingRadius)
        .def_property("energy_spread", &RAYX::DesignSource::getEnergySpread, &RAYX::DesignSource::setEnergySpread)
        .def_property("energy_spread_type", &RAYX::DesignSource::getEnergySpreadType, &RAYX::DesignSource::setEnergySpreadType)
        .def("set_energy_distribution_file", &RAYX::DesignSource::setEnergyDistributionFile)
        .def_property("energy_spread_unit", &RAYX::DesignSource::getEnergySpreadUnit, &RAYX::DesignSource::setEnergySpreadUnit)
        .def_property("electron_energy", &RAYX::DesignSource::getElectronEnergy, &RAYX::DesignSource::setElectronEnergy)
        .def_property("electron_energy_orientation", &RAYX::DesignSource::getElectronEnergyOrientation,
                      &RAYX::DesignSource::setElectronEnergyOrientation)
        .def_property("energy", &RAYX::DesignSource::getEnergy, &RAYX::DesignSource::setEnergy)
        .def_property("photon_flux", &RAYX::DesignSource::getPhotonFlux, &RAYX::DesignSource::setPhotonFlux)
        .def_property("number_of_rays", &RAYX::DesignSource::getNumberOfRays, &RAYX::DesignSource::setNumberOfRays)
        .def("__repr__",
             [](const RAYX::DesignSource& s) { return "Source(name = " + s.getName() + ", type = " + RAYX::elementTypeToString(s.getType()) + ")"; });

    pybind11::class_<RAYX::Beamline>(m, "Beamline")
        .def_property_readonly("elements", &RAYX::Beamline::getElements)
        .def_property_readonly("sources", &RAYX::Beamline::getSources)
        .def("trace", [](RAYX::Beamline& bl) {
            RAYX::DeviceConfig deviceConfig = RAYX::DeviceConfig().enableBestDevice();
            RAYX::Tracer tracer = RAYX::Tracer(deviceConfig);
            RAYX::RaySoA s_of_a = tracer.trace(bl, RAYX::Sequential::No, 10000, 10, -1);
            // RAYX::BundleHistory history = RAYX::raySoAToBundleHistory(s_of_a);
            return Rays(std::move(s_of_a));
        });

    m.def(
        "import_beamline", [](std::string path) { return RAYX::importBeamline(path); }, "Import a beamline from an RML file", pybind11::arg("path"));
}